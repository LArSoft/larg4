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
// nobleGasTPCSD.cc: Class representing a sensitive tracking detector
// Author: Hans Wenzel (Fermilab)
//=============================================================================
#include "larg4/SimEnergyDeposit/SimEnergyDepositSD.h"
#include "larg4/SimEnergyDeposit/SimEnergyDepositHit.h"
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
namespace artg4tk {

 artg4tk::SimEnergyDepositSD::SimEnergyDepositSD(G4String name)
: G4VSensitiveDetector(name) {
   hitCollection.clear();
    G4String HCname =  name + "_HC";
    collectionName.insert(HCname);
        G4cout << collectionName.size() << "   SimEnergyDepositSD name:  " << name << " collection Name: " << HCname << G4endl;
    HCID = -1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

artg4tk::SimEnergyDepositSD::~SimEnergyDepositSD() {
//    RootIO::GetInstance()->Close();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void  artg4tk::SimEnergyDepositSD::Initialize(G4HCofThisEvent* HCE) {
   hitCollection.clear();

    if (HCID < 0) {
        G4cout << "artg4tk::SimEnergyDepositSD::Initialize:  " << SensitiveDetectorName << "   " << collectionName[0] << G4endl;
        HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    }
    //    HCE->AddHitsCollection(HCID, trackerCollection);
}
//artg4tk::nobleGasTPCHitCollection* artg4tk::nobleGasTPCSD::getTrackerCollection()
//      {return trackerCollection;}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool  artg4tk::SimEnergyDepositSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
       G4double edep = aStep->GetTotalEnergyDeposit()/CLHEP::MeV;
      
    if (edep == 0.) return false;
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


  SimEnergyDepositHit  newHit =  SimEnergyDepositHit(photons,
						     nrelec,
						     edep,
						     start,
						     end,
						     aStep->GetPreStepPoint()->GetGlobalTime() / CLHEP::ns,
						     aStep->GetPostStepPoint()->GetGlobalTime() /CLHEP::ns,
						     aStep->GetTrack()->GetTrackID(),
						     aStep->GetTrack()->GetParticleDefinition()->GetParticleDefinitionID()  );


    hitCollection.push_back(newHit);
    return true;
}
}
