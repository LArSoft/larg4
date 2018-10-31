
//=============================================================================
// LArG4Detector_service.hh: 
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
#include "cetlib/search_path.h"
 // larg4 includes: 
#include "larg4/Services/LArG4Detector_service.h"
// artg4tk includes: 
#include "artg4tk/pluginDetectors/gdml/ColorReader.hh"
#include "artg4tk/pluginDetectors/gdml/CalorimeterSD.hh"
#include "artg4tk/pluginDetectors/gdml/CalorimeterHit.hh"
#include "artg4tk/pluginDetectors/gdml/DRCalorimeterSD.hh"
#include "artg4tk/pluginDetectors/gdml/DRCalorimeterHit.hh"
#include "artg4tk/pluginDetectors/gdml/ByParticle.hh"
#include "artg4tk/pluginDetectors/gdml/PhotonSD.hh"
#include "artg4tk/pluginDetectors/gdml/PhotonHit.hh"
#include "artg4tk/pluginDetectors/gdml/TrackerSD.hh"
#include "artg4tk/pluginDetectors/gdml/TrackerHit.hh"
#include "larg4/Services/SimEnergyDepositSD.h"
#include "lardataobj/Simulation/SimEnergyDeposit.h"
#include "larg4/Services/AuxDetSD.h"
#include "lardataobj/Simulation/AuxDetHit.h"
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
#include "Geant4/G4RegionStore.hh"
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
  gdmlFileName_( p.get<std::string>("gdmlFileName_","")),
  checkoverlaps_( p.get<bool>("CheckOverlaps",false)),
  dumpMP_( p.get<bool>("DumpMaterialProperties",false)),
  logInfo_("LArG4DetectorService") {

}

// Destructor

larg4::LArG4DetectorService::~LArG4DetectorService() {
}

std::vector<G4LogicalVolume *> larg4::LArG4DetectorService::doBuildLVs() {
    ColorReader* fReader = new ColorReader;
    G4GDMLParser *parser = new G4GDMLParser(fReader);
    parser->SetOverlapCheck(checkoverlaps_);
    cet::search_path sp{"FW_SEARCH_PATH"};
    std::string fullGDMLFileName;
    if (!sp.find_file(gdmlFileName_, fullGDMLFileName)) {
      throw cet::exception("LArG4DetectorService") << "Cannot find file: " << gdmlFileName_;
    }
    parser->Read(fullGDMLFileName);
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
                std::cout << "fStepLimit:  " << atof((*vit).value) << "  " << atof((*vit).value) / CLHEP::cm << " cm" << std::endl;
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
		} else if ((*vit).value == "AuxDet") {
                    G4String name = ((*iter).first)->GetName() + "_AuxDet";
		    AuxDetSD * aAuxDetSD = new AuxDetSD(name);
                    SDman->AddNewDetector(aAuxDetSD);
                    ((*iter).first)->SetSensitiveDetector(aAuxDetSD);
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
                } else if ((*vit).value == "HadIntAndEdepTrk") {
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
    if (dumpMP_)
      {
	G4cout << *(G4Material::GetMaterialTable());
	/*
	G4Region* region = G4RegionStore::GetInstance()->GetRegion("DefaultRegionForTheWorld", false);
	std::vector<G4Material*>::const_iterator mItr = region->GetMaterialIterator();
	size_t nMaterial = region->GetNumberOfMaterials();
	G4cout << nMaterial << G4endl;
	for (size_t iMate = 0; iMate < nMaterial; iMate++) {
	  G4cout << (*mItr)->GetName() << G4endl;
	  G4MaterialPropertiesTable* mt = (*mItr)->GetMaterialPropertiesTable();
	  if (mt != nullptr) {
            mt->DumpTable();
	    //            if (mt->GetProperty("SLOWCOMPONENT", true) != nullptr) {
	    //  mt->GetProperty("SLOWCOMPONENT", true)->SetSpline(true);
            //    std::cout << "Scint " << mt->GetProperty("SLOWCOMPONENT", true)->GetVectorLength()<< std::endl;
            //}
        }
        mItr++;
    }
    G4cout << G4endl;
*/
}
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
            producer -> produces<artg4tk::ByParticle>(EdepID);
            std::string NCerenID = identifier + "NCeren";
            producer -> produces<artg4tk::ByParticle>(NCerenID);
        } else if ((*cii).second == "Calorimeter") {
            std::string identifier = myName() +(*cii).first;
            producer -> produces<artg4tk::CalorimeterHitCollection>(identifier);
        } else if ((*cii).second == "PhotonDetector") {
            std::string identifier = myName() + (*cii).first;
            producer -> produces<artg4tk::PhotonHitCollection>(identifier);
        } else if ((*cii).second == "Tracker") {
            std::string identifier = myName()+(*cii).first;
            producer -> produces<artg4tk::TrackerHitCollection>(identifier);
	} else if ((*cii).second == "SimEnergyDeposit") {
            std::string identifier = myName() + (*cii).first;
            producer -> produces<sim::SimEnergyDepositCollection>(identifier);
	} else if ((*cii).second == "AuxDet") {
            std::string identifier = myName() + (*cii).first;
            producer -> produces<sim::AuxDetHitCollection>(identifier);
        } else if ((*cii).second == "HadInteraction") {
            // std::string identifier = myName() + (*cii).first;
            producer -> produces<artg4tk::ArtG4tkVtx>(); // do NOT use product instance name (for now)
        } else if ((*cii).second == "HadIntAndEdepTrk") {
            producer->produces<artg4tk::ArtG4tkVtx>();
            producer->produces<artg4tk::TrackerHitCollection>();
        }
    }
}

