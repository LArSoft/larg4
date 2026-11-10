#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "fhiclcpp/ParameterSet.h"
#include "nusimdata/SimulationBase/MCTruth.h"

#include <memory>
#include <vector>

namespace larg4::test {
  class AncestryTestInput : public art::EDProducer {
  public:
    explicit AncestryTestInput(fhicl::ParameterSet const& config) : EDProducer{config}
    {
      produces<std::vector<simb::MCTruth>>();
    }

    void produce(art::Event& event) override
    {
      // One truth record lets the tracking action serialize its end-of-event map.
      event.put(std::make_unique<std::vector<simb::MCTruth>>(1));
    }
  };
}

DEFINE_ART_MODULE(larg4::test::AncestryTestInput)
