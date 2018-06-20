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
// LArG4Detector_service.h: 
// LArG4DetectorService is the service that constructs the Geant 4 Geometry 
// as specified in a gdml file.
// To use this service, all you need to do is put it in the services section
// of the fcl configuration file, like this (Just change the name of the gdml file):
// 
// <pre>
// services: { 
//   ...
//     ...  
// LArG4Detector : 
//    {
//    category: "world"
//    gdmlFileName_ : "ta_target.gdml"
//    }  
//   }
// </pre>
// Author: Hans Wenzel (Fermilab)
//=============================================================================
// framework includes:
#include "art/Framework/Services/Registry/ServiceMacros.h"
// larg4 includes: 
#include "larg4/Services/LArG4Detector_service.h"
// artg4tk includes:
#include "artg4tk/pluginDetectors/gdml/ColorReader.hh"
#include "artg4tk/pluginDetectors/gdml/CalorimeterSD.hh"
#include "artg4tk/pluginDetectors/gdml/CalorimeterHit.hh"
#include "artg4tk/pluginDetectors/gdml/DRCalorimeterSD.hh"
#include "artg4tk/pluginDetectors/gdml/DRCalorimeterHit.hh"
#include "artg4tk/pluginDetectors/gdml/myParticleEContribArtData.hh"
#include "artg4tk/pluginDetectors/gdml/PhotonSD.hh"
#include "artg4tk/pluginDetectors/gdml/PhotonHit.hh"
#include "artg4tk/pluginDetectors/gdml/TrackerSD.hh"
#include "artg4tk/pluginDetectors/gdml/TrackerHit.hh"
#include "larg4/Services/SimEnergyDepositSD.h"
#include "lardataobj/Simulation/SimEnergyDeposit.h"
//
#include "artg4tk/pluginDetectors/gdml/InteractionSD.hh"
#include "artg4tk/pluginDetectors/gdml/myInteractionArtHitData.hh"
//
// NOTE; No need to include the products because they'll come via HadInteractionSD.hh
//       but just in case, they're artg4tk/DataProducts/G4DetectorHits/ArtG4tk*
//
#include "artg4tk/pluginDetectors/gdml/HadInteractionSD.hh"
#include "artg4tk/pluginDetectors/gdml/HadIntAndEdepTrkSD.hh"
//
// Geant 4 includes:
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4VUserDetectorConstruction.hh"
#include "Geant4/G4GDMLParser.hh"
#include "Geant4/globals.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4LogicalVolumeStore.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4PhysicalVolumeStore.hh"
#include "Geant4/G4UserLimits.hh"
#include "Geant4/G4UnitsTable.hh"
#include "Geant4/G4StepLimiter.hh"
// C++ includes
#include <vector>
#include <map>

using std::string;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

larg4::LArG4DetectorService::LArG4DetectorService(fhicl::ParameterSet const & p, art::ActivityRegistry &)
  : artg4tk::DetectorBase(p,
    p.get<string>("name", "LArG4DetectorService"),
    p.get<string>("category", "World"),
    p.get<string>("mother_category", "")),
    // Initialize our message logger
    logInfo_("LArG4DetectorService") 
{

   std::string fntmp = p.get<std::string>("gdmlFileName_");
      
   if ( fntmp.substr(0,1) == "$" )
   {
      // path to GDML geom file is given by by env.var. !
      //   
      std::vector<std::string> selm = split( fntmp, '/' ); 
      assert ( !selm.empty() );
      std::string envvar = selm[0].substr(1);
      std::string path = std::string( getenv( envvar.c_str() ) );
      gdmlFileName_ = path;
      for ( size_t i=1; i<selm.size(); ++i )
      {
         gdmlFileName_ += ( "/" + selm[i] );
      }   
   } 
   else
   {
      // absolute path to GDML geom file
      //
      gdmlFileName_ = fntmp;
   }

}

// Destructor

larg4::LArG4DetectorService::~LArG4DetectorService() {
}

