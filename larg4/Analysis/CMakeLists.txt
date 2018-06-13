# Use the magic that is @art_make@ to construct the shared object files for
# this whole directory. 

art_make( MODULE_LIBRARIES  art_Framework_Services_Optional_TFileService_service
                            ${G4_LIB_LIST}
			    ${XERXESCLIB})
