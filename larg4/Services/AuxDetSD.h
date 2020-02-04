//=============================================================================
// AuxDetSD.h: Class representing a sensitive for a thin CRT detector
// Author: Hans Wenzel (Fermilab)
//=============================================================================
#ifndef AuxDetSD_h
#define AuxDetSD_h 1
#include "lardataobj/Simulation/AuxDetHit.h"
#include "larg4/Services/TempHit.h"
#include "larcore/Geometry/Geometry.h"
#include "Geant4/G4VSensitiveDetector.hh"

#if defined __clang__
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunused-private-field"
#endif

class G4Step;
class G4HCofThisEvent;
class AuxDetHitCollection;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace larg4 {

    class AuxDetSD : public G4VSensitiveDetector {
    public:
      AuxDetSD(G4String name );
      virtual ~AuxDetSD();
      void Initialize(G4HCofThisEvent*);
      void EndOfEvent(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      const sim::AuxDetHitCollection& GetHits() const { return hitCollection; }

    private:
      TempHitCollection temphitCollection;
      sim::AuxDetHitCollection hitCollection;
    };
}   // namespace larg4
#if defined __clang__
  #pragma clang diagnostic pop
#endif

#endif

