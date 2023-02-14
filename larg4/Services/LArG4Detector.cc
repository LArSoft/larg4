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
#include "larg4/Services/AuxDetSD.h"
#include "larg4/Services/LArG4Detector_service.h"
#include "larg4/Services/SimEnergyDepositSD.h"
#include "larg4/pluginActions/ParticleListAction_service.h"
// artg4tk includes:
#include "artg4tk/pluginDetectors/gdml/ByParticle.hh"
#include "artg4tk/pluginDetectors/gdml/CalorimeterHit.hh"
#include "artg4tk/pluginDetectors/gdml/CalorimeterSD.hh"
#include "artg4tk/pluginDetectors/gdml/ColorReader.hh"
#include "artg4tk/pluginDetectors/gdml/DRCalorimeterHit.hh"
#include "artg4tk/pluginDetectors/gdml/DRCalorimeterSD.hh"
#include "artg4tk/pluginDetectors/gdml/HadIntAndEdepTrkSD.hh"
#include "artg4tk/pluginDetectors/gdml/HadInteractionSD.hh"
#include "artg4tk/pluginDetectors/gdml/PhotonHit.hh"
#include "artg4tk/pluginDetectors/gdml/PhotonSD.hh"
#include "artg4tk/pluginDetectors/gdml/TrackerHit.hh"
#include "artg4tk/pluginDetectors/gdml/TrackerSD.hh"
//lardataobj includes:
#include "lardataobj/Simulation/AuxDetHit.h"
#include "lardataobj/Simulation/SimEnergyDeposit.h"
// Geant 4 includes:
#include "Geant4/G4AutoDelete.hh"
#include "Geant4/G4GDMLParser.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4LogicalVolumeStore.hh"
#include "Geant4/G4PhysicalVolumeStore.hh"
#include "Geant4/G4RegionStore.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4StepLimiter.hh"
#include "Geant4/G4Types.hh"
#include "Geant4/G4UnitsTable.hh"
#include "Geant4/G4UserLimits.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4VUserDetectorConstruction.hh"
#include "Geant4/globals.hh"

// C++ includes
#include <unordered_map>

using std::string;

namespace {
  template <typename T>
  auto make_product(T t)
  {
    return std::make_unique<T>(std::move(t));
  }
}

larg4::LArG4DetectorService::LArG4DetectorService(fhicl::ParameterSet const& p)
  : artg4tk::DetectorBase(p,
                          p.get<string>("name", "LArG4DetectorService"),
                          p.get<string>("category", "World"),
                          p.get<string>("mother_category", ""))
  , gdmlFileName_{p.get<std::string>("gdmlFileName_", "")}
  , checkOverlaps_{p.get<bool>("CheckOverlaps", false)}
  , updateSimEnergyDeposits_{p.get<bool>("UpdateSimEnergyDeposits", true)}
  , updateAuxDetHits_{p.get<bool>("UpdateAuxDetHits", true)}
  , volumeNames_{p.get<std::vector<std::string>>("volumeNames", {})}
  , stepLimits_{p.get<std::vector<float>>("stepLimits", {})}
  , inputVolumes_{size(volumeNames_)}
  , dumpMP_{p.get<bool>("DumpMaterialProperties", false)}
{
  // Make sure units are defined.
  G4UnitDefinition::GetUnitsTable();

  // -- D.R. : Check for valid volume, steplimit pairs
  if (inputVolumes_ != size(stepLimits_)) {
    throw cet::exception("LArG4DetectorService") << "Configuration error: volumeNames:[] and"
                                                 << " stepLimits:[] have different sizes!"
                                                 << "\n";
  }

  //-- define commonly used units, that we might need
  new G4UnitDefinition("volt/cm", "V/cm", "Electric field", CLHEP::volt / CLHEP::cm);

  if (inputVolumes_ > 0) {
    mf::LogInfo("LArG4DetectorService::Ctr")
      << "Reading stepLimit(s) from the configuration file, for volume(s):";
  }
  for (size_t i = 0; i < inputVolumes_; ++i) {
    if (stepLimits_[i] < 0) {
      throw cet::exception("LArG4DetectorService")
        << "Invalid stepLimits found. Step limits must be"
        << " positive! Bad value : stepLimits[" << i << "] = " << stepLimits_.at(i) << " [mm]\n";
    }

    overrideGDMLStepLimit_Map.emplace(volumeNames_[i], stepLimits_[i] * CLHEP::mm);
    mf::LogInfo("LArG4DetectorService::Ctr")
      << "Volume: " << volumeNames_[i] << ", stepLimit: " << stepLimits_[i];
  } //--loop over inputVolumes
} //--Ctor

