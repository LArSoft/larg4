cet_build_plugin(larg4Main art::EDProducer
  LIBRARIES PRIVATE
  lardataobj::MCBase
  larg4::pluginActions_MCTruthEventAction_service
  larg4::pluginActions_ParticleListAction_service
  lardataalg::MCDumpers
  lardataobj::Simulation
  nug4::ParticleNavigation
  nurandom::RandomUtils_NuRandomService_service
  artg4tk::geantInit
  artg4tk::services_ActionHolder_service
  artg4tk::services_DetectorHolder_service
  artg4tk::services_PhysicsListHolder_service
  nusimdata::SimulationBase
  art::Framework_Principal
  art::Framework_Services_Registry
  canvas::canvas
  Geant4::G4intercoms
  Geant4::G4interfaces
  ROOT::Physics
)

cet_build_plugin(larg4SingleGen art::EDProducer
  LIBRARIES PRIVATE
  lardataobj::MCBase
  nusimdata::SimulationBase
  nurandom::RandomUtils_NuRandomService_service
  art::Framework_Services_Registry
  messagefacility::MF_MessageLogger
  fhiclcpp::types
  fhiclcpp::fhiclcpp
  cetlib::cetlib
  cetlib_except::cetlib_except
  ROOT::EG
  ROOT::Physics
  ROOT::Hist
  ROOT::RIO
  CLHEP::Random
)

install_headers()
install_source()
