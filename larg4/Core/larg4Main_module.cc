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
#include "artg4tk/geantInit/ArtG4DetectorConstruction.hh"
#include "artg4tk/geantInit/ArtG4RunManager.hh"

// The actions
#include "artg4tk/geantInit/ArtG4EventAction.hh"
#include "artg4tk/geantInit/ArtG4PrimaryGeneratorAction.hh"
#include "artg4tk/geantInit/ArtG4StackingAction.hh"
#include "artg4tk/geantInit/ArtG4SteppingAction.hh"
#include "artg4tk/geantInit/ArtG4TrackingAction.hh"
#include "larg4/pluginActions/MCTruthEventAction_service.h" // combined actions.
#include "larg4/pluginActions/ParticleListAction_service.h" // combined actions.

// Services
#include "art/Framework/Services/Optional/RandomNumberGenerator.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "artg4tk/services/ActionHolder_service.hh"
#include "artg4tk/services/DetectorHolder_service.hh"
#include "artg4tk/services/PhysicsListHolder_service.hh"
// art extensions
#include "lardataalg/MCDumpers/MCDumpers.h"
#include "lardataobj/Simulation/GeneratedParticleInfo.h"
#include "lardataobj/Simulation/ParticleAncestryMap.h"
#include "nug4/ParticleNavigation/ParticleList.h"
#include "nurandom/RandomUtils/NuRandomService.h"
// Geant4 includes
#include "Geant4/G4UImanager.hh"
#include "Geant4/G4UIterminal.hh"
// C++ includes
#include <atomic>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

using MCTruthCollection = std::vector<simb::MCTruth>;

namespace larg4 {

  // Define the producer
  class larg4Main : public art::EDProducer {
  public:
    explicit larg4Main(fhicl::ParameterSet const& p);
    ~larg4Main();

  private:
    void produce(art::Event& e) override;
    void beginJob() override;
    void beginRun(art::Run& r) override;
    void endRun(art::Run&) override;

    std::vector<art::Handle<MCTruthCollection>> inputCollections(art::Event const& e) const;

    // Our custom run manager
    static std::unique_ptr<artg4tk::ArtG4RunManager> runManager_;

    // G4 session and managers
    G4UIsession* session_{nullptr};
    G4UImanager* UI_{nullptr};

    // Pseudorandom engine seed (originally hardcoded to 12345),
    // obtained from the configuration file.
    // Note: the maximum seed value is 9e8, which is potentially larger
    // than a long can hold.
    long seed_;

    // Directory path(s), in colon-delimited list, in which we should look for
    // macros, or the name of an environment variable containing that path.
    // Contains only the $FW_SEARCH_PATH by default, which contains some basic
    // macro files, but can be set by config file
    std::string macroPath_;

    // And a tool to find files along that path
    // Initialized based on macroPath_.
    cet::search_path pathFinder_;

    // Name of the Geant4 macro file, if provided
    std::string g4MacroFile_;

    // Input tags used to specify which MCTruth collections to use during G4
    std::vector<art::InputTag> inputCollectionTags_;

    // Boolean to determine whether we pause execution after each event
    // If it's true, then we do. Otherwise, we pause only after all events
    // have been produced.
    // False by default, can be changed by afterEvent in FHICL
    bool pauseAfterEvent_{false};

    // Run diagnostic level (verbosity)
    int rmvlevel_;

    // When to pop up user interface
    bool uiAtBeginRun_;
    bool uiAtEndEvent_{false}; // set by afterEvent in FHiCL

    // What to do at the end of the event
    // Choices are
    //     pass -- do nothing
    //     pause -- Let user press return at the end of each event
    //     ui    -- show the UI at the end of the event
    std::string afterEvent_;

    static bool initializeDetectors_;
    static std::atomic<int> processingRun_;

    bool
      fStoreDroppedMCParticles; ///< Whether to keep a `sim::MCParticle` list of dropped particles
    std::string fDroppedParticleInstanceName; ///< Name of the dropped particle collection
  };
}

