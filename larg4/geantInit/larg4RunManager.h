#ifndef ArtG4_ArtG4RunManager_hh
#define ArtG4_ArtG4RunManager_hh
//
// Override the G4RunManager class so that the ArtG4 framework can drive
// the event loop.
//
// Original author Rob Kutschke (Mu2e)
//
//     In G4RunManager, the event loop is entered when the user calls BeamOn().
//     This does some initialization, calls DoEventLoop() and does some termination.
//
//     This class inherits from G4RunManager and replaces the BeamOn method
//     by breaking it into four pieces, BeamOnBeginRun(), BeamOnOneEvent(), BeamOnEndEvent()
//     and BeamOnEndRun().  The first piece is called from the beginRun() method
//     of this module; the second and third pieces are called from the process() method of
//     this module; and the last piece is called rom the endRun() method of this
//     module.
//
//     The user initialization of G4 must be complete before calling BeamOnBeginRun().
//
//     Notes:
//     1) G4RunManager is an oddly implemented singleton.  Those who access methods
//        via the instance pointer in the base will never want to access any of the
//        methods in this class.  So we do not need an instance pointer to this class.
//
//     2) ArtG4 does not plan to change G4Run numbers within a single framework job.
//
//     3) The original G4RunManager had a local variable _i_event

// Included from Geant4
#include "Geant4/G4RunManager.hh"

namespace artg4tk {
  
  class ArtG4RunManager : public G4RunManager{
    
  public:
    
    ArtG4RunManager();
    virtual ~ArtG4RunManager();
    
    // The four methods that break up BeamOn into 4 pieces.
    virtual void BeamOnBeginRun( unsigned int runNumber, const char* macroFile=0, G4int n_select=-1);
    virtual void BeamOnDoOneEvent( int eventNumber );
    virtual void BeamOnEndEvent();
    virtual void BeamOnEndRun();
    
    
    // ArtG4 specific accessors.
    G4Event const* getCurrentEvent() const { return currentEvent; }
    G4Timer const*      getG4Timer() const { return timer; }
    
    G4double   realElapsedTime() const { return realElapsed_;   }
    G4double systemElapsedTime() const { return systemElapsed_; }
    G4double   userElapsedTime() const { return userElapsed_;   }
    
  private:
    
    // Private and unimplemented to prevent copying.
    ArtG4RunManager( ArtG4RunManager const & );
    ArtG4RunManager& operator=( ArtG4RunManager const & );
    
    // A test to see if this works.
    G4Event * previousEvent_;
    
    // The variables below correspond to local variables in G4RunManager::BeamOn.
    // or G4RunManager::DoEventLoop.   They need to be member data here because
    // the BeaonOn method of the base is spread across three methods in this class.
    // These variables are reset at the start of a run and remain valid until the
    // end of the run; this emulates the behaviour in the base.
    
    // Name of the macro file to be run after the first n_select events.
    char const * macroFile_;
    
    // Number of events after which to run the macro file.  If negative
    // then never run the macro.
    G4int  n_select_;
    
    // The number of events completed so far.
    // This was called i_event in G4RunManager but I did not like the name.
    G4int  nProcessed_;
    
    // Counters for cumulative time spent processing events.
    G4double realElapsed_;
    G4double systemElapsed_;
    G4double userElapsed_;
    
    // The command that executes the macro file.
    G4String msg_;
    
  };
  
} // end namespace artg4tk.

#endif /* ArtG4_ArtG4RunManager_hh */