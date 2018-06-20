set_source_files_properties(LArG4Detector_service.cc
                            PROPERTIES COMPILE_FLAGS -I$ENV{G4INCLUDE}/Geant4 )

art_make( LIB_LIBRARIES  larg4_Sensitive_Detectors
                         ${G4_LIB_LIST}
			 ${XERCESCLIB})
# Install header files into the products area
install_headers() 
