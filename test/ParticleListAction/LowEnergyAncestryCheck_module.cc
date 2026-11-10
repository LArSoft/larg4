#include "larg4/pluginActions/ParticleListAction_service.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "lardataobj/Simulation/sim.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "Geant4/G4Decay.hh"
#include "Geant4/G4DynamicParticle.hh"
#include "Geant4/G4Electron.hh"
#include "Geant4/G4Track.hh"

#include <map>
#include <set>
#include <stdexcept>
#include <vector>

namespace larg4::test {
  class LowEnergyAncestryCheck : public art::EDAnalyzer {
  public:
    explicit LowEnergyAncestryCheck(fhicl::ParameterSet const& config)
      : EDAnalyzer{config}, keepEM_{config.get<bool>("KeepEMShowerDaughters", false)}
    {
      consumes<std::vector<simb::MCTruth>>(art::InputTag{"truth"});
    }

    void analyze(art::Event const& event) override
    {
      std::vector<art::Handle<std::vector<simb::MCTruth>>> const truths{
        event.getHandle<std::vector<simb::MCTruth>>(art::InputTag{"truth"})};
      G4Decay decay;

      fhicl::ParameterSet config;
      config.put("EnergyCut", 0.01 * CLHEP::MeV);
      config.put("keepEMShowerDaughters", keepEM_);
      config.put("storeTrajectories", false);
      ParticleListActionService action{config};
      action.setInputCollections(truths);

      auto const drop = [&](int trackID, int parentID) {
        auto* particle = new G4DynamicParticle{
          G4Electron::Definition(), G4ThreeVector{0., 0., 1.}, 0.005 * CLHEP::MeV};
        G4Track track{particle, 0., G4ThreeVector{}};
        track.SetTrackID(trackID);
        track.SetParentID(parentID);
        track.SetCreatorProcess(&decay);
        action.preUserTrackingAction(&track);
      };

      // Parent 1 is the terminal ancestor. Both a child and its child are cut.
      action.beginOfEventAction(nullptr);
      drop(2, 1);
      drop(3, 2);
      auto const targets = action.GetTargetIDMap();
      if (targets.at(2) != -1 || targets.at(3) != -1)
        throw std::runtime_error("Low-energy deposits lost their assigned ancestor");

      action.endOfEventAction(nullptr);
      auto const map = action.DroppedTracksCollection();
      std::map<int, std::set<int>> const expected{{1, {2, 3}}};
      if (map->GetMap() != expected || map->GetMap().count(sim::NoParticleId))
        throw std::runtime_error("Low-energy suppression map disagrees with deposit ancestry");
      if (!action.ParticleCollection()->empty())
        throw std::runtime_error("Subthreshold tracks should remain excluded from the inventory");

      // A new event can reuse track IDs without inheriting the previous edges.
      action.beginOfEventAction(nullptr);
      drop(2, 7);
      action.endOfEventAction(nullptr);
      if (action.DroppedTracksCollection()->GetMap() != std::map<int, std::set<int>>{{7, {2}}})
        throw std::runtime_error("Suppression ancestry leaked across event boundaries");
    }

  private:
    bool const keepEM_;
  };
}

DEFINE_ART_MODULE(larg4::test::LowEnergyAncestryCheck)
