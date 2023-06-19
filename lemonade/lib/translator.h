#pragma once
#include "model_inventory.h"              // for ModelInventory
#include "model_manager.h"                // for ModelManager
#include "translator/response.h"          // for Response
#include "translator/service.h"           // for AsyncService
#include "translator/translation_model.h" // for TranslationModel
#include <cstddef>                        // for size_t
#include <memory>                         // for shared_ptr

namespace lemonade {

class Translator {

public:
  using Response = marian::bergamot::Response;
  using Model = std::shared_ptr<marian::bergamot::TranslationModel>;
  using Service = marian::bergamot::AsyncService;

  Translator(size_t maxModels, size_t numWorkers)
      : manager_(maxModels), config_{numWorkers, /*cacheSize=*/2000,
                                     /*loggerConfig=*/{/*logLevel=*/"info"}},
        service_(config_), inventory_() {}

  Response translate(std::string input, const std::string &source,
                     const std::string &target);

private:
  Translator::Model getModel(const ModelInventory::ModelInfo &info);

  ModelManager manager_;
  ModelInventory inventory_;

  Service::Config config_;
  Service service_;
};

} // namespace lemonade
