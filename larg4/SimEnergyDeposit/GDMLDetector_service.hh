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
//=============================================================================


// Include guard
#ifndef GDMLDETECTOR_SERVICE_HH
#define GDMLDETECTOR_SERVICE_HH

// Includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"

//#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Core/EDProducer.h"

#include "messagefacility/MessageLogger/MessageLogger.h"

#include <vector>
#include <string>
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4VPhysicalVolume.hh"

// Get the base class
#include "artg4tk/Core/DetectorBase.hh"


namespace artg4tk {

    class GDMLDetectorService : public artg4tk::DetectorBase {
    private:
        std::vector<std::pair<std::string,std::string> > DetectorList;
    public:
        GDMLDetectorService(fhicl::ParameterSet const&, art::ActivityRegistry&);
        virtual ~GDMLDetectorService();

    private:

        // Private overriden methods
        virtual std::vector<G4LogicalVolume*> doBuildLVs() override;
        virtual std::vector<G4VPhysicalVolume*> doPlaceToPVs(std::vector<G4LogicalVolume*>) override;
        // We need to add something to the art event, so we need these two methods:

        // Tell Art what we'll produce
        virtual void doCallArtProduces(art::EDProducer * producer) override;

        // Actually produce
        virtual void doFillEventWithArtHits(G4HCofThisEvent * hc) override;

        std::string gdmlFileName_;
        // A message logger for this action
        mf::LogInfo logInfo_;
    };
}
using artg4tk::GDMLDetectorService;
DECLARE_ART_SERVICE(GDMLDetectorService,LEGACY)
#endif

