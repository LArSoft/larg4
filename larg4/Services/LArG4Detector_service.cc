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
#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"
// larg4 includes:
#include "larg4/Services/LArG4Detector_service.h"

DEFINE_ART_SERVICE(larg4::LArG4DetectorService)
