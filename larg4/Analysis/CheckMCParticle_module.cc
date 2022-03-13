// C++ includes.
#include <iostream>
#include <string>
#include <set>
#include <cmath>
#include <algorithm>

// Framework includes.
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Run.h"
#include "art_root_io/TFileDirectory.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Principal/Provenance.h"
#include "nusimdata/SimulationBase/MCParticle.h"

// Root includes.
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TDirectory.h"

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

larg4::CheckMCParticle::CheckMCParticle(fhicl::ParameterSet const& p) :
  art::EDAnalyzer(p),
  _myName(p.get<std::string>("name", "CheckMCParticle"))
{}

void larg4::CheckMCParticle::beginJob()
{
  art::ServiceHandle<art::TFileService const> tfs;
  _hnParts = tfs->make<TH1F>("hnParts", "Number of generated Particles", 100, 0., 2000.);
} // end beginJob

void larg4::CheckMCParticle::analyze(const art::Event& event)
{
#if defined _verbose_
  /*
  auto allDropped =  event.getMany<std::set<int>>();
  for (auto const& dropped : allDropped) {
    
    std::cout << "List of dropped Tracks: "<<std::endl;
    for(auto const& droppedTrack :  *dropped )
      {
	std::cout << droppedTrack << " ";
      }
    std::cout <<std::endl;   
  }
  */
#endif
  auto allGens = event.getMany<std::vector<simb::MCParticle>>();
  for (auto const& gens : allGens) {
    _hnParts->Fill(gens->size());
#if defined _verbose_
    for (auto const& genpart : *gens) {
      if (genpart.Mother()==0) {
        cout << "Primary momentum:  " <<   genpart.P();
        cout << "  position:  " << genpart.Vx()<< "  "<< genpart.Vy()<<"  "<< genpart.Vz()  << endl;
      }
      cout << "Part id:  " << genpart.TrackId();
      cout << " PDG id:  " << genpart.PdgCode();
      cout << " Status Code:  " << genpart.StatusCode();
      cout << " Mother:  " << genpart.Mother();
      cout << " Creation Process: "<<genpart.Process();
      cout << " End Process: "<<genpart.EndProcess();
      /*
      auto trajectory = genpart.Trajectory();
      cout <<" trajectory size:   " << trajectory.size();
      */
      cout <<" Nr. of Daughters: " << genpart.NumberDaughters();
      cout <<" FirstDaughter:" << genpart.FirstDaughter()<<endl;
      //      cout <<" LastDaughter: " << genpart.LastDaughter() <<endl;
      for (int i=0;i<genpart.NumberDaughters();i++)
      	{
	  cout << genpart.Daughter(i)<<",";
      	}
      cout<<endl;
    }
#endif
  }
} // end analyze

DEFINE_ART_MODULE(larg4::CheckMCParticle)
