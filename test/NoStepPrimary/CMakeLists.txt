cet_build_plugin(PrimaryTestInput art::EDProducer NO_INSTALL
  LIBRARIES PRIVATE nusimdata::SimulationBase ROOT::Physics
)
cet_build_plugin(NoStepPrimaryCheck art::EDProducer NO_INSTALL
  LIBRARIES PRIVATE larg4::pluginActions_ParticleListAction_service
  nug4::G4Base Geant4::G4processes Geant4::G4particles Geant4::G4track CLHEP::Vector
)
cet_transitive_paths(LIBRARY_DIR BINARY IN_TREE)
cet_test_env_prepend(CET_PLUGIN_PATH ${TRANSITIVE_PATHS_WITH_LIBRARY_DIR})
cet_test(NoStepPrimary_test HANDBUILT
  TEST_EXEC lar TEST_ARGS --rethrow-all -c no_step_primary.fcl
  DATAFILES no_step_primary.fcl
)
cet_test(NoStepPrimaryFullTrajectory_test HANDBUILT
  TEST_EXEC lar TEST_ARGS --rethrow-all -c no_step_primary_full.fcl
  DATAFILES no_step_primary.fcl no_step_primary_full.fcl
)