// Destructor

std::vector<G4LogicalVolume*> larg4::LArG4DetectorService::doBuildLVs()
{
  ColorReader reader;
  G4GDMLParser parser(&reader);
  parser.SetOverlapCheck(checkOverlaps_);
  cet::search_path sp{"FW_SEARCH_PATH"};
  std::string fullGDMLFileName;
  if (!sp.find_file(gdmlFileName_, fullGDMLFileName)) {
    throw cet::exception("LArG4DetectorService") << "Cannot find file: " << gdmlFileName_;
  }
  parser.Read(fullGDMLFileName, false);
  G4VPhysicalVolume* World = parser.GetWorldVolume();

  std::stringstream ss;
  ss << World->GetTranslation() << "\n\n";
  ss << "Found World:  " << World->GetName() << "\n";
  ss << "World LV:  " << World->GetLogicalVolume()->GetName() << "\n";
  G4LogicalVolumeStore* pLVStore = G4LogicalVolumeStore::GetInstance();
  ss << "Found " << pLVStore->size() << " logical volumes."
     << "\n\n";
  G4PhysicalVolumeStore* pPVStore = G4PhysicalVolumeStore::GetInstance();
  ss << "Found " << pPVStore->size() << " physical volumes."
     << "\n\n";
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  const G4GDMLAuxMapType* auxmap = parser.GetAuxMap();
  ss << "Found " << auxmap->size() << " volume(s) with auxiliary information."
     << "\n\n";
  ss << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
  mf::LogInfo("LArG4DetectorService::doBuildLVs") << ss.str();

  for (auto const& [volume, auxes] : *auxmap) {
    G4cout << "Volume " << volume->GetName()
           << " has the following list of auxiliary information: \n";
    for (auto const& aux : auxes) {
      G4cout << "--> Type: " << aux.type << " Value: " << aux.value << "\n";

      G4double value = atof(aux.value);
      G4double val_unit = 1; //--no unit
      G4String provided_category = "NONE";
      if ((aux.unit) && (aux.unit != "")) { // -- if provided and non-NULL
        val_unit = G4UnitDefinition::GetValueOf(aux.unit);
        provided_category = G4UnitDefinition::GetCategory(aux.unit);
        mf::LogInfo("AuxUnit") << " Unit parsed = " << aux.unit
                               << " from unit category: " << provided_category.c_str();
        value *=
          val_unit; //-- Now do something with the value, making sure that the unit is appropriate
      }

      if (aux.type == "StepLimit") {
        G4UserLimits* fStepLimit = new G4UserLimits();
        G4AutoDelete::Register(fStepLimit);

        //-- check that steplimit has valid length unit category
        G4String steplimit_category = "Length";
        if (provided_category == steplimit_category) {
          mf::LogInfo("AuxUnit") << "Valid StepLimit unit category obtained: "
                                 << provided_category.c_str();
          // -- convert length to mm
          value = (value / CLHEP::mm) * CLHEP::mm;
          fStepLimit->SetMaxAllowedStep(value);
          mf::LogInfo("fStepLimit")
            << "fStepLimit:  " << value << "  " << value / CLHEP::cm << " cm\n";
        }
        else if (provided_category ==
                 "NONE") { //--no unit category provided, use the default CLHEP::mm
          MF_LOG_WARNING("StepLimitUnit") << "StepLimit in geometry file does not have a unit!"
                                          << " Defaulting to mm...";
          value *= CLHEP::mm;
          fStepLimit->SetMaxAllowedStep(value);
          mf::LogInfo("fStepLimit")
            << "fStepLimit:  " << value << "  " << value / CLHEP::cm << " cm\n";
        }
        else { //--wrong unit category provided
          throw cet::exception("StepLimitUnit")
            << "StepLimit does not have a valid length unit!\n"
            << " Category of unit provided = " << provided_category << ".\n";
        }

        volume->SetUserLimits(fStepLimit);
        // -- D.R. insert into map <volName,stepLimit> to cross-check later
        MF_LOG_DEBUG("LArG4DetectorService::")
          << "Set stepLimit for volume: " << volume->GetName() << " from the GDML file.";
        setGDMLVolumes_.insert(std::make_pair(volume->GetName(), (float)(value / CLHEP::mm)));
      }
      if (aux.type == "SensDet") {
        if (aux.value == "DRCalorimeter") {
          G4String name = volume->GetName() + "_DRCalorimeter";
          artg4tk::DRCalorimeterSD* aDRCalorimeterSD = new artg4tk::DRCalorimeterSD(name);
          SDman->AddNewDetector(aDRCalorimeterSD);
          volume->SetSensitiveDetector(aDRCalorimeterSD);
          std::cout << "Attaching sensitive Detector: " << aux.value
                    << " to Volume:  " << volume->GetName() << "\n";
          detectors_.emplace_back(volume->GetName(), aux.value);
        }
        else if (aux.value == "Calorimeter") {
          G4String name = volume->GetName() + "_Calorimeter";
          artg4tk::CalorimeterSD* aCalorimeterSD = new artg4tk::CalorimeterSD(name);
          SDman->AddNewDetector(aCalorimeterSD);
          volume->SetSensitiveDetector(aCalorimeterSD);
          std::cout << "Attaching sensitive Detector: " << aux.value
                    << " to Volume:  " << volume->GetName() << "\n";
          detectors_.emplace_back(volume->GetName(), aux.value);
        }
        else if (aux.value == "PhotonDetector") {
          G4String name = volume->GetName() + "_PhotonDetector";
          artg4tk::PhotonSD* aPhotonSD = new artg4tk::PhotonSD(name);
          SDman->AddNewDetector(aPhotonSD);
          volume->SetSensitiveDetector(aPhotonSD);
          std::cout << "Attaching sensitive Detector: " << aux.value
                    << " to Volume:  " << volume->GetName() << "\n";
          detectors_.emplace_back(volume->GetName(), aux.value);
        }
        else if (aux.value == "Tracker") {
          G4String name = volume->GetName() + "_Tracker";
          artg4tk::TrackerSD* aTrackerSD = new artg4tk::TrackerSD(name);
          SDman->AddNewDetector(aTrackerSD);
          volume->SetSensitiveDetector(aTrackerSD);
          std::cout << "Attaching sensitive Detector: " << aux.value
                    << " to Volume:  " << volume->GetName() << "\n";
          detectors_.push_back(std::make_pair(volume->GetName(), aux.value));
        }
        else if (aux.value == "SimEnergyDeposit") {
          G4String name = volume->GetName() + "_SimEnergyDeposit";
          SimEnergyDepositSD* aSimEnergyDepositSD = new SimEnergyDepositSD(name);
          SDman->AddNewDetector(aSimEnergyDepositSD);
          volume->SetSensitiveDetector(aSimEnergyDepositSD);
          std::cout << "Attaching sensitive Detector: " << aux.value
                    << " to Volume:  " << volume->GetName() << "\n";
          detectors_.emplace_back(volume->GetName(), aux.value);
        }
        else if (aux.value == "AuxDet") {
          G4String name = volume->GetName() + "_AuxDet";
          AuxDetSD* aAuxDetSD = new AuxDetSD(name);
          SDman->AddNewDetector(aAuxDetSD);
          volume->SetSensitiveDetector(aAuxDetSD);
          std::cout << "Attaching sensitive Detector: " << aux.value
                    << " to Volume:  " << volume->GetName() << "\n";
          detectors_.emplace_back(volume->GetName(), aux.value);
        }
        else if (aux.value == "HadInteraction") {
          G4String name = volume->GetName() + "_HadInteraction";
          artg4tk::HadInteractionSD* aHadInteractionSD = new artg4tk::HadInteractionSD(name);
          // NOTE: This will be done in the HadInteractionSD ctor
          // SDman->AddNewDetector(aHadInteractionSD);
          volume->SetSensitiveDetector(aHadInteractionSD);
          std::cout << "Attaching sensitive Detector: " << aux.value
                    << " to Volume:  " << volume->GetName() << "\n";
          detectors_.emplace_back(volume->GetName(), aux.value);
        }
        else if (aux.value == "HadIntAndEdepTrk") {
          G4String name = volume->GetName() + "_HadIntAndEdepTrk";
          artg4tk::HadIntAndEdepTrkSD* aHadIntAndEdepTrkSD = new artg4tk::HadIntAndEdepTrkSD(name);
          // NOTE: This will be done in the HadIntAndEdepTrkSD ctor
          // SDman->AddNewDetector(aHadIntAndEdepTrkSD);
          volume->SetSensitiveDetector(aHadIntAndEdepTrkSD);
          std::cout << "Attaching sensitive Detector: " << aux.value
                    << " to Volume:  " << volume->GetName() << "\n";
          detectors_.emplace_back(volume->GetName(), aux.value);
        }
      }
    }
    std::cout
      << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
  }
  if (dumpMP_) { G4cout << *(G4Material::GetMaterialTable()) << G4endl; }
  if (inputVolumes_ > 0) { setStepLimits(); }
  std::cout << "List SD Tree: \n";
  SDman->ListTree();
  std::cout << " Collection Capacity:  " << SDman->GetCollectionCapacity() << "\n";
  G4HCtable* hctable = SDman->GetHCtable();
  for (G4int j = 0; j < SDman->GetCollectionCapacity(); ++j) {
    std::cout << "HC Name: " << hctable->GetHCname(j) << "   SD Name:  " << hctable->GetSDname(j)
              << "\n";
  }
  std::cout << "==================================================\n";
  // Return our logical volumes.
  std::vector<G4LogicalVolume*> myLVvec;
  myLVvec.push_back(pLVStore->at(0)); // only need to return the LV of the world
  std::cout << "nr of LV ======================:  " << myLVvec.size() << "\n";

  return myLVvec;
}

