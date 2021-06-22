set(CMAKE_CXX_FLAGS "${Geant4_CXX_FLAGS} -Wno-shadow -Wno-unused-parameter")
message("XercesC_LIBRARIES = ${XercesC_LIBRARIES}")
message("CLHEP_LIBRARIES = ${CLHEP_LIBRARIES}")
simple_plugin(
  LArG4Detector service
  SOURCE
    LArG4Detector_service.cc
    SimEnergyDepositSD.cc
    AuxDetSD.cc
  NOP
    art_Framework_Core
    art_Framework_Principal
    art_Framework_Services_Registry
    artg4tk_DataProducts_G4DetectorHits
    artg4tk_pluginDetectors_gdml
    artg4tk_services_DetectorHolder_service
    art_Persistency_Provenance
    Boost::filesystem
    Boost::system
    canvas
    cetlib
    cetlib_except
    ${CLHEP_LIBRARIES}
    fhiclcpp
    G4digits_hits
    G4event
    G4geometry
    G4global
    G4materials
    G4persistency
    larcorealg_Geometry
    MF_MessageLogger
    ROOT::Core
    ${XercesC_LIBRARIES}
)

install_headers()
install_source()
