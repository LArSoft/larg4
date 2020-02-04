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
// AuxDetSD.cc: Class representing a sensitive aux detector
// Author: Hans Wenzel (Fermilab)
//=============================================================================
#include<algorithm>
#include<unordered_set>
#include "larg4/Services/AuxDetSD.h"
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
//#define _verbose_ 1
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace larg4 {

  AuxDetSD::AuxDetSD(G4String name)
  : G4VSensitiveDetector(name)
  {
    hitCollection.clear();
  }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

AuxDetSD::~AuxDetSD() {

}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void  AuxDetSD::Initialize(G4HCofThisEvent* ) {
   hitCollection.clear();
   temphitCollection.clear();
}
  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  G4bool  AuxDetSD::ProcessHits(G4Step* step, G4TouchableHistory*) {
  G4double edep = step->GetTotalEnergyDeposit() / CLHEP::MeV;
  if (edep == 0.) return false;
  G4Track * track = step->GetTrack();
  const unsigned int trackID = track->GetTrackID();
  unsigned int ID = step->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();
  TempHit tmpHit = TempHit(
			   ID,
			   trackID,
			   track->GetParentID(),
			   step->IsFirstStepInVolume(),
			   step->IsLastStepInVolume(),
			   edep,
			   step->GetPreStepPoint()->GetPosition().getX() / CLHEP::cm,
			   step->GetPreStepPoint()->GetPosition().getY() / CLHEP::cm,
			   step->GetPreStepPoint()->GetPosition().getZ() / CLHEP::cm,
			   step->GetPreStepPoint()->GetGlobalTime() / CLHEP::ns,
			   step->GetPostStepPoint()->GetPosition().getX() / CLHEP::cm,
			   step->GetPostStepPoint()->GetPosition().getY() / CLHEP::cm,
			   step->GetPostStepPoint()->GetPosition().getZ() / CLHEP::cm,
			   step->GetPostStepPoint()->GetGlobalTime() / CLHEP::ns,
			   step->GetPostStepPoint()->GetMomentum().getX() / CLHEP::GeV,
			   step->GetPostStepPoint()->GetMomentum().getY() / CLHEP::GeV,
			   step->GetPostStepPoint()->GetMomentum().getZ() / CLHEP::GeV
			   );
  temphitCollection.push_back(tmpHit);

  /*
  sim::AuxDetHit newHit = sim::AuxDetHit(ID,
				    trackID,
				    edep,
				    step->GetPreStepPoint()->GetPosition().getX()/CLHEP::cm,
				    step->GetPreStepPoint()->GetPosition().getY()/CLHEP::cm,
				    step->GetPreStepPoint()->GetPosition().getZ()/CLHEP::cm,
				    step->GetPreStepPoint()->GetGlobalTime()/CLHEP::ns,
				    step->GetPostStepPoint()->GetPosition().getX()/CLHEP::cm,
				    step->GetPostStepPoint()->GetPosition().getY()/CLHEP::cm,
				    step->GetPostStepPoint()->GetPosition().getZ()/CLHEP::cm,
				    step->GetPostStepPoint()->GetGlobalTime()/CLHEP::ns,
				    step->GetPostStepPoint()->GetMomentum().getX()/CLHEP::GeV,
				    step->GetPostStepPoint()->GetMomentum().getY()/CLHEP::GeV,
				    step->GetPostStepPoint()->GetMomentum().getZ()/CLHEP::GeV
				    );
  hitCollection.push_back(newHit);
  */
  return true;
}


void AuxDetSD::EndOfEvent(G4HCofThisEvent*) {
    if (temphitCollection.size() == 0) return; // No hits so nothing to do
#if defined _verbose_
    std::cout << " EndOfEvent number of temp hits: " << temphitCollection.size() << std::endl;
    std::cout << " EndOfEvent number of aux hits: " << hitCollection.size() << std::endl;
#endif
    std::sort(temphitCollection.begin(), temphitCollection.end());
    int geoId = -1;
    int trackId = -1;
    unsigned int counter = 0;
    std::unordered_set<unsigned int> setofIDs;

    for (auto it = temphitCollection.begin(); it != temphitCollection.end(); it++) {
#if defined _verbose_
        std::cout << "geoID: " << it->GetID() << " track ID: " << it->GetTrackID() << " Edep: " << it->GetEnergyDeposited()
                << " Parent Id: " << it->GetParentID() << " exit Time:  " << it->GetExitT()
                << " is first: " << it->IsIsfirstinVolume() << " is last:  " << it->IsIslastinVolume();
#endif
        if (it->GetID() == geoId && trackId == it->GetTrackID()) // trackid and detector didn't change
        {
#if defined _verbose_
            std::cout << " A" << std::endl;
#endif
            if (it->GetExitT()) // change exit vector and add to total charge
            {
                hitCollection[counter - 1].SetEnergyDeposited(hitCollection[counter - 1].GetEnergyDeposited() + it->GetEnergyDeposited());
                hitCollection[counter - 1].SetExitX(it->GetExitX());
                hitCollection[counter - 1].SetExitY(it->GetExitY());
                hitCollection[counter - 1].SetExitZ(it->GetExitZ());
                hitCollection[counter - 1].SetExitT(it->GetExitT());
            } else // just add to total charge
            {
                hitCollection[counter - 1].SetEnergyDeposited(hitCollection[counter - 1].GetEnergyDeposited() + it->GetEnergyDeposited());
	    }
        } else if (setofIDs.find(it->GetParentID()) != setofIDs.end()) {
            setofIDs.insert(it->GetTrackID());
#if defined _verbose_
            std::cout << " A" << std::endl;
#endif
            hitCollection[counter - 1].SetEnergyDeposited(hitCollection[counter - 1].GetEnergyDeposited() + it->GetEnergyDeposited());
        } else if (it->GetID() != geoId) // new Detector
        {
            geoId = it->GetID();
            trackId = it->GetTrackID();
            setofIDs.clear();
            setofIDs.insert(it->GetTrackID());
#if defined _verbose_
            std::cout << " N" << std::endl;
#endif
            counter++;
            hitCollection.push_back(sim::AuxDetHit(it->GetID(),
                    it->GetTrackID(),
                    it->GetEnergyDeposited(),
                    it->GetEntryX(),
                    it->GetEntryY(),
                    it->GetEntryZ(),
                    it->GetEntryT(),
                    it->GetExitX(),
                    it->GetExitY(),
                    it->GetExitZ(),
                    it->GetExitT(),
                    it->GetExitMomentumX(),
                    it->GetExitMomentumY(),
                    it->GetExitMomentumZ()
						       ));
        } else {
            trackId = it->GetTrackID();
#if defined _verbose_
            std::cout << " N" << std::endl;
#endif
            counter++;
            setofIDs.clear();
            setofIDs.insert(it->GetTrackID());
            hitCollection.push_back(sim::AuxDetHit(it->GetID(),
                    it->GetTrackID(),
                    it->GetEnergyDeposited(),
                    it->GetEntryX(),
                    it->GetEntryY(),
                    it->GetEntryZ(),
                    it->GetEntryT(),
                    it->GetExitX(),
                    it->GetExitY(),
                    it->GetExitZ(),
                    it->GetExitT(),
                    it->GetExitMomentumX(),
                    it->GetExitMomentumY(),
                    it->GetExitMomentumZ()
                    ));
        }
    }
#if defined _verbose_
    std::cout << "Number of AuxDetHits: " << counter << std::endl;
#endif
	}  // EndOfEvent
} // namespace sim

