// Provides the implementation for the @ActionHolderService@ service.
// For more comprehensive documentation, see the header file ActionHolderService.hh

// Authors: Tasha Arvanitis, Adam Lyon
// Date: July 2012

// Includes
#include "larg4/Services/larg4ActionHolder_service.hh"

#include "art/Framework/Services/Registry/ServiceMacros.h"

#include "messagefacility/MessageLogger/MessageLogger.h"

#include "artg4tk/actionBase/RunActionBase.hh"
#include "artg4tk/actionBase/EventActionBase.hh"
#include "artg4tk/actionBase/TrackingActionBase.hh"
#include "artg4tk/actionBase/SteppingActionBase.hh"
#include "artg4tk/actionBase/StackingActionBase.hh"
#include "artg4tk/actionBase/PrimaryGeneratorActionBase.hh"
#include "larg4/actionBase/ParticleListActionBase.h"
#include <algorithm>

// Don't type 'std::' all the time...
using std::string;
using std::map;
using std::pair;


///////////////////
using namespace std;
/////////////////////
// Message category
static std::string msgctg = "ActionHolderService";

// Constructor doesn't do much with the passed arguments, but does initialize
// the logger for the service
larg4::larg4ActionHolderService::larg4ActionHolderService(fhicl::ParameterSet const&,
						art::ActivityRegistry&) :
  runActionsMap_(),
  eventActionsMap_(),
  trackingActionsMap_(),
  steppingActionsMap_(),
  stackingActionsMap_(),
  primaryGeneratorActionsMap_(),
  particleListActionsMap_(),
  currentArtEvent_(nullptr),
  allActionsMap_()
{}


// Register actions
template <typename A>
void larg4::larg4ActionHolderService::doRegisterAction(A * const action, 
						  std::map<std::string, A *>& actionMap) 
{
   mf::LogDebug(msgctg) << "Registering action " << action->myName();
  
  // Check if the name exists in the specific action map
  if ( 0 == actionMap.count( action->myName() ) ) {
    // Add the action!
    actionMap.insert(
                     pair<string, A *>( action->myName(), action )
                     );
    

    // Now, check whether the name exists in the overall map of all the actions
    // If so, move on (don't throw an exception, since a single action may need
    // to register in multiple maps). Otherwise, add it.
    if ( 0 == allActionsMap_.count( action->myName() ) ) {
      allActionsMap_.insert( 
        pair<string, ActionBase*>( action->myName(), dynamic_cast<ActionBase*>(action) ));
    }
  }
 
  else {
    // We already have this action in the specific action map - this is bad!
    throw cet::exception("larg4ActionHolderService")
    << "Duplicate action named " << action->myName() << ".\n";
  }

}

void larg4::larg4ActionHolderService::registerAction(RunActionBase * const action) {
  cerr<< "registering to   runActionsMap_"<<endl;
doRegisterAction(action, runActionsMap_);
  
}

void larg4::larg4ActionHolderService::registerAction(EventActionBase * const action) {
  cerr<< "registering to   eventActionsMap_"<<endl;
  doRegisterAction(action, eventActionsMap_);
}

void larg4::larg4ActionHolderService::registerAction(TrackingActionBase * const action) {
 cerr<< "registering to   trackingActionsMap_"<<endl;
  doRegisterAction(action, trackingActionsMap_);
}

void larg4::larg4ActionHolderService::registerAction(SteppingActionBase * const action) {
 cerr<< "registering to  steppingActionsMap_"<<endl;
  doRegisterAction(action, steppingActionsMap_);
}

void larg4::larg4ActionHolderService::registerAction(StackingActionBase * const action) {
 cerr<< "registering to  stackingActionsMap_"<<endl;
  doRegisterAction(action, stackingActionsMap_);
}

void larg4::larg4ActionHolderService::registerAction(PrimaryGeneratorActionBase * const action) {
 cerr<< "registering to  primaryGeneratorActionsMap_"<<endl;
  doRegisterAction(action, primaryGeneratorActionsMap_);
}
void larg4::larg4ActionHolderService::registerAction(ParticleListActionBase * const action) {
 cerr<< "registering to  particleListActions_"<<endl;
  doRegisterAction(action, particleListActionsMap_);
}
template <typename A>
A* larg4::larg4ActionHolderService::doGetAction(std::string name, std::map<std::string, A*>& actionMap) {
  
  // Make a typedef
  typedef typename std::map<std::string, A*>::const_iterator map_const_iter;
  
  // Find the action corresponding to the passed in name in the map
  map_const_iter actionIter = actionMap.find(name);
  if ( actionIter == actionMap.end() ) {
    throw cet::exception("larg4ActionHolderService") << "No action found with name "
        << name << ".\n";
  }
  return actionIter->second;
}

artg4tk::ActionBase* larg4::larg4ActionHolderService::getAction(std::string name, RunActionBase* out) {
  out = doGetAction(name, runActionsMap_);
  return out;
}

artg4tk::ActionBase* larg4::larg4ActionHolderService::getAction(std::string name, EventActionBase* out) {
  out = doGetAction(name, eventActionsMap_);
  return out;
}

