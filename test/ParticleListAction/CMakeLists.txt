cet_build_plugin(AncestryTestInput art::EDProducer NO_INSTALL
  LIBRARIES PRIVATE
  nusimdata::SimulationBase
)

cet_build_plugin(LowEnergyAncestryCheck art::EDAnalyzer NO_INSTALL
  LIBRARIES PRIVATE
  larg4::pluginActions_ParticleListAction_service
  Geant4::G4processes
  Geant4::G4particles
  Geant4::G4track
  CLHEP::Vector
)

cet_transitive_paths(LIBRARY_DIR BINARY IN_TREE)
cet_test_env_prepend(CET_PLUGIN_PATH ${TRANSITIVE_PATHS_WITH_LIBRARY_DIR})

cet_test(LowEnergyAncestry_test HANDBUILT
  TEST_EXEC lar
  TEST_ARGS --rethrow-all -c low_energy_ancestry.fcl
  DATAFILES low_energy_ancestry.fcl
)

cet_test(LowEnergyAncestryKeepEM_test HANDBUILT
  TEST_EXEC lar
  TEST_ARGS --rethrow-all -c low_energy_ancestry_keep_em.fcl
  DATAFILES low_energy_ancestry.fcl low_energy_ancestry_keep_em.fcl
)
