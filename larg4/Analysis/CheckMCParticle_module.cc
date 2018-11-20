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
// CheckMCParticle_module.cc: Analysis module to analyze the GenParticles 
// in the Event 
// Author: Hans Wenzel (Fermilab)
//=============================================================================
// larg4 includes:
#include "larg4/Analysis/CheckMCParticle_module.h"
// framework includes:
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

//using namespace std;

larg4::CheckMCParticle::CheckMCParticle(fhicl::ParameterSet const& p) :
art::EDAnalyzer(p),
_myName(p.get<std::string>("name", "CheckMCParticle")),
_hnParts(0),
_directory(0),
_file(0) {
}

void larg4::CheckMCParticle::beginRun(const art::Run& thisRun) {
}

void larg4::CheckMCParticle::beginJob() {

    art::ServiceHandle<art::TFileService> tfs;
    _directory = gDirectory;
    _file = gDirectory->GetFile();
    _hnParts = tfs->make<TH1F>("hnParts", "Number of generated Particles", 100, 0., 2000.);
} // end beginJob

void larg4::CheckMCParticle::analyze(const art::Event& event) {
    typedef std::vector< art::Handle<std::vector<simb::MCParticle> > >  HandleVector;
    HandleVector allGens;
    event.getManyByType(allGens);
    for (HandleVector::const_iterator i = allGens.begin(); i != allGens.end(); ++i) {
        const  std::vector<simb::MCParticle> & gens(**i);
        _hnParts->Fill(gens.size());
        for (std::vector<simb::MCParticle>::const_iterator j = gens.begin(); j != gens.end(); ++j) {
	  
            const  simb::MCParticle & genpart = *j;
            cout << "Part id:  " << genpart.TrackId()  << endl;
	    cout << "PDG id:  " << genpart.PdgCode()  << endl;
	    cout << "Status Code:  " << genpart.StatusCode()  << endl;
	    cout << "Mother:  " << genpart.Mother()  << endl;
	    if (genpart.Mother()==0)
	      {
		cout << "momentum:  " <<   genpart.P() << endl;
		cout << "position:  " << genpart.Vx()<< "  "<< genpart.Vy()<<"  "<< genpart.Vz()  << endl;
	      } 
	    // CLHEP::HepLorentzVector const& mom = genpart.PdgCode();
            //cout << "Part Energy:  " << mom.e() << endl;
            //cout << "invariant mass:  " << mom.invariantMass() << endl;
            //cout << "momentum:  " << mom.pz() << endl;
            //cout <<genpart<<endl;
	  
        }
    }

} // end analyze

void larg4::CheckMCParticle::endJob() {
}
using larg4::CheckMCParticle;

DEFINE_ART_MODULE(CheckMCParticle)
