# Core directory

# Contains the action objects, run manager, physics list, and generic
# detector construction class.

simple_plugin(larg4Main "module"
              artg4tk_geantInit
              larg4_pluginActions_ParticleListAction_service
              artg4tk_pluginDetectors_gdml
              artg4tk_services_PhysicsListHolder_service
              artg4tk_services_DetectorHolder_service
              ${XERCESCLIB}
              ${G4_LIB_LIST})
