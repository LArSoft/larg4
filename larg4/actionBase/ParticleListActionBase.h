// This file is the header for the @ParticleListActionBase@ class.

// Include guard
#ifndef PARTICLELIST_ACTION_BASE_H
#define PARTICLELIST_ACTION_BASE_H

#include <string>
#include <iostream>

#include "artg4tk/actionBase/ActionBase.hh"

#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "larg4/Services/larg4ActionHolder_service.h"

// Declarations of types we use as input parameters
class G4Step;
class G4Track;
class G4Event;

// Everything goes in the larg4 namespace
namespace larg4 {

  class ParticleListActionBase : public artg4tk::ActionBase {
  public:
    // Constructor. The derived class must call this constructor. It takes a 
    // single string for the name of the action object.
    ParticleListActionBase(std::string myName)
      : ActionBase( myName )
    {
      art::ServiceHandle<larg4::larg4ActionHolderService> actionHolder;
      actionHolder->registerAction(this);
    }


    // Destructor
    virtual ~ParticleListActionBase();

    // h3. The interesting methods. 
    // All of these are defined to do nothing by default. Users can override 
    // them if desired, and if they're not overloaded, they do nothing.

    // Called at the end of each step
    virtual void userSteppingAction(const G4Step *) {}
       // Called before a track is simulated
    virtual void preUserTrackingAction(const G4Track *) {}
    
    // Called when a track is stopped
    virtual void postUserTrackingAction(const G4Track *) {}

    // Called at the beginning of each event (after creation of primaries)
    virtual void beginOfEventAction(const G4Event *) {}

    // Called at the end of each event
    virtual void endOfEventAction(const G4Event *) {}

  };
}


#endif // STEPPING_ACTION_BASE_HH