std::unique_ptr<artg4tk::ArtG4RunManager> larg4::larg4Main::runManager_{nullptr};
bool larg4::larg4Main::initializeDetectors_{true};
std::atomic<int> larg4::larg4Main::processingRun_{0};

// Constructor - set parameters
larg4::larg4Main::larg4Main(fhicl::ParameterSet const& p)
  : EDProducer{p}
  , seed_(p.get<long>("seed", -1))
  , macroPath_(p.get<std::string>("macroPath", "FW_SEARCH_PATH"))
  , pathFinder_(macroPath_)
  , g4MacroFile_(p.get<std::string>("visMacro", "larg4.mac"))
  , inputCollectionTags_{p.get<std::vector<art::InputTag>>("inputCollections", {})}
  , rmvlevel_(p.get<int>("rmvlevel", 0))
  , uiAtBeginRun_(p.get<bool>("uiAtBeginRun", false))
  , afterEvent_(p.get<std::string>("afterEvent", "pass"))
  , fStoreDroppedMCParticles(p.get<bool>("StoreDroppedMCParticles", false))
  , fDroppedParticleInstanceName(
      p.get<std::string>("DroppedParticleInstanceName", "droppedMCParticles"))
{
  produces<sim::ParticleAncestryMap>();
  produces<std::vector<simb::MCParticle>>();
  if (fStoreDroppedMCParticles) {
    produces<std::vector<simb::MCParticle>>(fDroppedParticleInstanceName);
  } //Assign product instance name
  produces<art::Assns<simb::MCTruth, simb::MCParticle, sim::GeneratedParticleInfo>>();

  // We need all of the services to run @produces@ on the data they will store. We do this
  // by retrieving the holder services.
  art::ServiceHandle<artg4tk::ActionHolderService> actionHolder;
  art::ServiceHandle<artg4tk::DetectorHolderService> detectorHolder;

  if (initializeDetectors_) {
    detectorHolder->initialize();
    // Build the detectors' logical volumes
    detectorHolder->constructAllLVs();
    initializeDetectors_ = false;
  }

  // And running @callArtProduces@ on each
  actionHolder->callArtProduces(producesCollector());
  detectorHolder->callArtProduces(producesCollector());

  // -- Check for invalid seed value
  if (seed_ > 900000000) {
    throw cet::exception("largeant:BadSeedValue")
      << "The provided largeant seed value: " << seed_ << " is invalid! Maximum seed value is 9E8.";
  }
  // Set up the random number engine.
  // -- D.R.: Use the NuRandomService engine for additional control over the seed generation policy
  (void)art::ServiceHandle<rndm::NuRandomService>()->registerAndSeedEngine(
    createEngine(0, "G4Engine"), "G4Engine", p, "seed");

  // Handle the afterEvent setting
  if (afterEvent_ == "ui") { uiAtEndEvent_ = true; }
  else if (afterEvent_ == "pause") {
    pauseAfterEvent_ = true;
  }
}

larg4::larg4Main::~larg4Main()
{
  if (runManager_) { runManager_.reset(); }
}

void larg4::larg4Main::beginJob()
{
  mf::LogDebug("Main_Run_Manager") << "In begin job";
  if (runManager_) { return; }
  runManager_.reset(new artg4tk::ArtG4RunManager);
}

