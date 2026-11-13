#include "larg4/pluginActions/ParticleListAction_service.h"

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "nug4/G4Base/PrimaryParticleInformation.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "Geant4/G4Decay.hh"
#include "Geant4/G4DynamicParticle.hh"
#include "Geant4/G4PrimaryParticle.hh"
#include "Geant4/G4Proton.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4StepPoint.hh"
#include "Geant4/G4Track.hh"

#include <cmath>
#include <stdexcept>
#include <utility>
#include <vector>

namespace larg4::test {
  class NoStepPrimaryCheck : public art::EDProducer {
  public:
    explicit NoStepPrimaryCheck(fhicl::ParameterSet const& config)
      : EDProducer{config}, fullTrajectory_{config.get<bool>("FullTrajectory", false)}
    {
      consumes<std::vector<simb::MCTruth>>(art::InputTag{"truth"});
      produces<std::vector<simb::MCParticle>>();
      produces<art::Assns<simb::MCTruth, simb::MCParticle, sim::GeneratedParticleInfo>>();
    }

    void produce(art::Event& event) override
    {
      auto const truth = event.getValidHandle<std::vector<simb::MCTruth>>(art::InputTag{"truth"});
      std::vector<art::Handle<std::vector<simb::MCTruth>>> const handles{
        event.getHandle<std::vector<simb::MCTruth>>(art::InputTag{"truth"})};
      fhicl::ParameterSet config;
      config.put("EnergyCut", 0.01 * CLHEP::MeV);
      config.put("keepEMShowerDaughters", true);
      config.put("storeTrajectories", fullTrajectory_);
      ParticleListActionService action{config};
      action.setInputCollections(handles);
      auto const pid = event.getProductID<std::vector<simb::MCParticle>>();
      action.setPtrInfo(pid, event.productGetter(pid));
      G4Decay transportation{"Transportation"};
      G4Decay decay;

      auto const track =
        [&](
          int id, int parent, int generatedIndex, bool hasProcess, bool clearFinalProcess = false) {
          G4PrimaryParticle primary{G4Proton::Definition()};
          if (parent == 0) {
            auto* info = new g4b::PrimaryParticleInformation;
            info->SetMCTruth(&truth->front(), 0, generatedIndex);
            primary.SetUserInformation(info);
          }
          auto* dynamic = new G4DynamicParticle{G4Proton::Definition(),
                                                G4ThreeVector{0., 0., 1.},
                                                hasProcess || parent ? 1. * CLHEP::MeV : 0.};
          if (parent == 0) dynamic->SetPrimaryParticle(&primary);
          G4Track current{dynamic, 11. * CLHEP::ns, G4ThreeVector{10., 20., 30.} * CLHEP::mm};
          current.SetTrackID(id);
          current.SetParentID(parent);
          current.SetWeight(2.5);
          if (parent) current.SetCreatorProcess(&decay);
          G4Step step;
          step.SetTrack(&current);
          for (auto* point : {step.GetPreStepPoint(), step.GetPostStepPoint()}) {
            point->SetPosition(current.GetPosition());
            point->SetGlobalTime(current.GetGlobalTime());
            point->SetMass(dynamic->GetMass());
            point->SetKineticEnergy(dynamic->GetKineticEnergy());
            point->SetMomentumDirection(dynamic->GetMomentumDirection());
          }
          current.SetStep(&step);

          action.preUserTrackingAction(&current);
          if (hasProcess) {
            step.GetPostStepPoint()->SetProcessDefinedStep(&transportation);
            step.GetPostStepPoint()->SetPosition(G4ThreeVector{10., 20., 31.} * CLHEP::mm);
            step.GetPostStepPoint()->SetGlobalTime(12. * CLHEP::ns);
            action.userSteppingAction(&step);
          }
          if (clearFinalProcess) step.GetPostStepPoint()->SetProcessDefinedStep(nullptr);
          action.postUserTrackingAction(&current);
        };

      // A primary with no step process must survive alongside an ordinary primary.
      action.beginOfEventAction(nullptr);
      track(1, 0, 0, false);
      track(2, 0, 1, true);
      track(3, 0, 2, true, true); // Existing trajectory must not gain a fabricated endpoint.
      track(4, 2, -1, false);     // Existing no-process secondary exclusion is unchanged.
      action.endOfEventAction(nullptr);
      auto particles = action.ParticleCollection();
      auto associations = action.AssnsMCTruthToMCParticle();
      if (particles->size() != 3 || associations->size() != 3)
        throw std::runtime_error("Missing generated primary after no-process tracking");

      auto const& stationary = particles->at(0);
      auto const& moving = particles->at(1);
      if (stationary.TrackId() != 1 || stationary.Mother() != 0 || stationary.PdgCode() != 2212 ||
          stationary.NumberTrajectoryPoints() != 1 || stationary.P() != 0. ||
          std::abs(stationary.E() - stationary.Mass()) > 1.e-12 || stationary.Vx() != 1. ||
          stationary.Vy() != 2. || stationary.Vz() != 3. || stationary.T() != 11. ||
          stationary.Weight() != 2.5 || stationary.EndProcess() != simb::MCParticle{}.EndProcess())
        throw std::runtime_error("No-process primary has an invented or incomplete state");
      if (moving.TrackId() != 2 || moving.NumberTrajectoryPoints() != 2 ||
          moving.EndProcess() != "Transportation")
        throw std::runtime_error("Ordinary transported primary changed");
      auto const& existingTrajectory = particles->at(2);
      if (existingTrajectory.NumberTrajectoryPoints() != (fullTrajectory_ ? 2u : 1u) ||
          existingTrajectory.EndPosition() !=
            (fullTrajectory_ ? moving.EndPosition() : moving.Position()) ||
          existingTrajectory.EndProcess() != simb::MCParticle{}.EndProcess())
        throw std::runtime_error("No-process primary changed an existing trajectory");

      for (std::size_t index = 0; index < associations->size(); ++index) {
        auto const& pair = associations->at(index);
        auto const& info = associations->data(index);
        if (pair.first.id() != truth.id() || pair.first.key() != 0 || pair.second.id() != pid ||
            pair.second.key() != index || !info.hasGeneratedParticleIndex() ||
            info.generatedParticleIndex() != index)
          throw std::runtime_error("Primary generator association was lost");
      }

      // Reusing track IDs in another event must not retain old generator indices.
      action.beginOfEventAction(nullptr);
      track(1, 0, 1, false);
      action.endOfEventAction(nullptr);
      if (action.GetPrimaryTruthIndex(1) != 1 || action.ParticleCollection()->size() != 1)
        throw std::runtime_error("Primary state leaked across event boundaries");

      event.put(std::move(particles));
      event.put(std::move(associations));
    }

  private:
    bool const fullTrajectory_;
  };
}

DEFINE_ART_MODULE(larg4::test::NoStepPrimaryCheck)
