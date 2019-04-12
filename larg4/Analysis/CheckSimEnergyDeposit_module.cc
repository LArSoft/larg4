//
//               __        __ __  __  __
//   ____ ______/ /_____ _/ // / / /_/ /__
//  / __ `/ ___/ __/ __ `/ // /_/ __/ //_/
// / /_/ / /  / /_/ /_/ /__  __/ /_/ ,<
// \__,_/_/   \__/\__, /  /_/  \__/_/|_|
//               /____/
//
// artg4tk: art based Geant 4 Toolkit
//
//=============================================================================
// CheckSimEnergyDepositHit_module.cc: Analyzer module that demonstrates access to
// Calorimeter hits
// and makes some histograms
//
// Author: Hans Wenzel (Fermilab)
//=============================================================================
#include "larg4/Analysis/CheckSimEnergyDeposit_module.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include <cmath>
larg4::CheckSimEnergyDeposit::CheckSimEnergyDeposit(fhicl::ParameterSet const& p) :
art::EDAnalyzer(p),
_hnHits(0),
_hEdep(0),
_hnumPhotons(0),
_hLandauPhotons(0),
_hLandauEdep(0),
_hSteplength(0),
_ntuple(0) {
}

void larg4::CheckSimEnergyDeposit::beginJob() {
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

void larg4::CheckSimEnergyDeposit::analyze(const art::Event& event) {
  typedef std::vector< art::Handle<sim::SimEnergyDepositCollection> > HandleVector;
    HandleVector allSims;
    event.getManyByType(allSims);
    double sumE = 0.0;
    double sumPhotons=0.0;
    for (HandleVector::const_iterator i = allSims.begin(); i != allSims.end(); ++i) {
      const sim::SimEnergyDepositCollection & sims(**i);
      sumPhotons=0.0;
      sumE = 0.0;
      _hnHits->Fill(sims.size());
      for (sim::SimEnergyDepositCollection::const_iterator j = sims.begin(); j != sims.end(); ++j) {
	const sim::SimEnergyDeposit& hit = *j;
	// sum up energy deposit in a 1cm slice of liquid Argon.
	if (std::abs(hit.EndZ())<0.5) {
	  sumPhotons= sumPhotons + hit.NumPhotons();
	  sumE= sumE +hit.Energy();
	}
	_hnumPhotons->Fill( hit.NumPhotons());
	_hEdep->Fill( hit.Energy());   // energy deposit in MeV
	_hSteplength->Fill( hit.StepLength()); // step length in cm
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
      _hLandauPhotons->Fill(sumPhotons);
      _hLandauEdep->Fill(sumE);
    }
} // end analyze

void larg4::CheckSimEnergyDeposit::endJob() {

}// end endJob

using larg4::CheckSimEnergyDeposit;

DEFINE_ART_MODULE(CheckSimEnergyDeposit)