std::vector<G4LogicalVolume *> larg4::LArG4DetectorService::doBuildLVs() {
    ColorReader* fReader = new ColorReader;
    G4GDMLParser *parser = new G4GDMLParser(fReader);
    parser->Read(gdmlFileName_);
    G4VPhysicalVolume *World = parser->GetWorldVolume();
    std::cout << World->GetTranslation() << std::endl << std::endl;
    std::cout << "Found World:  " << World-> GetName() << std::endl;
    std::cout << "World LV:  " << World->GetLogicalVolume()->GetName() << std::endl;
    G4LogicalVolumeStore *pLVStore = G4LogicalVolumeStore::GetInstance();
    std::cout << "Found " << pLVStore->size()
            << " logical volumes."
            << std::endl << std::endl;
    G4PhysicalVolumeStore *pPVStore = G4PhysicalVolumeStore::GetInstance();
    std::cout << "Found " << pPVStore->size()
            << " physical volumes."
            << std::endl << std::endl;
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    const G4GDMLAuxMapType* auxmap = parser->GetAuxMap();
    std::cout << "Found " << auxmap->size()
            << " volume(s) with auxiliary information."
            << std::endl << std::endl;
    for (G4GDMLAuxMapType::const_iterator iter = auxmap->begin();
            iter != auxmap->end(); iter++) {
        G4cout << "Volume " << ((*iter).first)->GetName()
                << " has the following list of auxiliary information: "
                << G4endl;
        for (G4GDMLAuxListType::const_iterator vit = (*iter).second.begin();
                vit != (*iter).second.end(); vit++) {
            std::cout << "--> Type: " << (*vit).type
                    << " Value: " << (*vit).value << std::endl;
	    if ((*vit).type == "StepLimit") {
                G4UserLimits *fStepLimit = new G4UserLimits(atof((*vit).value));
		std::cout <<",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,fStepLimit:  "<<atof((*vit).value)<<"  "<<atof((*vit).value)/CLHEP::cm <<std::endl;
                ((*iter).first)->SetUserLimits(fStepLimit);
            }
            if ((*vit).type == "SensDet") {
                if ((*vit).value == "DRCalorimeter") {
                    G4String name = ((*iter).first)->GetName() + "_DRCalorimeter";
		    artg4tk::DRCalorimeterSD* aDRCalorimeterSD = new artg4tk::DRCalorimeterSD(name);
                    SDman->AddNewDetector(aDRCalorimeterSD);
                    ((*iter).first)->SetSensitiveDetector(aDRCalorimeterSD);
                    std::cout << "Attaching sensitive Detector: " << (*vit).value
                            << " to Volume:  " << ((*iter).first)->GetName() << std::endl;
                    DetectorList.push_back(std::make_pair((*iter).first->GetName(), (*vit).value));
                } else if ((*vit).value == "Calorimeter") {
                    G4String name = ((*iter).first)->GetName() + "_Calorimeter";
		    artg4tk::CalorimeterSD* aCalorimeterSD = new artg4tk::CalorimeterSD(name);
                    SDman->AddNewDetector(aCalorimeterSD);
                    ((*iter).first)->SetSensitiveDetector(aCalorimeterSD);
                    std::cout << "Attaching sensitive Detector: " << (*vit).value
                            << " to Volume:  " << ((*iter).first)->GetName() << std::endl;
                    DetectorList.push_back(std::make_pair((*iter).first->GetName(), (*vit).value));
                } else if ((*vit).value == "PhotonDetector") {
                    G4String name = ((*iter).first)->GetName() + "_PhotonDetector";
		    artg4tk::PhotonSD* aPhotonSD = new artg4tk::PhotonSD(name);
                    SDman->AddNewDetector(aPhotonSD);
                    ((*iter).first)->SetSensitiveDetector(aPhotonSD);
                    std::cout << "Attaching sensitive Detector: " << (*vit).value
                            << " to Volume:  " << ((*iter).first)->GetName() << std::endl;
                    DetectorList.push_back(std::make_pair((*iter).first->GetName(), (*vit).value));
                } else if ((*vit).value == "Tracker") {
                    G4String name = ((*iter).first)->GetName() + "_Tracker";
                    artg4tk::TrackerSD* aTrackerSD = new artg4tk::TrackerSD(name);
                    SDman->AddNewDetector(aTrackerSD);
                    ((*iter).first)->SetSensitiveDetector(aTrackerSD);
                    std::cout << "Attaching sensitive Detector: " << (*vit).value
                            << " to Volume:  " << ((*iter).first)->GetName() << std::endl;
                    DetectorList.push_back(std::make_pair((*iter).first->GetName(), (*vit).value));
		} else if ((*vit).value == "SimEnergyDeposit") {
                    G4String name = ((*iter).first)->GetName() + "_SimEnergyDeposit";
		    SimEnergyDepositSD * aSimEnergyDepositSD = new SimEnergyDepositSD(name);
                    SDman->AddNewDetector(aSimEnergyDepositSD);
                    ((*iter).first)->SetSensitiveDetector(aSimEnergyDepositSD);
                    std::cout << "Attaching sensitive Detector: " << (*vit).value
                            << " to Volume:  " << ((*iter).first)->GetName() << std::endl;
                    DetectorList.push_back(std::make_pair((*iter).first->GetName(), (*vit).value));
                } else if ((*vit).value == "Interaction") {
                    G4String name = ((*iter).first)->GetName() + "_Interaction";
                    artg4tk::InteractionSD* aInteractionSD = new artg4tk::InteractionSD(name);
                    SDman->AddNewDetector(aInteractionSD);
                    ((*iter).first)->SetSensitiveDetector(aInteractionSD);
                    std::cout << "Attaching sensitive Detector: " << (*vit).value
                            << " to Volume:  " << ((*iter).first)->GetName() << std::endl;
                    DetectorList.push_back(std::make_pair((*iter).first->GetName(), (*vit).value));
                } else if ((*vit).value == "HadInteraction") {
                    G4String name = ((*iter).first)->GetName() + "_HadInteraction";
                    artg4tk::HadInteractionSD* aHadInteractionSD = new artg4tk::HadInteractionSD(name);
                    // NOTE: This will be done in the HadInteractionSD ctor
		    // SDman->AddNewDetector(aHadInteractionSD);
                    ((*iter).first)->SetSensitiveDetector(aHadInteractionSD);
                    std::cout << "Attaching sensitive Detector: " << (*vit).value
                            << " to Volume:  " << ((*iter).first)->GetName() << std::endl;
                    DetectorList.push_back(std::make_pair((*iter).first->GetName(), (*vit).value));
                } else if ( (*vit).value == "HadIntAndEdepTrk" ) {
                    G4String name = ((*iter).first)->GetName() + "_HadIntAndEdepTrk";
                    artg4tk::HadIntAndEdepTrkSD* aHadIntAndEdepTrkSD = new artg4tk::HadIntAndEdepTrkSD(name);
                    // NOTE: This will be done in the HadIntAndEdepTrkSD ctor
		    // SDman->AddNewDetector(aHadIntAndEdepTrkSD);
                    ((*iter).first)->SetSensitiveDetector(aHadIntAndEdepTrkSD);
                    std::cout << "Attaching sensitive Detector: " << (*vit).value
                            << " to Volume:  " << ((*iter).first)->GetName() << std::endl;
                    DetectorList.push_back(std::make_pair((*iter).first->GetName(), (*vit).value));
		}
            }
        }
    }
    G4cout << *(G4Material::GetMaterialTable());
    std::cout << "List SD Tree: " << std::endl;
    SDman->ListTree();
    std::cout << " Collection Capacity:  " << SDman->GetCollectionCapacity() << std::endl;
    G4HCtable* hctable = SDman->GetHCtable();
    for (G4int j = 0; j < SDman->GetCollectionCapacity(); j++) {
        std::cout << "HC Name: " << hctable->GetHCname(j) << "   SD Name:  " << hctable->GetSDname(j) << std::endl;
    }
    std::cout << "==================================================" << std::endl;
    // Return our logical volumes.
    std::vector<G4LogicalVolume *> myLVvec;
    myLVvec.push_back(pLVStore->at(0)); // only need to return the LV of the world
    std::cout << "nr of LV ======================:  " << myLVvec.size() << std::endl;
    return myLVvec;
}

