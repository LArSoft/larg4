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
// nobleGasTPCSD.hh: Class representing a sensitive tracking detector
// Author: Hans Wenzel (Fermilab)
//=============================================================================
#ifndef SimEnergyDepositSD_h
#define SimEnergyDepositSD_h 1

#include "Geant4/G4VSensitiveDetector.hh"
#include "lardataobj/Simulation/SimEnergyDeposit.h"

class G4Step;
class G4HCofThisEvent;
//class SimEnergyDepositCollection;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace larg4 {

    class SimEnergyDepositSD : public G4VSensitiveDetector {
    public:
        SimEnergyDepositSD(G4String);
        ~SimEnergyDepositSD();
        void Initialize(G4HCofThisEvent*);
        G4bool ProcessHits(G4Step*, G4TouchableHistory*);
	const sim::SimEnergyDepositCollection& GetHits() const { return hitCollection; }
    private:
      sim::SimEnergyDepositCollection hitCollection;
      G4int HCID;

    };

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
}
#endif

