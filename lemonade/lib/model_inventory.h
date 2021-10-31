#pragma once
#include "logging.h"
#include "rapidjson/document.h"
#include <QStandardPaths>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

namespace lemonade {

class ModelInventory {
public:
  using LanguageDirection = std::pair<std::string, std::string>;

  struct ModelInfo {
    std::string name;
    std::string type;
    std::string code;
    LanguageDirection direction;
  };

  ModelInventory(const std::string &modelsJSON, const std::string &modelsDir);

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

  Logger logger_;
};

} // namespace lemonade
