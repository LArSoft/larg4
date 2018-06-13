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
// classes.h: Class which want to make persistent
// Author: Hans Wenzel (Fermilab)
//=============================================================================

//#include <vector>
//#include <map>


#include "canvas/Persistency/Common/Wrapper.h"
#include "artg4tk/pluginDetectors/gdml/CalorimeterHit.hh"
#include "artg4tk/pluginDetectors/gdml/DRCalorimeterHit.hh"
#include "artg4tk/pluginDetectors/gdml/PhotonHit.hh"
#include "artg4tk/pluginDetectors/gdml/TrackerHit.hh"
#include "larg4/SimEnergyDeposit/SimEnergyDepositHit.hh"
#include "artg4tk/pluginDetectors/gdml/myInteractionArtHitData.hh"
#include "artg4tk/pluginDetectors/gdml/myParticleEContribArtData.hh"
//#include "artg4tk/pluginDetectors/gdml/myParticleNCerenContribArtData.hh"
// Template the wrapper for the vector (typedef okay)
template class art::Wrapper< artg4tk::CalorimeterHitCollection >;
template class art::Wrapper< artg4tk::DRCalorimeterHitCollection >;
template class art::Wrapper< artg4tk::PhotonHitCollection >;
template class art::Wrapper< artg4tk::TrackerHitCollection >;
template class art::Wrapper< artg4tk::SimEnergyDepositHitCollection >;
template class art::Wrapper< artg4tk::myInteractionArtHitDataCollection >;
template class art::Wrapper< artg4tk::myParticleEContribArtData >;
//template class art::Wrapper< artg4tk::myParticleNCerenContribArtData >;
