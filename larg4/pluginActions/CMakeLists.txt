# Use the magic that is @art_make@ to construct the shared object files for
# this whole directory. 

art_make( SERVICE_LIBRARIES ${G4_LIB_LIST}  ${XERCESCLIB} )
install_headers()
