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
// GDMLDetector_service.hh:
// GDMLDetectorService is the service that constructs the Geant 4 Geometry
// as specified in a gdml file.
// To use this service, all you need to do is put it in the services section
// of the fcl configuration file, like this:
//
// <pre>
// services: {
//   ...
//   user: {
//     ...
// GDMLDetector :
//    {
//    category: "world"
//    gdmlFileName_ : "ta_target.gdml"
//    }
//   }
// }
// </pre>
// Author: Hans Wenzel (Fermilab)
// Modified: David Rivera - add ability to set step limits for different volumes
//=============================================================================


// Include guard
#ifndef GDMLDETECTOR_SERVICE_HH
#define GDMLDETECTOR_SERVICE_HH

// Includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <vector>
#include <string>
#include <unordered_map>
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4LogicalVolumeStore.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4GDMLParser.hh"

// Get the base class
#include "artg4tk/Core/DetectorBase.hh"

namespace art { class ProducesCollector; }

namespace larg4 {

  class LArG4DetectorService : public artg4tk::DetectorBase {
  private:
    std::string gdmlFileName_;              // name of the gdml file
    bool checkoverlaps_;                    // enable/disable check of overlaps
    std::vector<std::string> volumeNames_;  // list of volume names for which step limits should be set
    std::vector<float> stepLimits_;         // corresponding step limits to be set for each volume in the list of volumeNames, [mm]
    size_t inputVolumes_;                   // number of stepLimits to be set
    bool dumpMP_;                           // enable/disable dump of material properties


    // A message logger for this action
    mf::LogInfo logInfo_;

    std::vector< std::pair<std::string,std::string>>  DetectorList;
    std::map<std::string, G4double>                   overrideGDMLStepLimit_Map;
    std::unordered_map<std::string, float>            setGDMLVolumes_;         // holds all <volume, steplimit> pairs set from the GDML file
  public:
    LArG4DetectorService(fhicl::ParameterSet const&);
    ~LArG4DetectorService();

  private:

    // Private overriden methods
    virtual std::vector<G4LogicalVolume*> doBuildLVs() override;
    virtual std::vector<G4VPhysicalVolume*> doPlaceToPVs(std::vector<G4LogicalVolume*>) override;

    // -- D.R. Set the step limits for specific volumes from the configuration file
    void setStepLimits();

    // We need to add something to the art event, so we need these two methods:

    // Tell Art what we'll produce
    virtual void doCallArtProduces(art::ProducesCollector& collector) override;

    // Actually produce
    virtual void doFillEventWithArtHits(G4HCofThisEvent * hc) override;
  };
}

DECLARE_ART_SERVICE(larg4::LArG4DetectorService, LEGACY)

#endif
