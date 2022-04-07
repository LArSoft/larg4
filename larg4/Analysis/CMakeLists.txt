art_make(
  MODULE_LIBRARIES
    art_root_io::TFileService_service
    art::Framework_Services_Registry
    art::Persistency_Provenance
    art::Utilities
    artg4tk::services_DetectorHolder_service
    Boost::filesystem
    canvas::canvas
    cetlib_except::cetlib_except
    ${G4ZLIB}
    nusimdata::SimulationBase
    ROOT::Core
    ROOT::Hist
    ROOT::Tree
) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.09.03) - deprecated: use art_make_library(), art_dictonary(), and cet_build_plugin() with explicit source lists and plugin base types
install_source()
