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
#include "larg4/Services/LArPhotonSD.h"
#include "larsim/LArG4/OpDetPhotonTable.h"
#include "larsim/LArG4/OpDetLookup.h"
#include "lardataobj/Simulation/SimPhotons.h"
#include "Geant4/G4VProcess.hh"
#include "Geant4/G4OpticalPhoton.hh"
#include "Geant4/G4HCofThisEvent.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4ios.hh"
#include "Geant4/G4VVisManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace larg4 {
  LArPhotonSD::LArPhotonSD(G4String name)
    : G4VSensitiveDetector(name) {
    
    // Get instances of singleton classes
    //fTheOpDetLookup        = OpDetLookup::Instance();
    fThePhotonTable        = OpDetPhotonTable::Instance();
    
  }
  
  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  
  void LArPhotonSD::Initialize(G4HCofThisEvent* HCE) {
    // hjw hitCollection.clear();
  }
  
  
  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  
  LArPhotonSD::~LArPhotonSD() {
  }
  
  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  /* 
  G4bool LArPhotonSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
    int ID = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();
    G4Track* theTrack = aStep->GetTrack();
    
    // we only deal with optical Photons:
    if (theTrack->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {
      return false;
    }
    G4double theEdep = theTrack->GetTotalEnergy();
    const G4VProcess * thisProcess = theTrack->GetCreatorProcess();
    
    G4String processname;
    if (thisProcess != NULL)
      processname = thisProcess->GetProcessName();
    else
      processname = "No Process";
    G4int theCreationProcessid;
    if (processname == "Cerenkov") {
      theCreationProcessid = 0;
    } else if (processname == "Scintillation") {
      theCreationProcessid = 1;
    } else {
      theCreationProcessid = -1;
    }
    PhotonHit newHit = PhotonHit(ID,
				 theCreationProcessid,
				 theEdep,
				 aStep->GetPostStepPoint()->GetPosition().x(),
				 aStep->GetPostStepPoint()->GetPosition().y(),
				 aStep->GetPostStepPoint()->GetPosition().z(),
				 theTrack->GetGlobalTime()
				 );
    hitCollection.push_back(newHit);
    theTrack->SetTrackStatus(fStopAndKill);
    return true;
  }// end ProcessHits
*/
 //--------------------------------------------------------

  G4bool LArPhotonSD::ProcessHits(G4Step * aStep, G4TouchableHistory *)
  {
  G4Track* theTrack = aStep->GetTrack();
    
    // we only deal with optical Photons:
    if (theTrack->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {
      return false;
    }

    sim::OnePhoton ThePhoton;
    
    
    // Get photon data to store in the hit

    ThePhoton.SetInSD      = true;

    ThePhoton.InitialPosition     = TVector3(
					      theTrack->GetVertexPosition().x(),
					      theTrack->GetVertexPosition().y(),
					      theTrack->GetVertexPosition().z()	
					      );
    ThePhoton.Time                = theTrack->GetGlobalTime();
    ThePhoton.Energy              = theTrack->GetVertexKineticEnergy();

    // Lookup which OpDet we are in
    G4StepPoint *preStepPoint = aStep->GetPreStepPoint();

    //    int OpDet = fTheOpDetLookup->GetOpDet(preStepPoint->GetPhysicalVolume());
    unsigned int OpDet = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();

    // Store relative position on the photon detector
    G4ThreeVector worldPosition  = preStepPoint->GetPosition();
    G4ThreeVector localPosition  = preStepPoint->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(worldPosition);
    ThePhoton.FinalLocalPosition = TVector3(localPosition.x()/CLHEP::cm, localPosition.y()/CLHEP::cm, localPosition.z()/CLHEP::cm);
    // Add this photon to the detected photons table
    fThePhotonTable->AddPhoton(OpDet, std::move(ThePhoton));   
    // Kill this photon track
    theTrack->SetTrackStatus(fStopAndKill);
    return true;
    

  }// end ProcessHits
} // end namespace  larg4

