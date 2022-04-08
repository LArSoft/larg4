cet_build_plugin(CheckAuxDetHit art::EDAnalyzer
  LIBRARIES PRIVATE
  lardataobj::Simulation
  artg4tk::services_DetectorHolder_service
  art_root_io::TFileService_service
  art_root_io::tfile_support
  art::Framework_Services_Registry
  art::Framework_Principal
  ROOT::Hist
  ROOT::Tree
  ROOT::RIO
)

cet_build_plugin(CheckMCParticle art::EDAnalyzer
  LIBRARIES PRIVATE
  nusimdata::SimulationBase
  art_root_io::TFileService_service
  art_root_io::tfile_support
  art::Framework_Services_Registry
  art::Framework_Principal
  ROOT::Hist
  ROOT::RIO
)

cet_build_plugin(CheckSimEnergyDeposit art::EDAnalyzer
  LIBRARIES PRIVATE
  lardataobj::Simulation
  artg4tk::services_DetectorHolder_service
  art_root_io::TFileService_service
  art_root_io::tfile_support
  ROOT::Hist
  ROOT::Tree
)

install_source()