void larg4::larg4Main::beginRun(art::Run& r)
{
  if (processingRun_++ != 0) { return; }

  // Get the physics list and pass it to Geant and initialize the list if necessary
  art::ServiceHandle<artg4tk::PhysicsListHolderService const> physicsListHolder;
  runManager_->SetUserInitialization(physicsListHolder->makePhysicsList());

  // Get all of the detectors and initialize them
  // Declare the detector construction to Geant
  auto detectorHolder = art::ServiceHandle<artg4tk::DetectorHolderService>().get();
  runManager_->SetUserInitialization(
    new artg4tk::ArtG4DetectorConstruction{detectorHolder->worldPhysicalVolume()});

  // Get all of the actions and initialize them
  auto actionHolder = art::ServiceHandle<artg4tk::ActionHolderService>().get();
  actionHolder->initialize();

  // Store the run in the action holder
  actionHolder->setCurrArtRun(r);

  // Declare the primary generator action to Geant
  runManager_->SetUserAction(new artg4tk::ArtG4PrimaryGeneratorAction{actionHolder});

  // Note that these actions (and ArtG4PrimaryGeneratorAction above) are all
  // generic actions that really don't do much on their own. Rather, to
  // use the power of actions, one must create action objects (derived from
  // @ActionBase@) and register them with the Art @ActionHolder@ service.
  // See @ActionBase@ and/or @ActionHolderService@ for more information.
  runManager_->SetUserAction(new artg4tk::ArtG4SteppingAction{actionHolder});
  runManager_->SetUserAction(new artg4tk::ArtG4StackingAction{actionHolder});
  runManager_->SetUserAction(new artg4tk::ArtG4EventAction{actionHolder, detectorHolder});
  runManager_->SetUserAction(new artg4tk::ArtG4TrackingAction{actionHolder});

  runManager_->Initialize();
  physicsListHolder->initializePhysicsList();

  //get the pointer to the User Interface manager
  UI_ = G4UImanager::GetUIpointer();

  // Find the macro (or try to) along the directory path.
  std::string macroLocation = "";
  bool macroWasFound = pathFinder_.find_file(g4MacroFile_, macroLocation);
  mf::LogInfo("larg4Main") << "Finding path for " << g4MacroFile_ << "...\nSearch "
                           << (macroWasFound ? "successful " : "unsuccessful ") << "and path is: \n"
                           << macroLocation;

  // Execute the macro if we were able to find it
  if (macroWasFound) {
    // Create the string containing the execution command
    mf::LogInfo("larg4Main") << "Executing macro: " << g4MacroFile_;
    std::string commandToExecute = "/control/execute ";
    commandToExecute.append(macroLocation);
    UI_->ApplyCommand(commandToExecute);
  }
  else {
    mf::LogInfo("larg4Main") << "Unable to find " << g4MacroFile_ << " in the path(s) "
                             << macroPath_;
  }

  // Open a UI if asked
  if (uiAtBeginRun_) {
    session_ = new G4UIterminal;
    session_->SessionStart();
    delete session_;
  }

  // Start the Geant run!
  runManager_->BeamOnBeginRun(r.id().run());
}

// Produce the Geant event
void larg4::larg4Main::produce(art::Event& e)
{
  // The holder services need the event
  art::ServiceHandle<artg4tk::ActionHolderService>()->setCurrArtEvent(e);
  art::ServiceHandle<artg4tk::DetectorHolderService>()->setCurrArtEvent(e);

  auto const mclists = inputCollections(e);
  art::ServiceHandle<larg4::MCTruthEventActionService>()->setInputCollections(mclists);

  art::ServiceHandle<larg4::ParticleListActionService> pla;
  pla->ParticleFilter();
  pla->setInputCollections(mclists);
  auto const pid = e.getProductID<std::vector<simb::MCParticle>>();
  pla->setPtrInfo(pid, e.productGetter(pid));

  runManager_->BeamOnDoOneEvent(e.id().event());
  runManager_->BeamOnEndEvent();

  e.put(pla->ParticleCollection());
  e.put(pla->AssnsMCTruthToMCParticle());
  e.put(pla->DroppedTracksCollection());
  if (fStoreDroppedMCParticles) { //Produce and place only if requested
    e.put(pla->DroppedParticleCollection(), fDroppedParticleInstanceName);
  }
}

void larg4::larg4Main::endRun(art::Run& r)
{
  if (--processingRun_ != 0) { return; }

  runManager_->BeamOnEndRun();
}

std::vector<art::Handle<MCTruthCollection>> larg4::larg4Main::inputCollections(
  art::Event const& e) const
{
  if (empty(inputCollectionTags_)) { return e.getMany<std::vector<simb::MCTruth>>(); }

  std::vector<art::Handle<MCTruthCollection>> result;
  for (auto const& tag : inputCollectionTags_) {
    result.push_back(e.getHandle<MCTruthCollection>(tag));
  }
  return result;
}

DEFINE_ART_MODULE(larg4::larg4Main)
