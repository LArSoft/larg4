//
//               __        __ __  __  __
//   ____ ______/ /_____ _/ // / / /_/ /__
//  / __ `/ ___/ __/ __ `/ // /_/ __/ //_/
// / /_/ / /  / /_/ /_/ /__  __/ /_/ ,<
// \__,_/_/   \__/\__, /  /_/  \__/_/|_|
//               /____/
//
// larg4: art based Geant 4 Toolkit
//
//=============================================================================
// CheckMCParticle_module.hh: Analysis module to analyze the GenParticles
// in the Event
// Author: Hans Wenzel (Fermilab)
//=============================================================================
#ifndef CHECKGENPARTICLE_MODULE_HH
#define	CHECKGENPARTICLE_MODULE_HH
//
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
#include "art_root_io/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Run.h"
#include "art_root_io/TFileDirectory.h"
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

using namespace std;
namespace larg4 {
    class CheckMCParticle;
}

class larg4::CheckMCParticle : public art::EDAnalyzer {
public:

 explicit CheckMCParticle(fhicl::ParameterSet const& p);
    virtual void beginJob();
    virtual void beginRun(const art::Run& Run);
    virtual void endJob();
    virtual void analyze(const art::Event& event);

private:
    std::string _myName;
    TH1F* _hnParts;
    TDirectory const * _directory;
    TFile * _file;

};

#endif	/* CHECKGENPARTICLE_MODULE_HH */

