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
#ifndef LARG4DETECTOR_SERVICE_HH
#define LARG4DETECTOR_SERVICE_HH

#include "artg4tk/Core/DetectorBase.hh"

#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"

namespace art {
  class ProducesCollector;
}

namespace fhicl {
  class ParameterSet;
}

class G4HCofThisEvent;
class G4LogicalVolume;
class G4VPhysicalVolume;

#include "Geant4/G4Types.hh"

#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace larg4 {

  class LArG4DetectorService : public artg4tk::DetectorBase {
  public:
    explicit LArG4DetectorService(fhicl::ParameterSet const&);

  private:
    std::vector<G4LogicalVolume*> doBuildLVs() override;
    std::vector<G4VPhysicalVolume*> doPlaceToPVs(std::vector<G4LogicalVolume*>) override;

    // -- D.R. Set the step limits for specific volumes from the configuration file
    void setStepLimits();

    // We need to add something to the art event, so we need these two methods:

    std::string instanceName(std::string const&) const;

    // Tell Art what we'll produce
    void doCallArtProduces(art::ProducesCollector& collector) override;

    // Actually produce
    void doFillEventWithArtHits(G4HCofThisEvent* hc) override;

    std::string gdmlFileName_; // name of the gdml file
    bool checkOverlaps_;       // enable/disable check of overlaps
    bool
      updateSimEnergyDeposits_; // enable/disable change of TrackID  for Tracks where no MCParticle was created
    bool
      updateAuxDetHits_;        // enable/disable change of TrackID  for Tracks where no MCParticle was created
    std::vector<std::string>
      volumeNames_; // list of volume names for which step limits should be set
    std::vector<float>
      stepLimits_; // corresponding step limits to be set for each volume in the list of volumeNames, [mm]
    size_t inputVolumes_; // number of stepLimits to be set
    bool dumpMP_;         // enable/disable dump of material properties

    std::vector<std::pair<std::string, std::string>> detectors_{};
    std::map<std::string, G4double> overrideGDMLStepLimit_Map{};
    std::unordered_map<std::string, float>
      setGDMLVolumes_{}; // holds all <volume, steplimit> pairs set from the GDML file
  };
}

DECLARE_ART_SERVICE(larg4::LArG4DetectorService, LEGACY)

#endif // LARG4DETECTOR_SERVICE_HH
