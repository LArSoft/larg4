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
#include "larg4/Analysis/CheckSimEnergyDepositHit_module.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include <cmath>   
artg4tk::CheckSimEnergyDepositHit::CheckSimEnergyDepositHit(fhicl::ParameterSet const& p) :
art::EDAnalyzer(p),
_hnHits(0),
_hEdep(0),
_hnumPhotons(0),
_hLandauPhotons(0),
_hLandauEdep(0),
_hSteplength(0),
_ntuple(0) {
}

void artg4tk::CheckSimEnergyDepositHit::beginRun(const art::Run& thisRun) {

}

void artg4tk::CheckSimEnergyDepositHit::beginJob() {
    art::ServiceHandle<art::TFileService> tfs;
    _hnHits = tfs->make<TH1F>("hnHits", "Number of SimEnergyDepositHits", 300, 0, 0);
    _hEdep = tfs->make<TH1F>("hEdep", "Energy deposition in SimEnergyDepositHits", 100,0.,0.02);
    _hnumPhotons = tfs->make<TH1F>("hnumPhotons", "number of photons per  SimEnergyDepositHit", 100,0.,500.);
    _hLandauPhotons= tfs->make<TH1F>("hLandauPhotons", "number of photons/cm", 100,0.,2000000.);
    _hLandauEdep= tfs->make<TH1F>("hLandauEdep", "Edep/0.5cm", 100,0.,40.);
    _hSteplength= tfs->make<TH1F>("hSteplength", "geant 4 step length", 100,0.,0.05);
    _ntuple = tfs->make<TNtuple>("ntuple","Demo ntuple",
			  "Event:Edep:em_Edep:nonem_Edep:xpos:ypos:zpos:time");

} // end beginJob

void artg4tk::CheckSimEnergyDepositHit::analyze(const art::Event& event) {
    typedef std::vector< art::Handle<SimEnergyDepositHitCollection> > HandleVector;
    HandleVector allSims;
    event.getManyByType(allSims);
    double sumE = 0.0;
    double sumPhotons=0.0;
    for (HandleVector::const_iterator i = allSims.begin(); i != allSims.end(); ++i) {
        const SimEnergyDepositHitCollection & sims(**i);
       cout << " SimEnergyDepositHit collection size:  " << sims.size() << endl;
       sumPhotons=0.0;
       sumE = 0.0;
       _hnHits->Fill(sims.size());
       	for (SimEnergyDepositHitCollection::const_iterator j = sims.begin(); j != sims.end(); ++j) {
	  const SimEnergyDepositHit& hit = *j;
	  _hEdep->Fill(hit.Energy());
	  // sum up energy deposit in a 5mm slice of liquid Argon. 
	  if (std::abs(hit.EndZ()/CLHEP::cm)<0.25) {
	      sumPhotons= sumPhotons + hit.NumPhotons();
	      sumE= sumE +hit.Energy();
	    }
	  _hnumPhotons->Fill( hit.NumPhotons());
	  _hEdep->Fill( hit.Energy());
	  std::cout<< "length:   "<<hit.StepLength()<< "   "<<hit.StepLength()/CLHEP::cm <<std::endl;
	  _hSteplength->Fill( hit.StepLength());      
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
	_hLandauEdep->Fill(sumE / CLHEP::MeV);
	
    }
} // end analyze

void artg4tk::CheckSimEnergyDepositHit::endJob() {

}// end endJob

using artg4tk::CheckSimEnergyDepositHit;

DEFINE_ART_MODULE(CheckSimEnergyDepositHit)
