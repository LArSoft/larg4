#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "fhiclcpp/ParameterSet.h"
#include "nusimdata/SimulationBase/MCTruth.h"

#include "TLorentzVector.h"

#include <memory>
#include <utility>
#include <vector>

namespace larg4::test {
  class PrimaryTestInput : public art::EDProducer {
  public:
    explicit PrimaryTestInput(fhicl::ParameterSet const& config) : EDProducer{config}
    {
      produces<std::vector<simb::MCTruth>>();
    }

    void produce(art::Event& event) override
    {
      // Give the tracking action real generator indices to preserve.
      auto truths = std::make_unique<std::vector<simb::MCTruth>>(1);
      for (int index = 0; index < 3; ++index) {
        simb::MCParticle particle{index, 2212, "primary", -1, 0.938272081, 1};
        particle.AddTrajectoryPoint(TLorentzVector{}, TLorentzVector{0., 0., 0., particle.Mass()});
        truths->front().Add(particle);
      }
      event.put(std::move(truths));
    }
  };
}

DEFINE_ART_MODULE(larg4::test::PrimaryTestInput)
