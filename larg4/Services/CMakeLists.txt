set_source_files_properties(LArG4Detector_service.cc PROPERTIES COMPILE_FLAGS -I$ENV{G4INCLUDE}/Geant4 )

simple_plugin(
  LArG4Detector service
  SOURCE
    LArG4Detector_service.cc
    SimEnergyDepositSD.cc
  NOP
    ${G4_LIB_LIST}
    ${XERCESCLIB}
)

install_headers() 
