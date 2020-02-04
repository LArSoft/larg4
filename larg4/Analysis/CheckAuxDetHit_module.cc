// STL includes
#include <cmath>

// art Framework includes.
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art_root_io/TFileDirectory.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Principal/Provenance.h"
#include "artg4tk/services/DetectorHolder_service.hh"
#include "lardataobj/Simulation/AuxDetHit.h"

// Root includes.
#include "TFile.h"
#include "TH1F.h"
#include "TNtuple.h"
#include "TDirectory.h"

// Other includes.
#include "CLHEP/Units/SystemOfUnits.h"

namespace larg4 {
    class CheckAuxDetHit;
}

class larg4::CheckAuxDetHit : public art::EDAnalyzer {
public:
  explicit CheckAuxDetHit(fhicl::ParameterSet const& p);

private:
  void beginJob() override;
  void analyze(const art::Event& event) override;

  TH1F* _hnHits{nullptr}; // number of AuxDetHitHits
  TH1F* _hEdep{nullptr};  // average energy deposition in AuxDetHitHits
  TH1F* _hID{nullptr};    // AuxDet ID's
  TH1F* _hexit{nullptr};    // exit points in z
  TH1F* _hentry{nullptr};    // entry points in z
  // TNtuple* _ntuple;
};

larg4::CheckAuxDetHit::CheckAuxDetHit(fhicl::ParameterSet const& p) :
  art::EDAnalyzer(p)
{}

void larg4::CheckAuxDetHit::beginJob()
{
  art::ServiceHandle<art::TFileService const> tfs;
  _hnHits = tfs->make<TH1F>("hnHits", "Number of AuxDetHits", 30, 0,30 );
  _hEdep = tfs->make<TH1F>("hEdep", "Energy deposition in AuxDetHits", 100,0.,4.);
  _hID = tfs->make<TH1F>("hID", "Id of hit AuxDet", 100,0.,5.);
  _hexit = tfs->make<TH1F>("hexit", "exit points in z", 100,-100.,100.);
  _hentry = tfs->make<TH1F>("hentry", "entry points in z", 100,-100.,100.);
  // _ntuple = tfs->make<TNtuple>("ntuple","Demo ntuple",
  //                              "Event:Edep:em_Edep:nonem_Edep:xpos:ypos:zpos:time");
} // end beginJob

void larg4::CheckAuxDetHit::analyze(const art::Event& event)
{
  std::vector<art::Handle<sim::AuxDetHitCollection>> allSims;
  event.getManyByType(allSims);
  for (auto const& sims : allSims) {
    _hnHits->Fill(sims->size());
    for (auto const& hit : *sims) {
      _hEdep->Fill(hit.GetEnergyDeposited());
      _hexit->Fill(hit.GetExitZ());
      _hentry->Fill(hit.GetEntryZ());
      _hID->Fill(hit.GetID());
    }
  }
} // end analyze

DEFINE_ART_MODULE(larg4::CheckAuxDetHit)
