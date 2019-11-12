// larg4Main is the main producer module for Geant.

// larg4Main_module.cc replicates many GEANT programs' @main()@ driver. It
// creates and initializes the run manager, controls the beginning and end of
// events, and controls visualization.

#include "nusimdata/SimulationBase/MCParticle.h"
#include "nusimdata/SimulationBase/MCTruth.h"


// Art includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"

// Local includes (like actions)
#include "artg4tk/geantInit/ArtG4RunManager.hh"
#include "artg4tk/geantInit/ArtG4DetectorConstruction.hh"


// The actions
#include "artg4tk/geantInit/ArtG4EventAction.hh"
#include "artg4tk/geantInit/ArtG4PrimaryGeneratorAction.hh"
#include "artg4tk/geantInit/ArtG4RunAction.hh"
#include "artg4tk/geantInit/ArtG4SteppingAction.hh"
#include "artg4tk/geantInit/ArtG4StackingAction.hh"
#include "artg4tk/geantInit/ArtG4TrackingAction.hh"
#include "larg4/pluginActions/ParticleListAction_service.h" // combined actions.
// Services
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "artg4tk/services/ActionHolder_service.hh"
#include "artg4tk/services/DetectorHolder_service.hh"

#include "artg4tk/services/PhysicsListHolder_service.hh"
#include "art/Framework/Services/Optional/RandomNumberGenerator.h"

#include "nug4/ParticleNavigation/ParticleList.h"
#include "lardataobj/Simulation/GeneratedParticleInfo.h"

#include "lardataalg/MCDumpers/MCDumpers.h"

// G4 includes
#ifdef G4VIS_USE
#include "Geant4/G4VisExecutive.hh"
#endif

#include "Geant4/G4UImanager.hh"
#include "Geant4/G4UIterminal.hh"

using namespace std;

namespace larg4 {

  // Define the producer
  class larg4Main : public art::EDProducer {
  public:

    explicit larg4Main(fhicl::ParameterSet const & p);

  private:
    virtual void produce(art::Event & e) override;
    virtual void beginJob() override;
    virtual void beginRun(art::Run &r) override;
    virtual void endRun(art::Run &) override;

    // Our custom run manager
    unique_ptr<artg4tk::ArtG4RunManager> runManager_;

    // G4 session and managers
    G4UIsession* session_;
    G4UImanager* UI_;

    // Visualization manager if necessary
#ifdef G4VIS_USE
    G4VisManager* visManager_;
#endif

	// Pseudorandom engine seed (originally hardcoded to 12345),
	// obtained from the configuration file.
	// Note: the maximum seed value is 9e8, which is potentially larger
	// than a long can hold.
	long seed_;

    // Determine whether we should use visualization
    // False by default, can be set by config file
    bool enableVisualization_;

    // Directory path(s), in colon-delimited list, in which we should look for
    // macros, or the name of an environment variable containing that path.
    // Contains only the current directory ('.') by default, but can be
    // set by config file
    string macroPath_;

    // And a tool to find files along that path
    // Initialized based on macroPath_.
    cet::search_path pathFinder_;

    // Name of a macro file for visualization
    // 'vis.mac' by default, and can be customized by config file.
    string visMacro_;

    // Boolean to determine whether we pause execution after each event
    // If it's true, then we do. Otherwise, we pause only after all events
    // have been produced.
    // False by default, can be changed by afterEvent in FHICL
    bool pauseAfterEvent_;

	// Boolean to determine whether we're in "visualize only certain
	// events" mode. If so, we pause and show the visualization only after
	// the given events. Turning this on only works if visualization is
	// also enabled, and it will pause, pass, or bring up a UI at the end
	// of the given events, as specified by afterEvent.
	bool visSpecificEvents_;

	// If we're in "visualize only certain events" mode, this vector
	// contains the events for which the visualization should be displayed.
	// This is a map because determining whether an event is in there is
	// O(log(n)), rather than O(n) for a vector, and find(...) is a heck of
	// a lot more convenient than looping over the vector.
	std::map<int, bool> eventsToDisplay_;

    // Run diagnostic level (verbosity)
    int rmvlevel_;

