set(CMAKE_CXX_FLAGS "${Geant4_CXX_FLAGS} -Wno-shadow -Wno-unused-parameter")
simple_plugin(
  MCTruthEventAction service
NOP
  art_Framework_Principal
  art_Framework_Services_Registry
  artg4tk_actionBase
  artg4tk_services_ActionHolder_service
  art_Persistency_Provenance
  Boost::filesystem
  Boost::system
  canvas
  cetlib_except
  ${CLHEP_LIBRARIES}
  fhiclcpp
  G4global
  G4particles
  MF_MessageLogger
  nusimdata_SimulationBase
  nug4_G4Base
  ROOT::Core
  ROOT::Physics
SOURCE
  MCTruthEventAction_service.cc
)

simple_plugin(
  ParticleListAction service
NOP
  art_Framework_Services_Registry
  canvas
  fhiclcpp
  cetlib_except
  Boost::filesystem
  Boost::system
  art_Framework_Core
  art_Framework_Principal
  artg4tk_actionBase
  artg4tk_services_ActionHolder_service
  art_Persistency_Provenance
  ${CLHEP_LIBRARIES}
  G4particles
  MF_MessageLogger
  nusimdata_SimulationBase
  nug4_G4Base
  nug4_ParticleNavigation
  ROOT::Core
  ROOT::Physics
SOURCE
  ParticleListAction_service.cxx
)

install_headers()
install_source()
