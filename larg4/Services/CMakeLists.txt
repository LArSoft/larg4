simple_plugin(
  LArG4Detector service
  SOURCE
    LArG4Detector_service.cc
    SimEnergyDepositSD.cc
    AuxDetSD.cc
  NOP
    art::Framework_Core
    art::Framework_Principal
    art::Framework_Services_Registry
    artg4tk::DataProducts_G4DetectorHits
    artg4tk::pluginDetectors_gdml
    artg4tk::services_DetectorHolder_service
    larg4_pluginActions_ParticleListAction_service
    art::Persistency_Provenance
    canvas::canvas
    cetlib::cetlib
    cetlib_except::cetlib_except
    fhiclcpp::fhiclcpp
    nug4::ParticleNavigation
    nusimdata::SimulationBase
    Geant4::G4digits_hits
    Geant4::G4event
    Geant4::G4geometry
    Geant4::G4global
    Geant4::G4materials
    Geant4::G4persistency
    larcorealg_Geometry
    messagefacility::MF_MessageLogger
    ROOT::Core
    XercesC::XercesC
) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.09.03) - deprecated: use cet_build_plugin() with explicit source lists and plugin base types

install_headers()
install_source()
