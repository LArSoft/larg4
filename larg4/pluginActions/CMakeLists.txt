cet_build_plugin(MCTruthEventAction artg4tk::ActionService
  LIBRARIES
  PUBLIC
  nug4::ParticleNavigation
  nug4::G4Base
  nusimdata::SimulationBase
  art::Framework_Principal
  art::Framework_Services_Registry
  Geant4::G4global
  PRIVATE
  canvas::canvas
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
  Geant4::G4particles
  Geant4::G4event
  CLHEP::Vector
  ROOT::Physics
)

cet_build_plugin(ParticleListAction artg4tk::ActionService
  LIBRARIES
  PUBLIC
  lardataobj::Simulation
  nug4::ParticleNavigation
  nug4::G4Base
  nusimdata::SimulationBase
  art::Framework_Principal
  art::Framework_Services_Registry
  canvas::canvas
  messagefacility::MF_MessageLogger
  Geant4::G4global
  ROOT::Core
  larcorealg::headers
  larcore::Geometry_Geometry_service
  PRIVATE
  fhiclcpp::fhiclcpp
  ROOT::Physics
  Geant4::G4tracking
  Geant4::G4track
  Geant4::G4particles
  range-v3::range-v3
)

install_headers()
install_source()
