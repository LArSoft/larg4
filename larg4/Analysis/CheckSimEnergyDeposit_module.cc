// art Framework includes.
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art_root_io/TFileService.h"
#include "art_root_io/TFileDirectory.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "artg4tk/services/DetectorHolder_service.hh"
#include "lardataobj/Simulation/SimEnergyDeposit.h"

// Root includes.
#include "TH1F.h"
#include "TNtuple.h"

// STL includes.
#include <cmath>

// Other includes.
#include "CLHEP/Units/SystemOfUnits.h"

using namespace std;
namespace larg4 {
    class CheckSimEnergyDeposit;
}

class larg4::CheckSimEnergyDeposit : public art::EDAnalyzer {
public:

  explicit CheckSimEnergyDeposit(fhicl::ParameterSet const& p);

private:
  void beginJob() override;
  void analyze(const art::Event& event) override;

  TH1F* _hnHits{nullptr};         // number of SimEnergyDepositHits
  TH1F* _hEdep{nullptr};          // average energy deposition in SimEnergyDepositHits
  TH1F* _hnumPhotons{nullptr};    // number of Photons per SimEnergyDepositHits
  TH1F* _hLandauPhotons{nullptr}; // Edep/cm  SimEnergyDepositHits
  TH1F* _hLandauEdep{nullptr};    // number of Photons/cm SimEnergyDepositHits
  TH1F* _hSteplength{nullptr};    // Geant 4 step length
  TNtuple* _ntuple{nullptr};
};

larg4::CheckSimEnergyDeposit::CheckSimEnergyDeposit(fhicl::ParameterSet const& p) :
  art::EDAnalyzer(p)
{}

void larg4::CheckSimEnergyDeposit::beginJob()
{
  art::ServiceHandle<art::TFileService const> tfs;
  _hnHits = tfs->make<TH1F>("hnHits", "Number of SimEnergyDeposits", 300, 0, 0);
  _hEdep = tfs->make<TH1F>("hEdep", "Energy deposition in SimEnergyDeposits", 100,0.,0.02);
  _hnumPhotons = tfs->make<TH1F>("hnumPhotons", "number of photons per  SimEnergyDeposit", 100,0.,500.);
  _hLandauPhotons= tfs->make<TH1F>("hLandauPhotons", "number of photons/cm", 100,0.,2000000.);
  _hLandauEdep= tfs->make<TH1F>("hLandauEdep", "Edep/cm", 100,0.,10.);
  _hSteplength= tfs->make<TH1F>("hSteplength", "geant 4 step length", 100,0.,0.05);
  _ntuple = tfs->make<TNtuple>("ntuple","Demo ntuple",
                               "Event:Edep:em_Edep:nonem_Edep:xpos:ypos:zpos:time");
} // end beginJob

void larg4::CheckSimEnergyDeposit::analyze(const art::Event& event)
{
  //std::vector<art::Handle<sim::SimEnergyDepositCollection>> allSims;
  //event.getManyByType(allSims);
  auto allSims = event.getMany<sim::SimEnergyDepositCollection>();
  for (auto const& sims : allSims) {
    double sumPhotons=0.0;
    double sumE = 0.0;
    _hnHits->Fill(sims->size());
    //    std::cout<< "CheckSimEnergyDeposit: ";
    for (auto const& hit : *sims) {
      // sum up energy deposit in a 1cm slice of liquid Argon.
      if (std::abs(hit.EndZ())<0.5) {
        sumPhotons= sumPhotons + hit.NumPhotons();
        sumE= sumE +hit.Energy();
      }
      _hnumPhotons->Fill( hit.NumPhotons());
      _hEdep->Fill( hit.Energy());   // energy deposit in MeV
      _hSteplength->Fill( hit.StepLength()); // step length in cm
      //std::cout<<hit.TrackID()<<" ,"<< <<std::endl
      //std::cout<<hit.TrackID()<<",";
      /*
        _ntuple->Fill(event.event(),
        hit.GetEdep(),
        hit.GetEdepEM(),
        hit.GetEdepnonEM(),
        hit.GetXpos(),
        hit.GetYpos(),
        hit.GetZpos(),
        hit.GetTime());
      */
    }
    std::cout<< std::endl;
    _hLandauPhotons->Fill(sumPhotons);
    _hLandauEdep->Fill(sumE);
  }
} // end analyze

DEFINE_ART_MODULE(larg4::CheckSimEnergyDeposit)