    // When to pop up user interface
    bool uiAtBeginRun_;
    bool uiAtEndEvent_; // set by afterEvent in FHICL

    // What to do at the end of the event
    // Choices are
    //     pass -- do nothing
    //     pause -- Let user press return at the end of each event
    //     ui    -- show the UI at the end of the event
    std::string afterEvent_;

    // Message logger
    mf::LogInfo logInfo_;
    //    bool fSparsifyTrajectories; ///< Sparsify MCParticle Trajectories
    //larg4::ParticleListAction* fparticleListAction; ///< Geant4 user action to particle information.

  };
}

// Constructor - set parameters
larg4::larg4Main::larg4Main(fhicl::ParameterSet const & p)
  : EDProducer{p},
    runManager_(),
    session_(0),
    UI_(0),
	seed_(p.get<long>("seed", -1)),
    enableVisualization_( p.get<bool>("enableVisualization",false)),
    macroPath_( p.get<std::string>("macroPath",".")),
    pathFinder_( macroPath_),
    visMacro_( p.get<std::string>("visMacro", "vis.mac")),
    pauseAfterEvent_(false),
	visSpecificEvents_(p.get<bool>("visualizeSpecificEvents",false)),
	eventsToDisplay_(),
    rmvlevel_( p.get<int>("rmvlevel",0)),
    uiAtBeginRun_( p.get<bool>("uiAtBeginRun", false)),
    uiAtEndEvent_(false),
    afterEvent_( p.get<std::string>("afterEvent", "pass")),
  logInfo_("larg4Main")
//  fSparsifyTrajectories(false),
//  fparticleListAction(0)
  //  pla_("ParticleListAction")

{
  produces< std::vector<simb::MCParticle> >();
  produces< art::Assns<simb::MCTruth, simb::MCParticle> >();
	// If we're in "visualize specific events" mode (essentially only pause
	// after given events), then extract the list of events we need to
	// pause for. They are placed in a map because it is more efficient to
	// determine whether a given entry is present in the map than a vector.
	if (visSpecificEvents_) {
		std::vector<int> eventsToDisplayVec =
			p.get<vector<int>>("eventsToDisplay");
		for (size_t i = 0; i < eventsToDisplayVec.size(); i++) {
			eventsToDisplay_[eventsToDisplayVec[i]] = true;
		}
		// Would be nice to have error checking here, but for now, if you
		// do something silly, it'll probably just crash.
	}

  // We need all of the services to run @produces@ on the data they will store. We do this
  // by retrieving the holder services.
  art::ServiceHandle<ActionHolderService> actionHolder;
  art::ServiceHandle<DetectorHolderService> detectorHolder;

    detectorHolder->initialize();
    // Build the detectors' logical volumes
    detectorHolder -> constructAllLVs();
    // And running @callArtProduces@ on each
    actionHolder -> callArtProduces(producesCollector());
    detectorHolder -> callArtProduces(producesCollector());

    //    ((artg4tk::SteppingActionBase*)&*pla)-> callArtProduces(this);
    // ((artg4tk::EventActionBase*)&*pla) -> callArtProduces(this);
    // ((artg4tk::TrackingActionBase*)&*pla) -> callArtProduces(this);
  // Set up the random number engine.
  // See the documentation in RandomNumberHeader.h for
  // how this works. Note that @createEngine@ is a member function
  // of our base class (actually, a couple of base classes deep!).
  // Note that the name @G4Engine@ is special.
  if (seed_ == -1) {
	  // Construct seed from time and pid. (default behavior if
	  // no seed is provided by the fcl file)
	  // Note: According to Kevin Lynch, the two lines below are not portable.
	  seed_ = time(0) + getpid();
	  seed_ = ((seed_ & 0xFFFF0000) >> 16) | ((seed_ & 0x0000FFFF) << 16); //exchange upper and lower word
	  seed_ = seed_ % 900000000; // ensure the seed is in the correct range for createEngine
  }
  createEngine( seed_, "G4Engine");

  // Handle the afterEvent setting
  if ( afterEvent_ == "ui" ) {
    uiAtEndEvent_ = true;
  }
  else if ( afterEvent_ == "pause" ) {
    pauseAfterEvent_ = true;
  }
}