std::vector<G4VPhysicalVolume *> larg4::LArG4DetectorService::doPlaceToPVs(std::vector<G4LogicalVolume *>) {
    // Note we don't use our input.
    std::vector<G4VPhysicalVolume *> myPVvec;
    G4PhysicalVolumeStore *pPVStore = G4PhysicalVolumeStore::GetInstance();
    myPVvec.push_back(pPVStore->at(pPVStore->size() - 1)); // only need to return the PV of the world  (last entry in Volume Store)
    return myPVvec;
}

void larg4::LArG4DetectorService::doCallArtProduces(art::EDProducer * producer) {
    // Tell Art what we produce, and label the entries
    std::vector<std::pair<std::string, std::string> >::const_iterator cii;
    for (cii = DetectorList.begin(); cii != DetectorList.end(); cii++) {
        if ((*cii).second == "DRCalorimeter") {
            std::string identifier = myName() +(*cii).first;
            producer -> produces<artg4tk::DRCalorimeterHitCollection>(identifier);
            std::string EdepID = identifier + "Edep";
            producer -> produces<artg4tk::myParticleEContribArtData>(EdepID);
            std::string NCerenID = identifier + "NCeren";
            producer -> produces<artg4tk::myParticleEContribArtData>(NCerenID);
        } else if ((*cii).second == "Calorimeter") {
            std::string identifier = myName() +(*cii).first;
            producer -> produces<artg4tk::CalorimeterHitCollection>(identifier);
        } else if ((*cii).second == "PhotonDetector") {
            std::string identifier = myName() + (*cii).first;
            producer -> produces<artg4tk::PhotonHitCollection>(identifier);
        } else if ((*cii).second == "Tracker") {
            std::string identifier = myName() + (*cii).first;
            producer -> produces<artg4tk::TrackerHitCollection>(identifier);
	} else if ((*cii).second == "SimEnergyDeposit") {
            std::string identifier = myName() + (*cii).first;
            producer -> produces<sim::SimEnergyDepositCollection>(identifier);
        } else if ((*cii).second == "Interaction") {
            std::string identifier = myName() + (*cii).first;
            producer -> produces<artg4tk::myInteractionArtHitDataCollection>(identifier);
        } else if ( (*cii).second == "HadInteraction") {
            // std::string identifier = myName() + (*cii).first;
            producer -> produces<artg4tk::ArtG4tkVtx>(); // do NOT use product instance name (for now)
        }
	else if ( (*cii).second == "HadIntAndEdepTrk" )
	{
	   producer->produces<artg4tk::ArtG4tkVtx>();
	   producer->produces<artg4tk::TrackerHitCollection>();
	} 
    }
}

