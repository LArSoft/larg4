# CMakeLists.txt file for gdml detector description files

# install all *.mac files in this directory
file(GLOB gdml_files *.gdml)
file (GLOB xsd_files *.xsd)
install( FILES ${gdml_files}  ${xsd_files}
         DESTINATION ${product}/${version}/${PROJECT_NAME}/gdml )

# Also install in the build area
foreach(aFile ${gdml_files})
  get_filename_component( basename ${aFile} NAME )
  configure_file( ${aFile} ${CMAKE_BINARY_DIR}/gdml/${basename} COPYONLY )
endforeach(aFile)
foreach(aFile ${xsd_files})
  get_filename_component( basename ${aFile} NAME )
  configure_file( ${aFile} ${CMAKE_BINARY_DIR}/gdml/${basename} COPYONLY )
endforeach(aFile)
