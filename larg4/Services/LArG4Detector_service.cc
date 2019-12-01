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
// Modified: David Rivera
//=============================================================================
// framework includes:
#include "art/Framework/Core/ProducesCollector.h"
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
#include <unordered_map>
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

larg4::LArG4DetectorService::LArG4DetectorService(fhicl::ParameterSet const & p)
: artg4tk::DetectorBase(p,
                        p.get<string>("name", "LArG4DetectorService"),
                        p.get<string>("category", "World"),
                        p.get<string>("mother_category", "")),
  gdmlFileName_( p.get<std::string>("gdmlFileName_","")),
  checkoverlaps_( p.get<bool>("CheckOverlaps",false)),
  volumeNames_( p.get<std::vector<std::string>>("volumeNames",{}) ),
  stepLimits_( p.get<std::vector<float>>("stepLimits",{}) ),
  dumpMP_( p.get<bool>("DumpMaterialProperties",false)),
  logInfo_( "LArG4DetectorService" )
{
  // -- D.R. : Check for valid volume, steplimit pairs
  if(volumeNames_.size() != stepLimits_.size()) {
    throw cet::exception("LArG4DetectorService") << "Configuration error: volumeNames:[] and"
                                                 << " stepLimits:[] have different sizes!" << "\n";
  }

  inputVolumes_ = volumeNames_.size();

  //-- define commonly used units, that we might need
  new G4UnitDefinition("volt/cm","V/cm","Electric field",CLHEP::volt/CLHEP::cm);

  if (inputVolumes_ > 0) { mf::LogInfo("LArG4DetectorService::Ctr") << "Reading stepLimit(s) from the configuration file, for volume(s):"; }
  for(size_t i=0; i<inputVolumes_; ++i){
    if(stepLimits_.at(i) < 0) {
      throw cet::exception("LArG4DetectorService") << "Invalid stepLimits found. Step limits must be"
                      << " positive! Bad value : stepLimits[" << i << "] = " << stepLimits_.at(i)
                      << " [mm]\n";
    } else {
      selectedVolumes_.push_back( std::make_pair( volumeNames_.at(i), stepLimits_.at(i) ) );
      mf::LogInfo("LArG4DetectorService::Ctr") << "Volume: " << volumeNames_.at(i)
                                               << ", stepLimit: " << stepLimits_.at(i);
    }//--check for negative
  } //--loop over inputVolumes
}//--Ctor

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
    std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
    for (G4GDMLAuxMapType::const_iterator iter = auxmap->begin();
        iter != auxmap->end(); iter++) {
        G4cout << "Volume " << ((*iter).first)->GetName()
               << " has the following list of auxiliary information: "
               << G4endl;
        for (G4GDMLAuxListType::const_iterator vit = (*iter).second.begin();
            vit != (*iter).second.end(); vit++) {
            G4cout << "--> Type: " << (*vit).type
                   << " Value: " << (*vit).value << std::endl;

            G4double value = atof((*vit).value);
            G4double val_unit = 1; //--no unit
            G4String provided_category = "NONE";
            if( ((*vit).unit) && ((*vit).unit != "") ) { // -- if provided and non-NULL
              val_unit = G4UnitDefinition::GetValueOf( (*vit).unit );
              provided_category = G4UnitDefinition::GetCategory((*vit).unit);
              mf::LogInfo("AuxUnit") << " Unit parsed = " << (*vit).unit << " from unit category: " << provided_category.c_str();
              value *= val_unit; //-- Now do something with the value, making sure that the unit is appropriate
            }

            if ((*vit).type == "StepLimit") {
                G4UserLimits *fStepLimit = NULL;

                //-- check that steplimit has valid length unit category
                G4String steplimit_category = "Length";
                if(provided_category == steplimit_category) {
                  mf::LogInfo("AuxUnit") << "Valid StepLimit unit category obtained: " << provided_category.c_str();
                  fStepLimit = new G4UserLimits(value);
                  G4cout << "fStepLimit:  " << value << "  " << value / CLHEP::cm << " cm" << std::endl;
                } else if (provided_category == "NONE"){ //--no unit category provided, use the default CLHEP::mm
                  MF_LOG_WARNING("StepLimitUnit") << "StepLimit in geometry file does not have a unit!"
                                                  << " Defaulting to mm...";
                  value *= CLHEP::mm;
                  fStepLimit = new G4UserLimits(value);
                  G4cout << "fStepLimit:  " << value << "  " << value / CLHEP::cm << " cm" << std::endl;
                } else { //--wrong unit category provided
                  throw cet::exception("StepLimitUnit") << "StepLimit does not have a valid length unit!\n"
                                                        << " Category of unit provided = " << provided_category << ".\n";
                }

                ((*iter).first)->SetUserLimits(fStepLimit);
                // -- D.R. insert into map <volName,stepLimit> to cross-check later
                MF_LOG_DEBUG("LArG4DetectorService::") << "Set stepLimit for volume: " << ((*iter).first)->GetName()
                        << " from the GDML file.";
                setGDMLVolumes_.insert(std::make_pair( ((*iter).first)->GetName(), atof((*vit).value) ));
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
        std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
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
    if (inputVolumes_ > 0) {
      setStepLimits(parser);
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

void larg4::LArG4DetectorService::setStepLimits(G4GDMLParser *parser){
  // -- D. Rivera : This function sets step limits for volumes provided in the configuration file
  //                and overrides the step limit (if any) set for the same volumes but from the GMDL
  //                geometry file. The GDML step limit (if provided in the gdml file) is set first
  //                and later overriden by this method if a valid volumeName,setStepLimit is provided.
  MF_LOG_WARNING("LArG4DetectorService::setStepLimits") << "Setting step limits from configuration"
                  << " file. This will OVERRIDE redundant stepLimit(s) set in the GDML file. Note"
                  << " that stepLimits are only active if enabled in the physicsListService via the"
                  << " appropriate parameter.";

  std::string volumeName  = "";
  float previousStepLimit = 0.;
  float newStepLimit      = 0.;
  for(size_t i=0; i<inputVolumes_; ++i)
  {
    // -- Check whether the volumeName provided corresponds to a valid volumeName in the geometry
    G4LogicalVolume* setVol = parser->GetVolume(selectedVolumes_[i].first);

    // -- get the G4LogicalVolume corresponding to the selectedVolume
    volumeName = setVol->GetName();
    newStepLimit = selectedVolumes_[i].second;
    MF_LOG_DEBUG("LArG4DetectorService::setStepLimits") << "Got logical volume with name: "
                                                        << volumeName;

    // -- check if a stepLimit for this volume has been set before:
    auto search = setGDMLVolumes_.find(volumeName);
    if(search != setGDMLVolumes_.end())
    {
      previousStepLimit = search->second;
      if (newStepLimit != previousStepLimit) {
        MF_LOG_WARNING("LArG4DetectorService::setStepLimits") << "OVERRIDING PREVIOUSLY SET"
                    << " STEPLIMIT FOR VOLUME : " << volumeName
                    << " FROM " << previousStepLimit << " mm TO " << newStepLimit << " mm";
      } else {
        MF_LOG_WARNING("LArG4DetectorService::setStepLimits") << "New stepLimit matches previously"
                    << " set stepLimit from the GDML file for volume : " << volumeName
                    << " stepLimit : " << newStepLimit << " mm. Nothing will be changed.";
        continue;
      }
    }//--check if new steplimit differs from a previously set value

    G4UserLimits *fStepLimitOverride = new G4UserLimits(selectedVolumes_[i].second);
    mf::LogInfo("LArG4DetectorService::setStepLimits") << "fStepLimitOverride:  "
              << selectedVolumes_[i].second << "  "
              << (selectedVolumes_[i].second * CLHEP::mm) / CLHEP::cm << " cm"
              << " for volume: " << selectedVolumes_[i].first; 
    setVol->SetUserLimits(fStepLimitOverride);
  }//--loop over input volumes
}//--end of setStepLimit()

void larg4::LArG4DetectorService::doCallArtProduces(art::ProducesCollector& collector) {
    // Tell Art what we produce, and label the entries
    std::vector<std::pair<std::string, std::string> >::const_iterator cii;
    for (cii = DetectorList.begin(); cii != DetectorList.end(); cii++) {
        if ((*cii).second == "DRCalorimeter") {
            std::string identifier = myName() +(*cii).first;
            collector.produces<artg4tk::DRCalorimeterHitCollection>(identifier);
            std::string EdepID = identifier + "Edep";
            collector.produces<artg4tk::ByParticle>(EdepID);
            std::string NCerenID = identifier + "NCeren";
            collector.produces<artg4tk::ByParticle>(NCerenID);
        } else if ((*cii).second == "Calorimeter") {
            std::string identifier = myName() +(*cii).first;
            collector.produces<artg4tk::CalorimeterHitCollection>(identifier);
        } else if ((*cii).second == "PhotonDetector") {
            std::string identifier = myName() + (*cii).first;
            collector.produces<artg4tk::PhotonHitCollection>(identifier);
        } else if ((*cii).second == "Tracker") {
            std::string identifier = myName()+(*cii).first;
            collector.produces<artg4tk::TrackerHitCollection>(identifier);
        } else if ((*cii).second == "SimEnergyDeposit") {
            std::string identifier = myName() + (*cii).first;
            collector.produces<sim::SimEnergyDepositCollection>(identifier);
        } else if ((*cii).second == "AuxDet") {
            std::string identifier = myName() + (*cii).first;
            collector.produces<sim::AuxDetHitCollection>(identifier);
        } else if ((*cii).second == "HadInteraction") {
            // std::string identifier = myName() + (*cii).first;
            collector.produces<artg4tk::ArtG4tkVtx>(); // do NOT use product instance name (for now)
        } else if ((*cii).second == "HadIntAndEdepTrk") {
            collector.produces<artg4tk::ArtG4tkVtx>();
            collector.produces<artg4tk::TrackerHitCollection>();
        }
    }
}

void larg4::LArG4DetectorService::doFillEventWithArtHits(G4HCofThisEvent * myHC) {
    //
    // NOTE(JVY): 1st hadronic interaction will be fetched as-is from HadInteractionSD
    //            a copy (via copy ctor) will be placed directly into art::Event
    //
    std::vector<std::pair<std::string, std::string> >::const_iterator cii;
    //    std::cout << "****************Detectorlist size:  " << DetectorList.size() << std::endl;
    for (cii = DetectorList.begin(); cii != DetectorList.end(); cii++) {
        std::string sdname = (*cii).first + "_" + (*cii).second;
        //  std::cout << "****************SDNAME:" << sdname << std::endl;
        if ((*cii).second == "HadInteraction") {
            G4SDManager* sdman = G4SDManager::GetSDMpointer();
            artg4tk::HadInteractionSD* hisd = dynamic_cast<artg4tk::HadInteractionSD*> (sdman->FindSensitiveDetector(sdname));
            if (hisd) {
                const artg4tk::ArtG4tkVtx& inter = hisd->Get1stInteraction();
                if (inter.GetNumOutcoming() > 0) {
                    auto firstint = std::make_unique<artg4tk::ArtG4tkVtx>(inter);
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
                    auto firstint = std::make_unique<artg4tk::ArtG4tkVtx>(inter);
                    e.put(std::move(firstint)); // note that there's NO product instance name (for now, at least)
                    // (part of) the is that the name is encoded into the "collection"
                    // which is NOT used in this specific case
                }
                const artg4tk::TrackerHitCollection& trkhits = sd->GetEdepTrkHits();
                if (!trkhits.empty()) {
                    auto hits = std::make_unique<artg4tk::TrackerHitCollection>(trkhits);
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
            auto hits = std::make_unique<artg4tk::TrackerHitCollection>(trkhits);
            std::string identifier = myName()+(*cii).first;
            e.put(std::move(hits), identifier);
        }else if ( (*cii).second == "SimEnergyDeposit") {
          G4SDManager* sdman = G4SDManager::GetSDMpointer();
          SimEnergyDepositSD* sedsd = dynamic_cast<SimEnergyDepositSD*>(sdman->FindSensitiveDetector(sdname));
          art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
          art::Event & e = detectorHolder -> getCurrArtEvent();
          const sim::SimEnergyDepositCollection& sedhits = sedsd->GetHits();
          auto hits = std::make_unique<sim::SimEnergyDepositCollection>(sedhits);
          std::string identifier=myName()+(*cii).first;
          e.put(std::move(hits), identifier);
        } else if ( (*cii).second == "AuxDet") {
          G4SDManager* sdman = G4SDManager::GetSDMpointer();
          AuxDetSD* auxsd = dynamic_cast<AuxDetSD*>(sdman->FindSensitiveDetector(sdname));
          art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
          art::Event & e = detectorHolder -> getCurrArtEvent();
          const sim::AuxDetHitCollection& auxhits = auxsd->GetHits();
          auto hits = std::make_unique<sim::AuxDetHitCollection>(auxhits);
          std::string identifier=myName()+(*cii).first;
          e.put(std::move(hits), identifier);
        } else if ((*cii).second == "Calorimeter") {
            G4SDManager* sdman = G4SDManager::GetSDMpointer();
            artg4tk::CalorimeterSD* calsd = dynamic_cast<artg4tk::CalorimeterSD*> (sdman->FindSensitiveDetector(sdname));
            art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
            art::Event & e = detectorHolder -> getCurrArtEvent();
            const artg4tk::CalorimeterHitCollection& calhits = calsd->GetHits();
            auto hits = std::make_unique<artg4tk::CalorimeterHitCollection>(calhits);
            std::string identifier = myName()+(*cii).first;
            e.put(std::move(hits), identifier);
        } else if ((*cii).second == "DRCalorimeter") {
            G4SDManager* sdman = G4SDManager::GetSDMpointer();
            artg4tk::DRCalorimeterSD* drcalsd = dynamic_cast<artg4tk::DRCalorimeterSD*> (sdman->FindSensitiveDetector(sdname));
            art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
            art::Event & e = detectorHolder -> getCurrArtEvent();
            const artg4tk::DRCalorimeterHitCollection& drcalhits = drcalsd->GetHits();
            auto hits = std::make_unique<artg4tk::DRCalorimeterHitCollection>(drcalhits);
            std::string identifier = myName()+(*cii).first;
            e.put(std::move(hits), identifier);
            //
            const artg4tk::ByParticle& edeps = drcalsd->GetEbyParticle();
            auto edepsptr = std::make_unique<artg4tk::ByParticle>(edeps);
            std::string edidentifier = myName()+(*cii).first + "Edep";
            e.put(std::move(edepsptr), edidentifier);
            //
            const artg4tk::ByParticle& nceren = drcalsd->GetNCerenbyParticle();
            auto ncerenptr = std::make_unique<artg4tk::ByParticle>(nceren);
            std::string ncidentifier = myName()+(*cii).first + "NCeren";
            e.put(std::move(ncerenptr), ncidentifier);
        } else if ((*cii).second == "PhotonDetector") {
            G4SDManager* sdman = G4SDManager::GetSDMpointer();
            artg4tk::PhotonSD* phsd = dynamic_cast<artg4tk::PhotonSD*> (sdman->FindSensitiveDetector(sdname));
            art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
            art::Event & e = detectorHolder -> getCurrArtEvent();
            const artg4tk::PhotonHitCollection& phhits = phsd->GetHits();
            auto hits = std::make_unique<artg4tk::PhotonHitCollection>(phhits);
            std::string identifier = myName()+(*cii).first;
            e.put(std::move(hits), identifier);
        }
    }
}
using larg4::LArG4DetectorService;
DEFINE_ART_SERVICE(LArG4DetectorService)
