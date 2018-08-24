//
//               __        __ __  __  __  
//   ____ ______/ /_____ _/ // / / /_/ /__
//  / __ `/ ___/ __/ __ `/ // /_/ __/ //_/
// / /_/ / /  / /_/ /_/ /__  __/ /_/ ,<   
// \__,_/_/   \__/\__, /  /_/  \__/_/|_|  
//               /____/                  
//
// artg4tk: art based Geant 4 Toolkit
// 
//=============================================================================
// AuxDetSD.h: Class representing a sensitive tracking detector
// Author: Hans Wenzel (Fermilab)
//=============================================================================
#ifndef AuxDetSD_h
#define AuxDetSD_h 1
#include "lardataobj/Simulation/AuxDetSimChannel.h"
#include "larcore/Geometry/Geometry.h"
#include "larcorealg/Geometry/AuxDetGeo.h"
#include "larcorealg/Geometry/AuxDetSensitiveGeo.h"
#include "Geant4/G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;
class AuxDetHitCollection;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace larg4 {

    class AuxDetSD : public G4VSensitiveDetector {
    public:
      //Constructor


      AuxDetSD(std::string const& name, 
	       unsigned int       adNum=0,
	       unsigned int       svNum=0);
    // Destructor
      virtual ~AuxDetSD();
      
      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      const sim::AuxDetSimChannelCollection& GetHits() const { return hitCollection; }
      virtual void AddParticleStep(int	inputTrackID,
				   float	inputEnergyDeposited,
				   float	inputEntryX,
				   float	inputEntryY,
				   float	inputEntryZ,
				   float	inputEntryT,
				   float	inputExitX,
				   float	inputExitY,
				   float	inputExitZ,
				   float	inputExitT,
				   float	inputExitMomentumX,
				   float	inputExitMomentumY,
				   float	inputExitMomentumZ);
      sim::AuxDetSimChannel const GetAuxDetSimChannel() const { return fAuxDetSimChannel; };
        
    // Called to clear any accumulated information.
    virtual void clear();
    private:
      sim::AuxDetSimChannelCollection hitCollection;
   art::ServiceHandle<geo::Geometry> fGeoHandle;        ///< Handle to the Geometry service
    uint32_t                          fAuxDet;           ///< which AuxDet this AuxDetReadout corresponds to
    uint32_t                          fAuxDetSensitive;  ///< which sensitive volume of the AuxDet this AuxDetReadout corresponds to
    sim::AuxDetSimChannel             fAuxDetSimChannel; ///< Contains the sim::AuxDetSimChannel for this AuxDet
    std::vector<sim::AuxDetIDE>       fAuxDetIDEs;       ///< list of IDEs in one channel
      //G4int HCID;
      //  art::ServiceHandle<geo::Geometry> fGeoHandle;        ///< Handle to the Geometry service
      //uint32_t                          fAuxDet;           ///< which AuxDet this AuxDetReadout corresponds to
      //uint32_t                          fAuxDetSensitive;  ///< which sensitive volume of the AuxDet this AuxDetReadout corresponds to
      //sim::AuxDetSimChannel             fAuxDetSimChannel; ///< Contains the sim::AuxDetSimChannel for this AuxDet
      //std::vector<sim::AuxDetIDE>       fAuxDetIDEs;       ///< list of IDEs in one channel
    };
}   // namespace larg4
#endif

