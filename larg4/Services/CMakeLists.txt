# Use the magic that is @art_make@ to construct the shared object files for
# this whole directory. 
set_source_files_properties(LArG4Detector_service.cc
                            PROPERTIES COMPILE_FLAGS -I$ENV{G4INCLUDE}/Geant4 )

art_make( LIB_LIBRARIES  artg4tk_DataProducts_G4DetectorHits
                         ${G4_LIB_LIST}
			 ${XERCESCLIB})

# Install header files into the products area
install_headers() 
