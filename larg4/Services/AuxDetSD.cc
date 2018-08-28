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
// AuxDetSD.cc: Class representing a sensitive tracking detector
// Author: Hans Wenzel (Fermilab)
//=============================================================================
#include "larg4/Services/AuxDetSD.h"
#include "larg4/pluginActions/ParticleListAction_service.h"
#include "Geant4/G4HCofThisEvent.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4ios.hh"
#include "Geant4/G4VVisManager.hh"
#include "Geant4/G4Event.hh"
#include "Geant4/G4EventManager.hh"
#include "Geant4/G4VSolid.hh"
#include "Geant4/G4UnitsTable.hh"
#include "Geant4/G4SystemOfUnits.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace larg4 {

  AuxDetSD::AuxDetSD(std::string const&  name,
		     unsigned int       adNum,
		     unsigned int       svNum)

  : G4VSensitiveDetector(name),
        fAuxDet(adNum),
      fAuxDetSensitive(svNum)
  {
    //    auxDetCollection.clear();
    G4String HCname =  name + "_HC";
    collectionName.insert(HCname);
    G4cout << collectionName.size() << "   AuxDetSD name:  " << name << " collection Name: " << HCname << G4endl;
 
    //    HCID = -1;
  }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

AuxDetSD::~AuxDetSD() {

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void  AuxDetSD::Initialize(G4HCofThisEvent* ) {

}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void AuxDetSD::clear()
  {
    fAuxDetIDEs.clear();
  }


G4bool  AuxDetSD::ProcessHits(G4Step* step, G4TouchableHistory*) {

		// collect the info for this step
  art::ServiceHandle<ParticleListActionService> pla;
    const int trackID = pla->GetCurrentTrackID();
    
    G4double energyDeposited = step->GetTotalEnergyDeposit()/CLHEP::GeV;
    
    G4ThreeVector startG4(step->GetPreStepPoint()->GetPosition() );
    double startWorld[3] = {startG4.getX()/CLHEP::cm,
                            startG4.getY()/CLHEP::cm,
                            startG4.getZ()/CLHEP::cm};
    
    double startTime = step->GetPreStepPoint()->GetGlobalTime()/CLHEP::ns;
    
    G4ThreeVector stopG4( step->GetPostStepPoint()->GetPosition());
    double stopWorld[3] = {stopG4.getX()/CLHEP::cm,
                           stopG4.getY()/CLHEP::cm,
                           stopG4.getZ()/CLHEP::cm};
    
    G4ThreeVector stopG4Momentum( step->GetPostStepPoint()->GetMomentum());
    double stopWorldMomVector[3] = {stopG4Momentum.getX()/CLHEP::GeV,
                                    stopG4Momentum.getY()/CLHEP::GeV,
                                    stopG4Momentum.getZ()/CLHEP::GeV};
    
    double stopTime = step->GetPostStepPoint()->GetGlobalTime()/CLHEP::ns;
    
    this->AddParticleStep( trackID,
                           energyDeposited,
                           startWorld[0],
                           startWorld[1],
                           startWorld[2],
                           startTime,
                           stopWorld[0],
                           stopWorld[1],
                           stopWorld[2],
                           stopTime,
                           stopWorldMomVector[0],
                           stopWorldMomVector[1],
                           stopWorldMomVector[2]
                           );
    
    return true;
}

 void AuxDetSD::AddParticleStep(
  			int	inputTrackID,
			float	inputEnergyDeposited,
			float	inputEntryX,
			float	inputEntryY,
			float	inputEntryZ,
			float	inputEntryT,
			float	inputExitX,
			float	inputExitY,
			float	inputExitZ,
			float	inputExitT,
			float	inputExitMomentumX,
			float	inputExitMomentumY,
			float	inputExitMomentumZ){

    sim::AuxDetIDE auxDetIDE;
    auxDetIDE.trackID		= inputTrackID;
    auxDetIDE.energyDeposited	= inputEnergyDeposited;
    auxDetIDE.entryX		= inputEntryX;
    auxDetIDE.entryY		= inputEntryY;
    auxDetIDE.entryZ		= inputEntryZ;
    auxDetIDE.entryT		= inputEntryT;
    auxDetIDE.exitX		= inputExitX;
    auxDetIDE.exitY		= inputExitY;
    auxDetIDE.exitZ		= inputExitZ;
    auxDetIDE.exitT		= inputExitT;
    auxDetIDE.exitMomentumX	= inputExitMomentumX;
    auxDetIDE.exitMomentumY	= inputExitMomentumY;
    auxDetIDE.exitMomentumZ	= inputExitMomentumZ;

    std::vector<sim::AuxDetIDE>::iterator IDEitr
      = std::find(fAuxDetIDEs.begin(), fAuxDetIDEs.end(), auxDetIDE);

    if(IDEitr != fAuxDetIDEs.end()){ //If trackID is already in the map, update it

      IDEitr->energyDeposited += inputEnergyDeposited;
      IDEitr->exitX            = inputExitX;
      IDEitr->exitY            = inputExitY;
      IDEitr->exitZ            = inputExitZ;
      IDEitr->exitT            = inputExitT;
      IDEitr->exitMomentumX    = inputExitMomentumX;
      IDEitr->exitMomentumY    = inputExitMomentumY;
      IDEitr->exitMomentumZ    = inputExitMomentumZ;
    }
    else{  //if trackID is not in the set yet, add it
      fAuxDetIDEs.push_back(std::move(auxDetIDE));
    }//else
  }//AddParticleStep
  


    /*





  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep == 0.) return false;
  if (aStep->GetTrack()->GetDynamicParticle()->GetCharge() == 0) return false;
  AuxDetHit  newHit =  AuxDetHit(
				   edep,
				   aStep->GetPostStepPoint()->GetPosition().x(),
				   aStep->GetPostStepPoint()->GetPosition().y(),
				   aStep->GetPostStepPoint()->GetPosition().z(),
				   aStep->GetPostStepPoint()->GetGlobalTime() / ns
				   );
    auxDetCollection.push_back(newHit);
    return true;
    */

} // namespace larg4