void larg4::LArG4DetectorService::doFillEventWithArtHits(G4HCofThisEvent * myHC) {
  //
  // the following are left over sensitive detectors that write into the geant 4 hit collection first and then copy to the EDM
  //
    std::unique_ptr<artg4tk::DRCalorimeterHitCollection> myDRArtHits(new artg4tk::DRCalorimeterHitCollection);
    std::unique_ptr<artg4tk::myInteractionArtHitDataCollection> myInteractionHits(new artg4tk::myInteractionArtHitDataCollection);
    std::unique_ptr<artg4tk::myParticleEContribArtData> myEdepCon(new artg4tk::myParticleEContribArtData);
    std::unique_ptr<artg4tk::myParticleEContribArtData> myNCerenCon(new artg4tk::myParticleEContribArtData);
    
    // NOTE(JVY): 1st hadronic interaction will be fetched as-is from HadInteractionSD
    //            a copy (via copy ctor) will be placed directly into art::Event
    //            NO BUSINESS with G4HCofThisEvent !!!
    //
    std::vector<std::pair<std::string, std::string> >::const_iterator cii;
    std::cout<<"****************Detectorlist size:  "<< DetectorList.size()<<std::endl;
    for (cii = DetectorList.begin(); cii != DetectorList.end(); cii++) 
    {
       std::string sdname = (*cii).first + "_" + (*cii).second;
       std::cout<<"****************SDNAME:"<< sdname<<std::endl;
       if ( (*cii).second == "HadInteraction" )
       {
	  G4SDManager* sdman = G4SDManager::GetSDMpointer();
	  artg4tk::HadInteractionSD* hisd = dynamic_cast<artg4tk::HadInteractionSD*>(sdman->FindSensitiveDetector(sdname));
	  if ( hisd )
	  {
             const artg4tk::ArtG4tkVtx& inter = hisd->Get1stInteraction();
	     if ( inter.GetNumOutcoming() > 0 )
	     {
	        std::unique_ptr<artg4tk::ArtG4tkVtx> firstint(new artg4tk::ArtG4tkVtx(inter));
                art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
                art::Event & e = detectorHolder -> getCurrArtEvent();
                e.put(std::move(firstint)); // note that there's NO product instance name (for now, at least)
		                            // (part of) the is that the name is encoded into the "collection"
					    // which is NOT used in this specific case
	     } 	     
	     hisd->clear(); // clear out after movind info to EDM; no need to clea out in the producer !
	  }
       }
       else if ( (*cii).second == "HadIntAndEdepTrk" )
       {
	  G4SDManager* sdman = G4SDManager::GetSDMpointer();
	  artg4tk::HadIntAndEdepTrkSD* sd = dynamic_cast<artg4tk::HadIntAndEdepTrkSD*>(sdman->FindSensitiveDetector(sdname));
	  if ( sd )
	  {
             art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
             art::Event & e = detectorHolder -> getCurrArtEvent();
             const artg4tk::ArtG4tkVtx& inter = sd->Get1stInteraction();
	     if ( inter.GetNumOutcoming() > 0 )
	     {
	        std::unique_ptr<artg4tk::ArtG4tkVtx> firstint(new artg4tk::ArtG4tkVtx(inter));
                e.put(std::move(firstint)); // note that there's NO product instance name (for now, at least)
		                            // (part of) the is that the name is encoded into the "collection"
					    // which is NOT used in this specific case
	     }
	     const artg4tk::TrackerHitCollection& trkhits = sd->GetEdepTrkHits();
	     if ( !trkhits.empty() )
	     {
	        std::unique_ptr<artg4tk::TrackerHitCollection> hits(new artg4tk::TrackerHitCollection(trkhits)); 
		e.put(std::move(hits));
	     } 	     
	     sd->clear(); // clear out after moving info to EDM; no need to clea out in the producer !
	  }
       }
	else if ( (*cii).second == "Tracker") {
	  G4SDManager* sdman = G4SDManager::GetSDMpointer();
	  artg4tk::TrackerSD* trsd = dynamic_cast<artg4tk::TrackerSD*>(sdman->FindSensitiveDetector(sdname));
	  art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
	  art::Event & e = detectorHolder -> getCurrArtEvent();
	  const artg4tk::TrackerHitCollection& trkhits = trsd->GetHits();
	  std::unique_ptr<artg4tk::TrackerHitCollection> hits(new artg4tk::TrackerHitCollection(trkhits)); 
	  std::string identifier=myName()+(*cii).first;
	  e.put(std::move(hits), identifier);
	} 
	else if ( (*cii).second == "Calorimeter") {
	  G4SDManager* sdman = G4SDManager::GetSDMpointer();
	  artg4tk::CalorimeterSD* calsd = dynamic_cast<artg4tk::CalorimeterSD*>(sdman->FindSensitiveDetector(sdname));
	  art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
	  art::Event & e = detectorHolder -> getCurrArtEvent();
	  const artg4tk::CalorimeterHitCollection& calhits = calsd->GetHits();
	  std::unique_ptr<artg4tk::CalorimeterHitCollection> hits(new artg4tk::CalorimeterHitCollection(calhits)); 
	  std::string identifier=myName()+(*cii).first;
	  e.put(std::move(hits), identifier);
	} 
	else if ( (*cii).second == "DRCalorimeter") {
	  G4SDManager* sdman = G4SDManager::GetSDMpointer();
	  artg4tk::DRCalorimeterSD* drcalsd = dynamic_cast<artg4tk::DRCalorimeterSD*>(sdman->FindSensitiveDetector(sdname));
	  art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
	  art::Event & e = detectorHolder -> getCurrArtEvent();
	  const artg4tk::DRCalorimeterHitCollection& drcalhits = drcalsd->GetHits();
	  std::unique_ptr<artg4tk::DRCalorimeterHitCollection> hits(new artg4tk::DRCalorimeterHitCollection(drcalhits)); 
	  std::string identifier=myName()+(*cii).first;
	  e.put(std::move(hits), identifier);
	}


	else if ( (*cii).second == "SimEnergyDeposit") {
	  G4SDManager* sdman = G4SDManager::GetSDMpointer();
	  SimEnergyDepositSD* sedsd = dynamic_cast<SimEnergyDepositSD*>(sdman->FindSensitiveDetector(sdname));
	  art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
	  art::Event & e = detectorHolder -> getCurrArtEvent();
	  const sim::SimEnergyDepositCollection& sedhits = sedsd->GetHits();
	  std::unique_ptr<sim::SimEnergyDepositCollection> hits(new sim::SimEnergyDepositCollection(sedhits)); 
	  std::string identifier=myName()+(*cii).first;
	  e.put(std::move(hits), identifier);
	} 
	else if ( (*cii).second == "PhotonDetector") {
	  G4SDManager* sdman = G4SDManager::GetSDMpointer();
	  artg4tk::PhotonSD* phsd = dynamic_cast<artg4tk::PhotonSD*>(sdman->FindSensitiveDetector(sdname));
	  art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
	  art::Event & e = detectorHolder -> getCurrArtEvent();
	  const artg4tk::PhotonHitCollection& phhits = phsd->GetHits();
	  std::unique_ptr<artg4tk::PhotonHitCollection> hits(new artg4tk::PhotonHitCollection(phhits)); 
	  std::string identifier=myName()+(*cii).first;
	  e.put(std::move(hits), identifier);
        } 

    }    
        
    for (int i = 0; i < myHC->GetNumberOfCollections(); i++) {
      G4VHitsCollection* hc = myHC->GetHC(i);
      G4String hcname = hc->GetName();
      std::vector<std::string> y = split(hcname, '_');
      std::string Classname = y[1];
      std::string Volume = y[0];
      std::string SDName = y[0] + "_" + y[1];
      std::cout<<"Classname:    " << Classname<<std::endl;
      if (Classname == "Interaction") {
	G4int NbHits = hc->GetSize();
	G4cout << "===================    " << NbHits << G4endl;
	for (G4int ii = 0; ii < NbHits; ii++) {
	  G4VHit* hit = hc->GetHit(ii);
	  artg4tk::InteractionHit* Hit = dynamic_cast<artg4tk::InteractionHit*> (hit);
	  Hit->Print();
	  artg4tk::myInteractionArtHitData myInteractionhit = artg4tk::myInteractionArtHitData(
									     Hit->GetPname(),
									     Hit->GetPmom(),
									     Hit->GetEkin(),
									     Hit->GetTheta()
									     );
	  myInteractionHits->push_back(myInteractionhit);
	}
	// Now that we have our collection of artized hits, add them to the event
	art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
	art::Event & e = detectorHolder -> getCurrArtEvent();
	std::string dataname = myName() + Volume;
	e.put(std::move(myInteractionHits), dataname);  
      } else {
	G4cout << "SD type: " << Classname << " unknown" << G4endl;
      }
      
    };
}
using larg4::LArG4DetectorService;
DEFINE_ART_SERVICE(LArG4DetectorService)
