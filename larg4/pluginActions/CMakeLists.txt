# Use the magic that is @art_make@ to construct the shared object files for
# this whole directory. 
simple_plugin(
  ParticleListAction service     nutools_G4Base
  SOURCE
    ParticleListAction_service.cxx

)


art_make( SERVICE_LIBRARIES ${G4_LIB_LIST}  ${XERCESCLIB} nutools_G4Base 
         LIB_LIBRARIES 
                        larsim_PhotonPropagation
                        larsim_PhotonPropagation_PhotonVisibilityService_service
#                        lardata_Utilities
			larsim_Simulation nutools_ParticleNavigation lardataobj_Simulation
#                        larcorealg_Geometry
#                        larcore_Geometry_Geometry_service
                        nusimdata_SimulationBase
			nutools_RandomUtils_NuRandomService_service
                        nutools_G4Base
                        ${ART_FRAMEWORK_CORE}
                        ${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_RANDOMNUMBERGENERATOR_SERVICE}
                        art_Persistency_Common
                        art_Persistency_Provenance
                        art_Utilities
                        canvas
                        ${MF_MESSAGELOGGER}
                        
                        ${FHICLCPP}
                        cetlib cetlib_except
                        ${ROOT_GENVECTOR}
                        ${ROOT_EG}
                        ${ROOT_TREEPLAYER} 
                        ${ROOT_FFTW}
                        ${ROOT_REFLEX}
                        ${ROOT_EGPYTHIA6}
                        ${ROOT_GUI}
                        ${ROOT_BASIC_LIB_LIST}
                        ${ROOT_GEOM}
                        ${CLHEP}
                        ${G4_LIB_LIST}
           MODULE_LIBRARIES larsim_LArG4
                        larsim_MCCheater_ParticleInventoryService_service
                        larsim_MCDumpers
                        larsim_Simulation nutools_ParticleNavigation lardataobj_Simulation
                        larcorealg_Geometry
 #                       larcore_Geometry_Geometry_service
                        nutools_RandomUtils_NuRandomService_service
                        nusimdata_SimulationBase
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
			canvas
                        ${MF_MESSAGELOGGER}
                        
                        ${FHICLCPP}
                        cetlib cetlib_except
                        ${ROOT_GENVECTOR}
                        ${ROOT_BASIC_LIB_LIST}
                        nutools_G4Base
                        CLHEP
                        ${G4_LIB_LIST}




)
install_headers()