std::vector<G4VPhysicalVolume*> larg4::LArG4DetectorService::doPlaceToPVs(
  std::vector<G4LogicalVolume*>)
{
  // Note we don't use our input.
  std::vector<G4VPhysicalVolume*> myPVvec;
  G4PhysicalVolumeStore* pPVStore = G4PhysicalVolumeStore::GetInstance();
  myPVvec.push_back(pPVStore->at(
    pPVStore->size() - 1)); // only need to return the PV of the world  (last entry in Volume Store)
  return myPVvec;
}

void larg4::LArG4DetectorService::setStepLimits()
{
  // -- D. Rivera : This function sets step limits for volumes provided in the configuration file
  //                and overrides the step limit (if any) set for the same volumes but from the GMDL
  //                geometry file. The GDML step limit (if provided in the gdml file) is set first
  //                and later overriden by this method if a valid volumeName,setStepLimit is provided.
  MF_LOG_WARNING("LArG4DetectorService::setStepLimits")
    << "Setting step limits from configuration"
    << " file. This will OVERRIDE redundant stepLimit(s) set in the GDML file. Note"
    << " that stepLimits are only active if enabled in the physicsListService via the"
    << " appropriate parameter.";

  std::string volumeName = "";
  G4LogicalVolume* setVol = nullptr;
  for (auto const& [name, newStepLimit] : overrideGDMLStepLimit_Map) {
    G4double previousStepLimit = 0.;

    // -- Check whether the volumeName provided corresponds to a valid volumeName in the geometry
    if (setVol = G4LogicalVolumeStore::GetInstance()->GetVolume(name, false); !setVol) {
      throw cet::exception("invalidInputVolumeName")
        << "Provided volume name : " << name << " not found!\n";
    }

    // -- get the G4LogicalVolume corresponding to the selectedVolume
    volumeName = setVol->GetName();
    MF_LOG_DEBUG("LArG4DetectorService::setStepLimits")
      << "Got logical volume with name: " << volumeName;

    G4UserLimits* fStepLimitOverride = new G4UserLimits();
    G4AutoDelete::Register(fStepLimitOverride);

    // -- check if a stepLimit for this volume has been set before:
    auto search = setGDMLVolumes_.find(volumeName);
    if (search != setGDMLVolumes_.end()) { // -- volume name found in override list
      previousStepLimit = (G4double)(search->second);
      if (newStepLimit != previousStepLimit) {
        MF_LOG_WARNING("LArG4DetectorService::setStepLimits")
          << "OVERRIDING PREVIOUSLY SET"
          << " STEPLIMIT FOR VOLUME : " << volumeName << " FROM " << previousStepLimit << " mm TO "
          << newStepLimit << " mm";
      }
      else {
        MF_LOG_WARNING("LArG4DetectorService::setStepLimits")
          << "New stepLimit matches previously"
          << " set stepLimit from the GDML file for volume : " << volumeName
          << " stepLimit : " << newStepLimit << " mm. Nothing will be changed.";
        continue;
      }
    } //--check if new steplimit differs from a previously set value

    fStepLimitOverride->SetMaxAllowedStep(newStepLimit); // -- !
    mf::LogInfo("LArG4DetectorService::setStepLimits")
      << "fStepLimitOverride:  " << newStepLimit / CLHEP::mm << " mm " << newStepLimit / CLHEP::cm
      << " cm "
      << "for volume: " << volumeName << "\n"
      << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
    setVol->SetUserLimits(fStepLimitOverride);
  } //--loop over input volumes
} //--end of setStepLimit()

