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
  CLHEP::Vector
  fhiclcpp::fhiclcpp
  Geant4::G4global
  Geant4::G4particles
  messagefacility::MF_MessageLogger
  nusimdata::SimulationBase
  nug4::G4Base
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
  CLHEP::Vector
  Geant4::G4particles
  messagefacility::MF_MessageLogger
  nusimdata::SimulationBase
  nug4::G4Base
  nug4::ParticleNavigation
  ROOT::Core
  ROOT::Physics
  range-v3::range-v3
SOURCE
  ParticleListAction_service.cxx
)

install_headers()
install_source()
