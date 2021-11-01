#pragma once
#include "model_inventory.h"              // for ModelInventory
#include "model_manager.h"                // for ModelManager
#include "translator/response.h"          // for Response
#include "translator/service.h"           // for AsyncService
#include "translator/translation_model.h" // for TranslationModel
#include "utils.h"
#include <cstddef> // for size_t
#include <memory>  // for shared_ptr

namespace lemonade {

class Translator {

public:
  Translator(size_t maxModels, size_t numWorkers)
      : manager_(maxModels), config_{numWorkers, /*cacheEnabled=*/true,
                                     /*cacheSize=*/2000,
                                     /*cacheMutexBuckets=*/numWorkers},
        service_(config_), inventory_() {}

  void translate(std::string input, const std::string &source,
                 const std::string &target,
                 marian::bergamot::CallbackType callback);

  marian::bergamot::Response btranslate(std::string input,
                                        const std::string &source,
                                        const std::string &target);

private:
  using Model = std::shared_ptr<marian::bergamot::TranslationModel>;
  using Service = marian::bergamot::AsyncService;

  ModelManager manager_;
  ModelInventory inventory_;

  Service::Config config_;
  Service service_;
};

} // namespace lemonade