void larg4::LArG4DetectorService::doFillEventWithArtHits(G4HCofThisEvent * myHC) {
        //
    // NOTE(JVY): 1st hadronic interaction will be fetched as-is from HadInteractionSD
    //            a copy (via copy ctor) will be placed directly into art::Event
    //
    std::vector<std::pair<std::string, std::string> >::const_iterator cii;
    std::cout << "****************Detectorlist size:  " << DetectorList.size() << std::endl;
    for (cii = DetectorList.begin(); cii != DetectorList.end(); cii++) {
        std::string sdname = (*cii).first + "_" + (*cii).second;
        std::cout << "****************SDNAME:" << sdname << std::endl;
        if ((*cii).second == "HadInteraction") {
            G4SDManager* sdman = G4SDManager::GetSDMpointer();
            artg4tk::HadInteractionSD* hisd = dynamic_cast<artg4tk::HadInteractionSD*> (sdman->FindSensitiveDetector(sdname));
            if (hisd) {
                const artg4tk::ArtG4tkVtx& inter = hisd->Get1stInteraction();
                if (inter.GetNumOutcoming() > 0) {
                    std::unique_ptr<artg4tk::ArtG4tkVtx> firstint(new artg4tk::ArtG4tkVtx(inter));
                    art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
                    art::Event & e = detectorHolder -> getCurrArtEvent();
                    e.put(std::move(firstint)); // note that there's NO product instance name (for now, at least)
                    // (part of) the is that the name is encoded into the "collection"
                    // which is NOT used in this specific case
                }
                hisd->clear(); // clear out after movind info to EDM; no need to clea out in the producer !
            }
        } else if ((*cii).second == "HadIntAndEdepTrk") {
            G4SDManager* sdman = G4SDManager::GetSDMpointer();
            artg4tk::HadIntAndEdepTrkSD* sd = dynamic_cast<artg4tk::HadIntAndEdepTrkSD*> (sdman->FindSensitiveDetector(sdname));
            if (sd) {
                art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
                art::Event & e = detectorHolder -> getCurrArtEvent();
                const artg4tk::ArtG4tkVtx& inter = sd->Get1stInteraction();
                if (inter.GetNumOutcoming() > 0) {
                    std::unique_ptr<artg4tk::ArtG4tkVtx> firstint(new artg4tk::ArtG4tkVtx(inter));
                    e.put(std::move(firstint)); // note that there's NO product instance name (for now, at least)
                    // (part of) the is that the name is encoded into the "collection"
                    // which is NOT used in this specific case
                }
                const artg4tk::TrackerHitCollection& trkhits = sd->GetEdepTrkHits();
                if (!trkhits.empty()) {
                    std::unique_ptr<artg4tk::TrackerHitCollection> hits(new artg4tk::TrackerHitCollection(trkhits));
                    e.put(std::move(hits));
                }
                sd->clear(); // clear out after moving info to EDM; no need to clea out in the producer !
            }
        } else if ((*cii).second == "Tracker") {
            G4SDManager* sdman = G4SDManager::GetSDMpointer();
            artg4tk::TrackerSD* trsd = dynamic_cast<artg4tk::TrackerSD*> (sdman->FindSensitiveDetector(sdname));
            art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
            art::Event & e = detectorHolder -> getCurrArtEvent();
            const artg4tk::TrackerHitCollection& trkhits = trsd->GetHits();
            std::unique_ptr<artg4tk::TrackerHitCollection> hits(new artg4tk::TrackerHitCollection(trkhits));
            std::string identifier = myName()+(*cii).first;
            e.put(std::move(hits), identifier);
	}else if ( (*cii).second == "SimEnergyDeposit") {
	  G4SDManager* sdman = G4SDManager::GetSDMpointer();
	  SimEnergyDepositSD* sedsd = dynamic_cast<SimEnergyDepositSD*>(sdman->FindSensitiveDetector(sdname));
	  art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
	  art::Event & e = detectorHolder -> getCurrArtEvent();
	  const sim::SimEnergyDepositCollection& sedhits = sedsd->GetHits();
	  std::unique_ptr<sim::SimEnergyDepositCollection> hits(new sim::SimEnergyDepositCollection(sedhits)); 
	  std::string identifier=myName()+(*cii).first;
	  e.put(std::move(hits), identifier);
 	} else if ( (*cii).second == "AuxDet") {
	  G4SDManager* sdman = G4SDManager::GetSDMpointer();
	  AuxDetSD* auxsd = dynamic_cast<AuxDetSD*>(sdman->FindSensitiveDetector(sdname));
	  art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
	  art::Event & e = detectorHolder -> getCurrArtEvent();
	  const sim::AuxDetHitCollection& auxhits = auxsd->GetHits();
	  std::unique_ptr<sim::AuxDetHitCollection> hits(new sim::AuxDetHitCollection(auxhits)); 
	  std::string identifier=myName()+(*cii).first;
	  e.put(std::move(hits), identifier);
        } else if ((*cii).second == "Calorimeter") {
            G4SDManager* sdman = G4SDManager::GetSDMpointer();
            artg4tk::CalorimeterSD* calsd = dynamic_cast<artg4tk::CalorimeterSD*> (sdman->FindSensitiveDetector(sdname));
            art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
            art::Event & e = detectorHolder -> getCurrArtEvent();
            const artg4tk::CalorimeterHitCollection& calhits = calsd->GetHits();
            std::unique_ptr<artg4tk::CalorimeterHitCollection> hits(new artg4tk::CalorimeterHitCollection(calhits));
            std::string identifier = myName()+(*cii).first;
            e.put(std::move(hits), identifier);
        } else if ((*cii).second == "DRCalorimeter") {
            G4SDManager* sdman = G4SDManager::GetSDMpointer();
            artg4tk::DRCalorimeterSD* drcalsd = dynamic_cast<artg4tk::DRCalorimeterSD*> (sdman->FindSensitiveDetector(sdname));
            art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
            art::Event & e = detectorHolder -> getCurrArtEvent();
            const artg4tk::DRCalorimeterHitCollection& drcalhits = drcalsd->GetHits();
            std::unique_ptr<artg4tk::DRCalorimeterHitCollection> hits(new artg4tk::DRCalorimeterHitCollection(drcalhits));
            std::string identifier = myName()+(*cii).first;
            e.put(std::move(hits), identifier);
            //
            const artg4tk::ByParticle& edeps = drcalsd->GetEbyParticle();
            std::unique_ptr<artg4tk::ByParticle> edepsptr(new artg4tk::ByParticle(edeps));
            std::string edidentifier = myName()+(*cii).first + "Edep";
            e.put(std::move(edepsptr), edidentifier);
            //
            const artg4tk::ByParticle& nceren = drcalsd->GetNCerenbyParticle();
            std::unique_ptr<artg4tk::ByParticle> ncerenptr(new artg4tk::ByParticle(nceren));
            std::string ncidentifier = myName()+(*cii).first + "NCeren";
            e.put(std::move(ncerenptr), ncidentifier);
        } else if ((*cii).second == "PhotonDetector") {
            G4SDManager* sdman = G4SDManager::GetSDMpointer();
            artg4tk::PhotonSD* phsd = dynamic_cast<artg4tk::PhotonSD*> (sdman->FindSensitiveDetector(sdname));
            art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
            art::Event & e = detectorHolder -> getCurrArtEvent();
            const artg4tk::PhotonHitCollection& phhits = phsd->GetHits();
            std::unique_ptr<artg4tk::PhotonHitCollection> hits(new artg4tk::PhotonHitCollection(phhits));
            std::string identifier = myName()+(*cii).first;
            e.put(std::move(hits), identifier);
        }
    }
}
using larg4::LArG4DetectorService;
DEFINE_ART_SERVICE(LArG4DetectorService)
