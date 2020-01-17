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
// SimEnergyDeposit.cc: Class representing a sensitive tracking detector
// Author: Hans Wenzel (Fermilab)
//=============================================================================
#include "larg4/Services/SimEnergyDepositSD.h"
#include "Geant4/G4HCofThisEvent.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4ios.hh"
#include "Geant4/G4VVisManager.hh"
#include "Geant4/G4Event.hh"
#include "Geant4/G4EventManager.hh"
#include "Geant4/G4VSolid.hh"
#include "Geant4/G4Cerenkov.hh"
#include "Geant4/G4Scintillation.hh"
#include "Geant4/G4SteppingManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace larg4 {

  SimEnergyDepositSD::SimEnergyDepositSD(G4String name)
: G4VSensitiveDetector(name) {
   hitCollection.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

 SimEnergyDepositSD::~SimEnergyDepositSD() {

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void   SimEnergyDepositSD::Initialize(G4HCofThisEvent* HCE) {
    hitCollection.clear();
  }
  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  G4bool   SimEnergyDepositSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
       G4double edep = aStep->GetTotalEnergyDeposit()/CLHEP::MeV;

       if (edep == 0.) return false;
       //std::cout << "7777777777777777:   "<< aStep->GetTotalEnergyDeposit()/CLHEP::MeV << "   " << aStep->GetTotalEnergyDeposit() <<std::endl;
       const int electronsperMeV= 10000;
       int nrelec=(int)round(edep*electronsperMeV);
       if (aStep->GetTrack()->GetDynamicParticle()->GetCharge() == 0) return false;
       G4int photons = 0;
       G4SteppingManager* fpSteppingManager = G4EventManager::GetEventManager()
         ->GetTrackingManager()->GetSteppingManager();
       G4StepStatus stepStatus = fpSteppingManager->GetfStepStatus();
       if (stepStatus != fAtRestDoItProc) {
         G4ProcessVector* procPost = fpSteppingManager->GetfPostStepDoItVector();
         size_t MAXofPostStepLoops = fpSteppingManager->GetMAXofPostStepLoops();
         for (size_t i3 = 0; i3 < MAXofPostStepLoops; i3++) {
           /*
             if ((*procPost)[i3]->GetProcessName() == "Cerenkov") {
             G4Cerenkov* proc =(G4Cerenkov*) (*procPost)[i3];
             photons+=proc->GetNumPhotons();
             }
           */
           if ((*procPost)[i3]->GetProcessName() == "Scintillation") {
             G4Scintillation* proc1 = (G4Scintillation*) (*procPost)[i3];
             photons += proc1->GetNumPhotons();
           }
         }
       }
       geo::Point_t start = geo::Point_t(
                                         aStep->GetPreStepPoint()->GetPosition().x()/CLHEP::cm,
                                         aStep->GetPreStepPoint()->GetPosition().y()/CLHEP::cm,
                                         aStep->GetPreStepPoint()->GetPosition().z()/CLHEP::cm);
       geo::Point_t end = geo::Point_t(
                                       aStep->GetPostStepPoint()->GetPosition().x()/CLHEP::cm,
                                       aStep->GetPostStepPoint()->GetPosition().y()/CLHEP::cm,
                                       aStep->GetPostStepPoint()->GetPosition().z()/CLHEP::cm);
       sim::SimEnergyDeposit  newHit =  sim::SimEnergyDeposit(photons,
                                                              nrelec,
                                                              1.0,
                                                              edep,
                                                              start,
                                                              end,
                                                              aStep->GetPreStepPoint()->GetGlobalTime() / CLHEP::ns,
                                                              aStep->GetPostStepPoint()->GetGlobalTime() /CLHEP::ns,
                                                              aStep->GetTrack()->GetTrackID(),
                                                              aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding()  );
       hitCollection.push_back(newHit);
    return true;
  }// end ProcessHits
} // end namespace  larg4
