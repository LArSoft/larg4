//  MCTruthEventAction is the service that injects particles into
// the simulation.
// To use this action, all you need to do is put it in the services section
// of the configuration file, like this:
//
// services: {
//   ...
//     MCTruthEventAction: {}
//     ...
// }
// Expected parameters:
// - name (string): A name describing the action service.
//       Default is 'exampleParticleGun'

// Include guard
#ifndef MCTRUTHEVENTACTION_SERVICE_HH
#define MCTRUTHEVENTACTION_SERVICE_HH

#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "fhiclcpp/ParameterSet.h"

#include "artg4tk/actionBase/PrimaryGeneratorActionBase.hh"
#include "nug4/G4Base/ConvertMCTruthToG4.h"

#include "Geant4/G4Event.hh"
#include "Geant4/G4ParticleTable.hh"
#include "Geant4/G4VPrimaryGenerator.hh"
#include "Geant4/G4VUserPrimaryGeneratorAction.hh"
#include "Geant4/globals.hh"

#include <map>

namespace larg4 {

  class MCTruthEventActionService : public artg4tk::PrimaryGeneratorActionBase {
  public:
    MCTruthEventActionService(fhicl::ParameterSet const&);
    ~MCTruthEventActionService();

    void
    setInputCollections(std::vector<art::Handle<std::vector<simb::MCTruth>>> const& mclists)
    {
      fMCLists = &mclists;
    }

  private:
    // To generate primaries, we need to overload the GeneratePrimaries
    // method.

    void generatePrimaries(G4Event* anEvent) override;

    static G4ParticleTable* fParticleTable; ///< Geant4's table of particle definitions.
    std::vector<art::Handle<std::vector<simb::MCTruth>>> const*
      fMCLists;                            ///< MCTruthCollection input lists
    std::map<G4int, G4int> fUnknownPDG;    ///< map of unknown PDG codes to instances
    std::map<G4int, G4int> fNon1StatusPDG; ///< PDG codes skipped because not status 1
    std::map<G4int, G4int> fProcessedPDG;  ///< PDG codes processed
  };
} //namespace larg4

DECLARE_ART_SERVICE(larg4::MCTruthEventActionService, LEGACY)

#endif // MCTRUTHEVENTACTION_SERVICE_HH
