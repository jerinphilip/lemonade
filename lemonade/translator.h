#pragma once
#include "rapidjson/document.h"
#include "translator/response.h"          // for Response
#include "translator/service.h"           // for AsyncService
#include "translator/translation_model.h" // for TranslationModel
#include <QStandardPaths>
#include <cstddef> // for size_t
#include <memory>  // for shared_ptr
#include <optional>

namespace lemonade {

using Model = std::shared_ptr<marian::bergamot::TranslationModel>;
using LanguageDirection = std::pair<std::string, std::string>;
using Response = marian::bergamot::Response;
using Service = marian::bergamot::AsyncService;

struct ModelInfo {
  std::string name;
  std::string type;
  std::string code;
  LanguageDirection direction;
};

class ModelInventory {
public:
  ModelInventory();
  std::optional<ModelInfo> query(const std::string &source,
                                 const std::string &target) const;
  std::string configFile(const ModelInfo &modelInfo);

private:
  struct Hash {
    size_t operator()(const LanguageDirection &direction) const;
  };

  std::unordered_map<LanguageDirection, ModelInfo, Hash> languageDirections_;
  rapidjson::Document inventory_;
  std::string modelsDir_;
  std::string modelsJSON_;

  static rapidjson::Document
  readInventoryFromDisk(const std::string &modelsJSON);
};

/// Manages models, LRU
class ModelManager {
  using Entry = std::pair<std::string, Model>;
  using Container = std::list<Entry>;

public:
  ModelManager(size_t maxModels) : maxModels_(maxModels) {}
  void cacheModel(const std::string &key, Model model);
  Model lookup(const std::string &key);

private:
  size_t maxModels_{0};
  Container models_;
  std::unordered_map<std::string, Container::iterator> lookup_;
};

class Translator {

public:
  Translator(size_t maxModels, size_t numWorkers)
      : manager_(maxModels), config_{numWorkers, /*cacheSize=*/2000,
                                     /*loggerConfig=*/{/*logLevel=*/"info"}},
        service_(config_), inventory_() {}

  Response translate(std::string input, const std::string &source,
                     const std::string &target);

private:
  Model getModel(const ModelInfo &info);

  ModelManager manager_;
  ModelInventory inventory_;

  Service::Config config_;
  Service service_;
};

class FakeTranslator {
public:
  FakeTranslator(size_t /*maxModels*/, size_t /*numWorkers*/){};
  Response translate(std::string input, const std::string &source_lang,
                     const std::string &target_lang);
};

} // namespace lemonade
