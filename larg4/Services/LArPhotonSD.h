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
// LArPhotonSD.cc: Class representing a sensitive detector registering optical
// photons
// Author: Hans Wenzel (Fermilab)
//=============================================================================
#ifndef LArPhotonSD_h
#define LArPhotonSD_h 1

#include "Geant4/G4VSensitiveDetector.hh"
//#include "artg4tk/pluginDetectors/gdml/PhotonHit.hh"
#include "lardataobj/Simulation/SimPhotons.h"
class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace larg4 {
  class OpDetPhotonTable;
  class LArPhotonSD : public G4VSensitiveDetector {
  public:
    LArPhotonSD(G4String);
    ~LArPhotonSD();
    
    void Initialize(G4HCofThisEvent*);
    G4bool ProcessHits(G4Step*, G4TouchableHistory*);
    //    const PhotonHitCollection& GetHits() const { return hitCollection;}
    
  private:
    OpDetPhotonTable         * fThePhotonTable;
    // PhotonHitCollection hitCollection;
  };
} //end namespace larg4
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

