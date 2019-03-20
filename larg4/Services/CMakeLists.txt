simple_plugin(LArG4Detector "service"
              SOURCE
              LArG4Detector_service.cc
              SimEnergyDepositSD.cc
              AuxDetSD.cc
              NOP
              ${ART_FRAMEWORK_CORE}
              ${ART_FRAMEWORK_PRINCIPAL}
              ${ART_PERSISTENCY_PROVENANCE}
              artg4tk_DataProducts_G4DetectorHits
              artg4tk_pluginDetectors_gdml
              artg4tk_services_DetectorHolder_service
              ${CLHEP}
              ${G4_LIB_LIST}
              ${MF_MESSAGELOGGER}
              ${ROOT_CORE}
              ${XERCESCLIB})

install_headers()
