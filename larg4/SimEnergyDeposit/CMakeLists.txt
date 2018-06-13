# Use the magic that is @art_make@ to construct the shared object files for
# this whole directory. 
#set_source_files_properties(GDMLDetector_service.cc PROPERTIES COMPILE_FLAGS -I/data/upsupdartg4tk/externals/geant4/v4_10_1_p02/Linux64bit+2.6-2.12-e7-prof/include/Geant4)
#set_source_files_properties(GDMLDetector_service.cc ColorReader.cc   PROPERTIES COMPILE_FLAGS -I${GEANT4_INC}/Geant4)
#set_source_files_properties(GDMLDetector_service.cc ColorReader.cc PROPERTIES COMPILE_FLAGS -I$ENV{G4INCLUDE}/Geant4 ${LARCOREOBJ_INC}/larcoreobj)
set_source_files_properties(GDMLDetector_service.cc
                            PROPERTIES COMPILE_FLAGS -I$ENV{G4INCLUDE}/Geant4 )

art_make( LIB_LIBRARIES  artg4tk_DataProducts_G4DetectorHits
                         ${G4_LIB_LIST}
			 ${XERCESCLIB})

# Install header files into the products area
install_headers() 