std::string larg4::LArG4DetectorService::instanceName(std::string const& volume_name) const
{
  return myName() + volume_name;
}

void larg4::LArG4DetectorService::doCallArtProduces(art::ProducesCollector& collector)
{
  // Tell Art what we produce, and label the entries
  for (auto const& [volume_name, sd_name] : detectors_) {
    if (sd_name == "DRCalorimeter") {
      auto const instance = instanceName(volume_name);
      collector.produces<artg4tk::DRCalorimeterHitCollection>(instance);
      collector.produces<artg4tk::ByParticle>(instance + "Edep");
      collector.produces<artg4tk::ByParticle>(instance + "NCeren");
    }
    else if (sd_name == "Calorimeter") {
      collector.produces<artg4tk::CalorimeterHitCollection>(instanceName(volume_name));
    }
    else if (sd_name == "PhotonDetector") {
      collector.produces<artg4tk::PhotonHitCollection>(instanceName(volume_name));
    }
    else if (sd_name == "Tracker") {
      collector.produces<artg4tk::TrackerHitCollection>(instanceName(volume_name));
    }
    else if (sd_name == "SimEnergyDeposit") {
      collector.produces<sim::SimEnergyDepositCollection>(instanceName(volume_name));
    }
    else if (sd_name == "AuxDet") {
      collector.produces<sim::AuxDetHitCollection>(instanceName(volume_name));
    }
    else if (sd_name == "HadInteraction") {
      collector.produces<artg4tk::ArtG4tkVtx>(); // do NOT use product instance name (for now)
    }
    else if (sd_name == "HadIntAndEdepTrk") {
      collector.produces<artg4tk::ArtG4tkVtx>();
      collector.produces<artg4tk::TrackerHitCollection>();
    }
  }
}

