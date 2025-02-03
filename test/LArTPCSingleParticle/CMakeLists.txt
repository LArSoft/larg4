cet_transitive_paths(LIBRARY_DIR BINARY IN_TREE)
cet_test_env_prepend(CET_PLUGIN_PATH ${TRANSITIVE_PATHS_WITH_LIBRARY_DIR})

cet_transitive_paths(GDML_DIR BINARY IN_TREE)
cet_test_env_prepend(FW_SEARCH_PATH ${TRANSITIVE_PATHS_WITH_GDML_DIR})

cet_test(LArTPCSingleParticle_test HANDBUILT
  TEST_EXEC lar
  TEST_ARGS --rethrow-all -c test_singleparticlelarg4.fcl
  DATAFILES  test_singleparticlelarg4.fcl
  )