// At begin job
void larg4::larg4Main::beginJob()
{

  // Set up run manager
  mf::LogDebug("Main_Run_Manager") << "In begin job";
  runManager_.reset( new artg4tk::ArtG4RunManager );
}

// At begin run
void larg4::larg4Main::beginRun(art::Run & r)
{
  // Get the physics list and pass it to Geant and initialize the list if necessary
  art::ServiceHandle<PhysicsListHolderService const> physicsListHolder;
  runManager_->SetUserInitialization( physicsListHolder->makePhysicsList() );

  // Get all of the detectors and initialize them
  // Declare the detector construction to Geant
  runManager_->SetUserInitialization(new artg4tk::ArtG4DetectorConstruction);

  // Get all of the actions and initialize them
  art::ServiceHandle<ActionHolderService> actionHolder;


  actionHolder->initialize();

  // Store the run in the action holder
  actionHolder->setCurrArtRun(r);

  // Declare the primary generator action to Geant
  runManager_->SetUserAction(new artg4tk::ArtG4PrimaryGeneratorAction);

  // Note that these actions (and ArtG4PrimaryGeneratorAction above) are all
  // generic actions that really don't do much on their own. Rather, to
  // use the power of actions, one must create action objects (derived from
  // @ActionBase@) and register them with the Art @ActionHolder@ service.
  // See @ActionBase@ and/or @ActionHolderService@ for more information.
  runManager_ -> SetUserAction(new artg4tk::ArtG4SteppingAction);
  runManager_ -> SetUserAction(new artg4tk::ArtG4StackingAction);
  runManager_ -> SetUserAction(new artg4tk::ArtG4EventAction);
  runManager_ -> SetUserAction(new artg4tk::ArtG4TrackingAction);
  runManager_ -> SetUserAction(new artg4tk::ArtG4RunAction);

  runManager_->Initialize();
  physicsListHolder->initializePhysicsList();

  //get the pointer to the User Interface manager
  UI_ = G4UImanager::GetUIpointer();

  // Set up visualization if it's allowed by current values of env. variables
#ifdef G4VIS_USE

  // Turn on visualization if necessary
  if (enableVisualization_) {
    logInfo_ << "Initializing visualization\n" << endl;

    // Create and initialize the visualization manager
    visManager_ = new G4VisExecutive;
    visManager_->Initialize();

    // Find the macro (or try to) along the directory path.
    string macroLocation = "";
    bool macroWasFound = pathFinder_.find_file(visMacro_, macroLocation);
    logInfo_ << "Finding path for " << visMacro_ << "...\nSearch "
	     << (macroWasFound ? "successful " : "unsuccessful ")
	     << "and path is: \n" << macroLocation << "\n" << endl;

    // Execute the macro if we were able to find it
    if (macroWasFound) {
      // Create the string containing the execution command
      logInfo_ << "Executing macro: " << visMacro_ << "\n" << endl;
      string commandToExecute = "/control/execute ";
      commandToExecute.append(macroLocation);
      UI_->ApplyCommand(commandToExecute);

    } else {
      // If it wasn't found...
      // Leave a message for the user ...
      logInfo_ << "Unable to find " << visMacro_ << " in the path(s) "
	       << macroPath_ << endl;
      // ... and disable visualization for the future
      enableVisualization_ = false;
      delete visManager_;

    } // if the macro was found

  } // if visualization was enabled

#endif // G4VIS_USE

  // Open a UI if asked
  if ( uiAtBeginRun_ ) {
    session_ = new G4UIterminal;
    session_->SessionStart();
    delete session_;
  }

  // Start the Geant run!
  runManager_ -> BeamOnBeginRun(r.id().run());
}

