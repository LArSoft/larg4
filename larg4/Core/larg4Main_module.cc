// larg4Main is the main producer module for Geant.

// larg4Main_module.cc replicates many GEANT programs' @main()@ driver. It
// creates and initializes the run manager, controls the beginning and end of
// events.

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

// art extensions
#include "art/Framework/Services/Optional/RandomNumberGenerator.h"
#include "nurandom/RandomUtils/NuRandomService.h"

#include "nug4/ParticleNavigation/ParticleList.h"
#include "lardataobj/Simulation/GeneratedParticleInfo.h"

#include "lardataalg/MCDumpers/MCDumpers.h"



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



    // Pseudorandom engine seed (originally hardcoded to 12345),
    // obtained from the configuration file.
    // Note: the maximum seed value is 9e8, which is potentially larger
    // than a long can hold.
    long seed_;

    // Directory path(s), in colon-delimited list, in which we should look for
    // macros, or the name of an environment variable containing that path.
    // Contains only the current directory ('.') by default, but can be
    // set by config file
    string macroPath_;

    // And a tool to find files along that path
    // Initialized based on macroPath_.
    cet::search_path pathFinder_;


    // Boolean to determine whether we pause execution after each event
    // If it's true, then we do. Otherwise, we pause only after all events
    // have been produced.
    // False by default, can be changed by afterEvent in FHICL
    bool pauseAfterEvent_;

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
  macroPath_( p.get<std::string>("macroPath",".")),
  pathFinder_( macroPath_),
  pauseAfterEvent_(false),
  rmvlevel_( p.get<int>("rmvlevel",0)),
  uiAtBeginRun_( p.get<bool>("uiAtBeginRun", false)),
  uiAtEndEvent_(false),
  afterEvent_( p.get<std::string>("afterEvent", "pass")),
  logInfo_("larg4Main")
{
  produces< std::vector<simb::MCParticle> >();
  produces< art::Assns<simb::MCTruth, simb::MCParticle, sim::GeneratedParticleInfo> >();

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

  // ((artg4tk::SteppingActionBase*)&*pla)-> callArtProduces(this);
  // ((artg4tk::EventActionBase*)&*pla) -> callArtProduces(this);
  // ((artg4tk::TrackingActionBase*)&*pla) -> callArtProduces(this);

  // -- Check for invalid seed value
  if (seed_ > 900000000) {
    //mf::LogError("SeedCheck") << "Bad seed provided, max seed value is 9E8.
    throw cet::exception("largeant:BadSeedValue")
          << "The provided largeant seed value: " << seed_
          << " is invalid! Maximum seed value is 9E8.";
  }
  // Set up the random number engine.
  // -- D.R.: Use the NuRandomService engine for additional control over the seed generation policy
  (void)art::ServiceHandle<rndm::NuRandomService>()->createEngine(*this,"G4Engine",p,"seed");

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
}

// At end run
void larg4::larg4Main::endRun(art::Run & r)
{
  art::ServiceHandle<ActionHolderService> actionHolder;
  actionHolder->setCurrArtRun(r);
  runManager_ -> BeamOnEndRun();
}

using larg4::larg4Main;
DEFINE_ART_MODULE(larg4Main)
