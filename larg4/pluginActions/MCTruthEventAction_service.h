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
#ifndef EXAMPLE_MCTRUTHEVENTACTION_SERVICE_HH
#define EXAMPLE_MCTRUTHEVENTACTION_SERVICE_HH

// Includes
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Core/EDProducer.h"

#include "Geant4/G4Event.hh"
#include "Geant4/G4VPrimaryGenerator.hh"
#include "Geant4/G4VUserPrimaryGeneratorAction.hh"
#include "Geant4/G4ParticleTable.hh"
#include "Geant4/globals.hh"

// Get the base class
#include "artg4tk/actionBase/PrimaryGeneratorActionBase.hh"

namespace larg4 {

  class MCTruthEventActionService
    : public artg4tk::PrimaryGeneratorActionBase {
  public:
    MCTruthEventActionService(fhicl::ParameterSet const&);

    void addG4Particle(G4Event *event,
                       int pdgId,
                       const G4ThreeVector& pos,
                       double time,
                       double energy,
                       const G4ThreeVector& mom);

    // To generate primaries, we need to overload the GeneratePrimaries
    // method.

    virtual void generatePrimaries(G4Event * anEvent) override;

    // We don't add anything to the event, so we don't need callArtProduces
    // or FillEventWithArtStuff.

  private:

    // A message logger for this action object
    mf::LogInfo logInfo_;
    static G4ParticleTable*           fParticleTable; ///< Geant4's table of particle definitions.
    std::map<G4int, G4int>            fUnknownPDG;    ///< map of unknown PDG codes to instances

  };
}//namespace larg4
using larg4::MCTruthEventActionService;
DECLARE_ART_SERVICE(MCTruthEventActionService,LEGACY)


#endif
