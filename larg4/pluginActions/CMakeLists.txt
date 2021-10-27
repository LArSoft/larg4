simple_plugin(
  MCTruthEventAction service
NOP
  art::Framework_Principal
  art::Framework_Services_Registry
  artg4tk::actionBase
  artg4tk::services_ActionHolder_service
  art::Persistency_Provenance
  canvas::canvas
  cetlib_except::cetlib_except
  CLHEP::CLHEP
  fhiclcpp
  ${G4GLOBAL}
  ${G4PARTICLES}
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
  art::Framework_Services_Registry
  canvas::canvas
  art::Framework_Core
  art::Framework_Principal
  artg4tk::actionBase
  artg4tk::services_ActionHolder_service
  art::Persistency_Provenance
  CLHEP::CLHEP
  ${G4PARTICLES}
  MF_MessageLogger
  nusimdata_SimulationBase
  nug4_G4Base
  nug4_ParticleNavigation
  ROOT::Core
  ROOT::Physics
  Range::Range
SOURCE
  ParticleListAction_service.cxx
)

install_headers()
install_source()
