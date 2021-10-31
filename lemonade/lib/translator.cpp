#include "translator.h"
#include "common/timer.h"                 // for Timer
#include "lemonade/lib/model_inventory.h" // for ModelInventory, ModelInven...
#include "translator/byte_array_util.h"   // for getMemoryBundleFromConfig
#include "translator/definitions.h"       // for MemoryBundle
#include "translator/response_options.h"  // for ResponseOptions
#include <future>                         // for future, promise

namespace lemonade {

void Translator::translate(std::string input, const std::string &source,
                           const std::string &target,
                           marian::bergamot::CallbackType callback) {
  std::optional<ModelInventory::ModelInfo> modelInfo =
      inventory_.query(source, target);

  if (modelInfo) {
    ModelInventory::ModelInfo m = modelInfo.value();
    Model model = manager_.lookup(m.code);
    if (!model) {
      auto modelConfig =
          marian::bergamot::parseOptionsFromFilePath(inventory_.configFile(m));

      // FIXME
      modelConfig->set("workspace", 128);

      marian::timer::Timer bundleTimer;
      marian::bergamot::MemoryBundle memoryBundle =
          marian::bergamot::getMemoryBundleFromConfig(modelConfig);
      std::cout << fmt::format("Bundle loading from disk {} seconds.\n",
                               bundleTimer.elapsed());
      marian::timer::Timer timer;
      model =
          service_.createCompatibleModel(modelConfig, std::move(memoryBundle));

      std::cout << fmt::format("Model building from bundle took {} seconds.\n",
                               timer.elapsed());
      manager_.cacheModel(m.code, model);
    }

    marian::bergamot::ResponseOptions responseOptions;
    service_.translate(model, std::move(input), callback, responseOptions);
  }
}

marian::bergamot::Response Translator::btranslate(std::string input,
                                                  const std::string &source,
                                                  const std::string &target) {

  using Response = marian::bergamot::Response;
  std::promise<Response> p;
  std::future<Response> f = p.get_future();
  auto callback = [&p](Response &&response) {
    p.set_value(std::move(response));
  };

  translate(std::move(input), source, target, callback);
  f.wait();
  return f.get();
}

} // namespace lemonade
