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
// CheckAuxDetHitHit_module.cc: Analyzer module that demonstrates access to 
// Calorimeter hits 
// and makes some histograms
// 
// Author: Hans Wenzel (Fermilab)
//=============================================================================
#include "larg4/Analysis/CheckAuxDetHit_module.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include <cmath>   
larg4::CheckAuxDetHit::CheckAuxDetHit(fhicl::ParameterSet const& p) :
art::EDAnalyzer(p),
_hnHits(0),
_hEdep(0),
_hID(0)
 {
}

void larg4::CheckAuxDetHit::beginRun(const art::Run& thisRun) {

}

void larg4::CheckAuxDetHit::beginJob() {
    art::ServiceHandle<art::TFileService const> tfs;
    _hnHits = tfs->make<TH1F>("hnHits", "Number of AuxDetHits", 30, 0,30 );
    _hEdep = tfs->make<TH1F>("hEdep", "Energy deposition in AuxDetHits", 100,0.,4.);
    _hID = tfs->make<TH1F>("hID", "Id of hit AuxDet", 100,0.,5.);
    //    _ntuple = tfs->make<TNtuple>("ntuple","Demo ntuple",
    //			  "Event:Edep:em_Edep:nonem_Edep:xpos:ypos:zpos:time");

} // end beginJob

void larg4::CheckAuxDetHit::analyze(const art::Event& event) {
  typedef std::vector< art::Handle<sim::AuxDetHitCollection> > HandleVector;
    HandleVector allSims;
    event.getManyByType(allSims);
    for (unsigned int ii=0;ii<4;ii++) Edeps[ii]=0.0;
    for (HandleVector::const_iterator i = allSims.begin(); i != allSims.end(); ++i) {
      const sim::AuxDetHitCollection & sims(**i);
       _hnHits->Fill(sims.size());
       for (sim::AuxDetHitCollection::const_iterator j = sims.begin(); j != sims.end(); ++j) {
	 const sim::AuxDetHit& hit = *j;
	  _hID->Fill(hit.GetID());
	  Edeps[hit.GetID()-1]= Edeps[hit.GetID()-1]+hit.GetEnergyDeposited();
        }
    }
   for (unsigned int ii=0;ii<4;ii++)  _hEdep->Fill(Edeps[ii]);
} // end analyze

void larg4::CheckAuxDetHit::endJob() {

}// end endJob

using larg4::CheckAuxDetHit;

DEFINE_ART_MODULE(CheckAuxDetHit)
