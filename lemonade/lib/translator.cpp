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
  if (source == "English" or target == "English") {
    std::optional<ModelInventory::ModelInfo> modelInfo =
        inventory_.query(source, target);

    if (modelInfo) {
      Model model = getModel(modelInfo.value());
      marian::bergamot::ResponseOptions responseOptions;
      service_.translate(model, std::move(input), callback, responseOptions);
    }
  } else {
    // Try to translate by pivoting.
    std::optional<ModelInventory::ModelInfo> first =
        inventory_.query(source, "English");
    std::optional<ModelInventory::ModelInfo> second =
        inventory_.query("English", target);

    Model sourceToPivot = getModel(first.value());
    Model pivotToTarget = getModel(second.value());
    marian::bergamot::ResponseOptions responseOptions;
    service_.pivot(sourceToPivot, pivotToTarget, std::move(input), callback,
                   responseOptions);
  }
}

Translator::Model Translator::getModel(const ModelInventory::ModelInfo &info) {
  Model model = manager_.lookup(info.code);
  if (!model) {
    auto modelConfig =
        marian::bergamot::parseOptionsFromFilePath(inventory_.configFile(info));

    // FIXME
    modelConfig->set("workspace", 128);

    marian::timer::Timer timer;
    model = service_.createCompatibleModel(modelConfig);

    std::cerr << fmt::format("Model building from bundle took {} seconds.\n",
                             timer.elapsed());
    manager_.cacheModel(info.code, model);
  }

  return model;
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
