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
// CheckAuxDetHit_module.h: Analyzer module that demonstrates access to 
// AuxDetHitHit hits 
// and makes some histograms
// 
// Author: Hans Wenzel (Fermilab)
//=============================================================================
#ifndef CHECKSIMENERGYDEPOSIT_MODULE_HH
#define	CHECKSIMENERGYDEPOSIT_MODULE_HH
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
#include "artg4tk/services/DetectorHolder_service.hh"
#include "lardataobj/Simulation/AuxDetHit.h"
// Root includes.
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TNtuple.h"
#include "TDirectory.h"
// Other includes.
#include "CLHEP/Units/SystemOfUnits.h"


//using namespace std;
namespace larg4 {
    class CheckAuxDetHit;
}

class larg4::CheckAuxDetHit : public art::EDAnalyzer {
public:

    explicit CheckAuxDetHit(fhicl::ParameterSet const& p);
    virtual void beginJob();
    virtual void beginRun(const art::Run& Run);
    virtual void endJob();
    virtual void analyze(const art::Event& event);

private:

  double Edeps[4]; 
  TH1F* _hnHits;        // number of AuxDetHitHits
  TH1F* _hEdep;         // average energy deposition in AuxDetHitHits
  TH1F* _hID;           // AuxDet ID's
  //   TNtuple* _ntuple;
};

#endif	/* CHECKSIMENERGYDEPOSIT_MODULE_HH */