// Produce the Geant event
void larg4::larg4Main::produce(art::Event & e)
{
  // The holder services need the event
  art::ServiceHandle<ActionHolderService> actionHolder;
  art::ServiceHandle<DetectorHolderService> detectorHolder;
  art::ServiceHandle<ParticleListActionService> pla;
  actionHolder -> setCurrArtEvent(e);
  detectorHolder -> setCurrArtEvent(e);
  pla -> setCurrArtEvent(e);
  pla -> setProductID( e.getProductID<std::vector<simb::MCParticle>>());

  // Begin event
  runManager_ -> BeamOnDoOneEvent(e.id().event());

  //  logInfo_ << "Producing event " << e.id().event() << "\n" << endl;

  // Done with the event
  runManager_ -> BeamOnEndEvent();

  auto  &partCol=pla->GetParticleCollection();
  auto &tpassn = pla->GetAssnsMCTruthToMCParticle();
  e.put(std::move(partCol));
  e.put(std::move(tpassn));

  /*
  unsigned int nGeneratedParticles = 0;
  art::ServiceHandle<larg4::ParticleListActionService const> h;
  sim::ParticleList particleList =h->YieldList();

  std::unique_ptr< std::vector<simb::MCParticle> >               partCol                    (new std::vector<simb::MCParticle  >);

  auto tpassn = std::make_unique<art::Assns<simb::MCTruth, simb::MCParticle, sim::GeneratedParticleInfo>>();
        for(auto const& partPair: particleList) {
          simb::MCParticle& p = *(partPair.second);
          ++nGeneratedParticles;

          // if the particle has been marked as dropped, we don't save it
          // (as of LArSoft ~v5.6 this does not ever happen because
          // ParticleListAction has already taken care of deleting them)
          //if (ParticleListAction::isDropped(&p)) continue;

          sim::GeneratedParticleInfo const truthInfo{
            fparticleListAction->GetPrimaryTruthIndex(p.TrackId())
            };
          if (!truthInfo.hasGeneratedParticleIndex() && (p.Mother() == 0)) {
            // this means it's primary but with no information; logic error!!
            art::Exception error(art::errors::LogicError);
            error << "Failed to match primary particle:\n";
            sim::dump::DumpMCParticle(error, p, "  ");
	    //            error << "\nwith particles from the truth record '"
            //  << mclistHandle.provenance()->inputTag() << "':\n";
	    //            sim::dump::DumpMCTruth(error, *mct, 2U, "  "); // 2 points per line
            //error << "\n";
            throw error;
          }

	  if(fSparsifyTrajectories) p.SparsifyTrajectory();

          partCol->push_back(std::move(p));

          tpassn->addSingle(mct, makeMCPartPtr(partCol->size() - 1), truthInfo);

        } // for(particleList)

  */


#ifdef G4VIS_USE
  // If visualization is enabled, and we want to pause after each event, do
  // the pausing.
  if (enableVisualization_) {

    // Flush the visualization
    //UI_->ApplyCommand("/tracking/storeTrajectory 1");
    UI_->ApplyCommand("/vis/viewer/flush");

    // Only pause or bring up a UI if
	//  a) we're doing so for all events (!visSpecificEvents_)
	//  b) the current event was specified as one to pause for
	//     (eventsToDisplay_.count(e.id().event()) > 0)
	if ( !visSpecificEvents_ || eventsToDisplay_.count(e.id().event()) > 0 ) {
		if ( uiAtEndEvent_ ) {
			session_ = new G4UIterminal;
			session_->SessionStart();
			delete session_;
		}

		if ( pauseAfterEvent_) {
			// Use cout so that it is printed to console immediately.
			// logInfo_ prints everything at once, so if we used that, we
			// would find out that we should press ENTER to continue only
			// *after* we'd actually done so!
			cout << "Event: " << e.id().event()
				<< ", pausing so you can appreciate visualization. "
				<< "Hit ENTER to continue." << std::endl;
			std::cin.ignore();
		}
	}

  }
#endif

}

// At end run
void larg4::larg4Main::endRun(art::Run & r)
{
  art::ServiceHandle<ActionHolderService> actionHolder;
  actionHolder->setCurrArtRun(r);

  runManager_ -> BeamOnEndRun();

  //  visualization stuff
#ifdef G4VIS_USE
  if ( enableVisualization_ ) {
    // Delete ui
    delete visManager_;
  }
#endif
}

using larg4::larg4Main;
DEFINE_ART_MODULE(larg4Main)