artg4tk::ActionBase* larg4::larg4ActionHolderService::getAction(std::string name, TrackingActionBase* out) {
  out = doGetAction(name, trackingActionsMap_);
  return out;
}

artg4tk::ActionBase* larg4::larg4ActionHolderService::getAction(std::string name, SteppingActionBase* out) {
  out = doGetAction(name, steppingActionsMap_);
  return out;
}

artg4tk::ActionBase* larg4::larg4ActionHolderService::getAction(std::string name, StackingActionBase* out) {
  out = doGetAction(name, stackingActionsMap_);
  return out;
}

artg4tk::ActionBase* larg4::larg4ActionHolderService::getAction(std::string name, PrimaryGeneratorActionBase* out) {
  out = doGetAction(name, primaryGeneratorActionsMap_);
  return out;
}
artg4tk::ActionBase* larg4::larg4ActionHolderService::getAction(std::string name, ParticleListActionBase* out) {
  out = doGetAction(name, particleListActionsMap_);
  return out;
}
// h3. Art-specific methods
void larg4::larg4ActionHolderService::callArtProduces(art::EDProducer * prod)
{

  // Loop over the "uber" activity map and call @callArtProduces@ on each
  for ( auto entry : allActionsMap_) {
    (entry.second)->callArtProduces(prod);
  }
}

void larg4::larg4ActionHolderService::initialize() {
  for ( auto entry : allActionsMap_ ) {
    (entry.second)->initialize();
  }
}

void larg4::larg4ActionHolderService::fillEventWithArtStuff()
{

  // Loop over the "uber" activity map and call @fillEventWithArtStuff@ on each
  for ( auto entry : allActionsMap_ ) {
    (entry.second)->fillEventWithArtStuff(getCurrArtEvent());
  }
}

void larg4::larg4ActionHolderService::fillRunBeginWithArtStuff()
{
  // Loop over the activities and call @fillRunBeginWithArtStuff@ on each
  for ( auto entry : allActionsMap_ ) {
    (entry.second)->fillRunBeginWithArtStuff(getCurrArtRun());
  }
}

void larg4::larg4ActionHolderService::fillRunEndWithArtStuff()
{
  // Loop over the activities and call @fillRunEndWithArtStuff@ on each
  for ( auto entry : allActionsMap_ ) {
    (entry.second)->fillRunEndWithArtStuff(getCurrArtRun());
  }
}

// h2. Action methods

// I tried to be good and use @std::for_each@ but it got really messy very 
// quickly. Oh well. 

// h3. Run action methods
void larg4::larg4ActionHolderService::beginOfRunAction(const G4Run* theRun) {
  
  // Loop over the runActionsMap and call @beginOfRunAction@ on each
  for ( auto entry : runActionsMap_ ) {
    (entry.second)->beginOfRunAction(theRun);
  }
}

void larg4::larg4ActionHolderService::endOfRunAction(const G4Run* theRun) {

  // Loop over the runActionsMap and call @endOfRunAction@ on each
  for ( auto entry : runActionsMap_ ) {
    (entry.second)->endOfRunAction(theRun);
  }
}

// h3. Event action methods
void larg4::larg4ActionHolderService::beginOfEventAction(const G4Event* theEvent) {
  for ( auto entry : eventActionsMap_ ) {
    (entry.second)->beginOfEventAction(theEvent);
  }
}

void larg4::larg4ActionHolderService::endOfEventAction(const G4Event* theEvent) {
  for ( auto entry : eventActionsMap_ ) {
    (entry.second)->endOfEventAction(theEvent);
  }
}

// h3. Tracking action methods
void larg4::larg4ActionHolderService::preUserTrackingAction(const G4Track* theTrack) {
  for ( auto entry : trackingActionsMap_ ) {
    (entry.second)->preUserTrackingAction(theTrack);
  }
 
}

void larg4::larg4ActionHolderService::postUserTrackingAction(const G4Track* theTrack) {
  for (auto entry : trackingActionsMap_ ) {
    (entry.second)->postUserTrackingAction(theTrack);
  }
}

// h3. Stepping actions
void larg4::larg4ActionHolderService::userSteppingAction(const G4Step* theStep) {
  for ( auto entry : steppingActionsMap_ ) {
    (entry.second)->userSteppingAction(theStep);
  }
}

// h3. Stacking actions
bool larg4::larg4ActionHolderService::killNewTrack(const G4Track* newTrack) {
  
  bool killTrack = false;

  for (auto entry : stackingActionsMap_) {
    if ( (entry.second)->killNewTrack(newTrack) ) {
      killTrack = true;
      break;
    }
  }
  
  return killTrack;
}
  
// h3. Primary generator actions
void larg4::larg4ActionHolderService::generatePrimaries(G4Event* theEvent) {
  for ( auto entry : primaryGeneratorActionsMap_ ) {
    (entry.second)->generatePrimaries(theEvent);
  }
}


// Register the service with Art
using larg4::larg4ActionHolderService;

DEFINE_ART_SERVICE(larg4ActionHolderService)
