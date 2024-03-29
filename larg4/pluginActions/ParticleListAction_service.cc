////////////////////////////////////////////////////////////////////////
/// \file  ParticleListAction.cxx
/// \brief Use Geant4's user "hooks" to maintain a list of particles generated by Geant4.
///
/// \author  seligman@nevis.columbia.edu
///
/// Design considerations
/// ---------------------
///
/// This class relies on the MCTruth index from
/// g4b::PrimaryParticleInformation to operate correctly.  This index
/// is an integer value that corresponds to an MCTruth object, as
/// accessed through art::Handle<std::vector<simb::MCTruth>> objects.
/// However, the order in which MCTruth objects are processed must be
/// consistent between this service and the MCTruthEventAction
/// service, which creates the PrimaryParticleInformation object,
/// otherwise the Assns objects created here will be incorrect.
///
/// Through art 3.09, one can rely on the order returned by a given
/// Event::getMany call to be predictable and consistent within the
/// same program.  However, this behavior should not necessarily be
/// relied upon, and a different implementation of this class would
/// insulate users from such details, making the implementation
/// simpler.  One should determine whether storing an art::ProductID
/// object along with an MCTruthIndex might be more helpful.
///
////////////////////////////////////////////////////////////////////////

#include "larg4/pluginActions/ParticleListAction_service.h"

#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"

DEFINE_ART_SERVICE(larg4::ParticleListActionService)
