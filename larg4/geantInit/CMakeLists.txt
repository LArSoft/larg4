
art_make( 
          
          LIB_LIBRARIES 	             
                        ${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			art_Persistency_Common
			art_Persistency_Provenance
			art_Utilities
                        canvas
			${MF_MESSAGELOGGER}			
			${FHICLCPP}
			cetlib cetlib_except
			artg4tk_actionBase
                        ${G4_LIB_LIST}
			artg4tk_services_ActionHolder_service
			artg4tk_services_DetectorHolder_service

	 )

install_headers()



