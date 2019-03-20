simple_plugin(ParticleListAction "service"
              nusimdata_SimulationBase
              nutools_G4Base
              nutools_ParticleNavigation
              artg4tk_actionBase
              artg4tk_services_ActionHolder_service
              ${CLHEP}
              ${ART_FRAMEWORK_CORE}
              ${ART_FRAMEWORK_PRINCIPAL}
              ${ART_PERSISTENCY_PROVENANCE}
              ${G4_LIB_LIST}
              ${MF_MESSAGELOGGER}
              ${ROOT_CORE}
              ${ROOT_PHYSICS}
              SOURCE ParticleListAction_service.cxx)

simple_plugin(MCTruthEventAction "service"
              artg4tk_actionBase
              artg4tk_services_ActionHolder_service
              ${ART_FRAMEWORK_PRINCIPAL}
              ${ART_PERSISTENCY_PROVENANCE}
              ${CLHEP}
              ${G4_LIB_LIST}
              ${MF_MESSAGELOGGER}
              ${XERCESCLIB}
              ${ROOT_CORE}
              ${ROOT_PHYSICS}
              nusimdata_SimulationBase
             nutools_G4Base)

install_headers()
