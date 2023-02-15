// C++ includes.
#include <algorithm>
#include <cmath>
#include <iostream>
#include <set>
#include <string>

// Framework includes.
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Provenance.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art_root_io/TFileDirectory.h"
#include "art_root_io/TFileService.h"
#include "nusimdata/SimulationBase/MCParticle.h"
#include "lardataobj/Simulation/ParticleAncestryMap.h"

// Root includes.
#include "TDirectory.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

// Other includes.
#include "CLHEP/Units/SystemOfUnits.h"
//#define _verbose_ 1
using namespace std;
namespace larg4 {
  class CheckMCParticle;
}

class larg4::CheckMCParticle : public art::EDAnalyzer {
public:
  explicit CheckMCParticle(fhicl::ParameterSet const& p);

private:
  void beginJob() override;
  void analyze(const art::Event& event) override;

  std::string const _myName;
  TH1F* _hnParts{nullptr};
};

larg4::CheckMCParticle::CheckMCParticle(fhicl::ParameterSet const& p)
  : art::EDAnalyzer(p), _myName(p.get<std::string>("name", "CheckMCParticle"))
{}

void larg4::CheckMCParticle::beginJob()
{
  art::ServiceHandle<art::TFileService const> tfs;
  _hnParts = tfs->make<TH1F>("hnParts", "Number of generated Particles", 100, 0., 2000.);
} // end beginJob

void larg4::CheckMCParticle::analyze(const art::Event& event)
{
#if defined _verbose_

  auto allDropped = event.getMany<sim::ParticleAncestryMap>();

  for (auto const& maps : allDropped) {
    auto const& map = maps->GetMap();
    for (auto const& [parent, daughters] : map) {
      std::cout << "Parent of dropped Tracks: " << parent << std::endl;
      std::cout << " droppedid size:  " << daughters.size() << std::endl;
      for (auto const& droppedid : daughters) {
        std::cout << droppedid << " ";
      }
      std::cout << std::endl;
    }
  }

#endif
  auto allGens = event.getMany<std::vector<simb::MCParticle>>();
  for (auto const& gens : allGens) {
    _hnParts->Fill(gens->size());
#if defined _verbose_
    for (auto const& genpart : *gens) {
      if (genpart.Mother() == 0) {
        cout << "Primary momentum:  " << genpart.P();
        cout << "  position:  " << genpart.Vx() << "  " << genpart.Vy() << "  " << genpart.Vz()
             << endl;
      }
      cout << "Part id:  " << genpart.TrackId();
      cout << " PDG id:  " << genpart.PdgCode();
      cout << " Status Code:  " << genpart.StatusCode();
      cout << " Mother:  " << genpart.Mother();
      cout << " Creation Process: " << genpart.Process();
      cout << " End Process: " << genpart.EndProcess();
      /*
      auto trajectory = genpart.Trajectory();
      cout <<" trajectory size:   " << trajectory.size();
      */
      cout << " Nr. of Daughters: " << genpart.NumberDaughters();
      cout << " FirstDaughter:" << genpart.FirstDaughter() << endl;
      //      cout <<" LastDaughter: " << genpart.LastDaughter() <<endl;
      for (int i = 0; i < genpart.NumberDaughters(); i++) {
        cout << genpart.Daughter(i) << ",";
      }
      cout << endl;
    }
#endif
  }
} // end analyze

DEFINE_ART_MODULE(larg4::CheckMCParticle)