void larg4::LArG4DetectorService::doFillEventWithArtHits(G4HCofThisEvent* myHC)
{
  //
  // NOTE(JVY): 1st hadronic interaction will be fetched as-is from HadInteractionSD
  //            a copy (via copy ctor) will be placed directly into art::Event
  //
  G4SDManager* sdman = G4SDManager::GetSDMpointer();
  art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;
  art::Event& e = detectorHolder->getCurrArtEvent();

  //add in PartliceListActionService ...
  art::ServiceHandle<larg4::ParticleListActionService> particleListAction;

  for (auto const& [volume_name, sd_name] : detectors_) {
    auto sd = sdman->FindSensitiveDetector(volume_name + "_" + sd_name);
    if (sd_name == "HadInteraction") {
      if (auto hisd = dynamic_cast<artg4tk::HadInteractionSD*>(sd)) {
        if (auto const& inter = hisd->Get1stInteraction(); inter.GetNumOutcoming() > 0) {
          e.put(make_product(inter));
        }
        hisd->clear();
      }
    }
    else if (sd_name == "HadIntAndEdepTrk") {
      if (auto trksd = dynamic_cast<artg4tk::HadIntAndEdepTrkSD*>(sd)) {
        if (auto const& inter = trksd->Get1stInteraction(); inter.GetNumOutcoming() > 0) {
          e.put(make_product(inter));
        }
        if (auto const& trkhits = trksd->GetEdepTrkHits(); !trkhits.empty()) {
          e.put(make_product(trkhits));
        }
        trksd->clear();
      }
    }
    else if (sd_name == "Tracker") {
      auto trsd = dynamic_cast<artg4tk::TrackerSD*>(sd);
      e.put(make_product(trsd->GetHits()), instanceName(volume_name));
    }
    else if (sd_name == "SimEnergyDeposit") {
      auto sedsd = dynamic_cast<SimEnergyDepositSD*>(sd);
      sim::SimEnergyDepositCollection hitCollection = sedsd->GetHits();
      std::map<int, int> tmap = particleListAction->GetTargetIDMap();
      if (updateSimEnergyDeposits_) {
        for (auto& hit : hitCollection) {
          hit.setTrackID(tmap[hit.TrackID()]);
        }
      }
      e.put(make_product(hitCollection), instanceName(volume_name));
    }
    else if (sd_name == "AuxDet") {
      auto auxsd = dynamic_cast<AuxDetSD*>(sd);
      sim::AuxDetHitCollection hitCollection = auxsd->GetHits();
      std::map<int, int> tmap = particleListAction->GetTargetIDMap();
      if (updateAuxDetHits_) {
        for (auto& hit : hitCollection) {
          hit.SetTrackID(tmap[hit.GetTrackID()]);
        }
      }
      e.put(make_product(auxsd->GetHits()), instanceName(volume_name));
    }
    else if (sd_name == "Calorimeter") {
      auto calsd = dynamic_cast<artg4tk::CalorimeterSD*>(sd);
      e.put(make_product(calsd->GetHits()), instanceName(volume_name));
    }
    else if (sd_name == "DRCalorimeter") {
      auto drcalsd = dynamic_cast<artg4tk::DRCalorimeterSD*>(sd);
      auto const identifier = instanceName(volume_name);
      e.put(make_product(drcalsd->GetHits()), identifier);
      e.put(make_product(drcalsd->GetEbyParticle()), identifier + "Edep");
      e.put(make_product(drcalsd->GetNCerenbyParticle()), identifier + "NCeren");
    }
    else if (sd_name == "PhotonDetector") {
      auto phsd = dynamic_cast<artg4tk::PhotonSD*>(sd);
      e.put(make_product(phsd->GetHits()), instanceName(volume_name));
    }
  }
}
