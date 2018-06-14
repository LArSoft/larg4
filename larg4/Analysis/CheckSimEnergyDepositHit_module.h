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
// CheckSimEnergyDepositHit_module.h: Analyzer module that demonstrates access to 
// SimEnergyDepositHit hits 
// and makes some histograms
// 
// Author: Hans Wenzel (Fermilab)
//=============================================================================
#ifndef CHECKHITS_MODULE_HH
#define	CHECKHITS_MODULE_HH
// art Framework includes.
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Services/Optional/TFileDirectory.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Principal/Provenance.h"
// artg4 includes:
#include "artg4tk/services/DetectorHolder_service.hh"
// artg4tk includes:
#include "larg4/SimEnergyDeposit/SimEnergyDepositHit.h"
// Root includes.
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TNtuple.h"
#include "TDirectory.h"
// Other includes.
#include "CLHEP/Units/SystemOfUnits.h"

using namespace std;
namespace artg4tk {
    class CheckSimEnergyDepositHit;
}

class artg4tk::CheckSimEnergyDepositHit : public art::EDAnalyzer {
public:

    explicit CheckSimEnergyDepositHit(fhicl::ParameterSet const& p);
    virtual void beginJob();
    virtual void beginRun(const art::Run& Run);
    virtual void endJob();
    virtual void analyze(const art::Event& event);

private:


  TH1F* _hnHits;        // number of SimEnergyDepositHits
  TH1F* _hEdep;        //  average energy deposition in SimEnergyDepositHits
  TH1F* _hnumPhotons;   //  number of Photons per SimEnergyDepositHits
  TH1F* _hLandauPhotons;//  Edep/cm  SimEnergyDepositHits
  TH1F* _hLandauEdep;   //  number of Photons/cm SimEnergyDepositHits
  TH1F* _hSteplength;   //  Geant 4 step length
  TNtuple* _ntuple;
};

#endif	/* CHECKHITS_MODULE_HH */

