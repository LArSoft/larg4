#ifndef LARG4_SERVICES_SIMENERGYDEPOSITSD_H
#define LARG4_SERVICES_SIMENERGYDEPOSITSD_H
// vim: set sw=2 expandtab :
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
// SimEnergyDepositSD: Class representing a liquid Ar TPC
// Author: Hans Wenzel (Fermilab)
//=============================================================================

#pragma once
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
    };

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
}

