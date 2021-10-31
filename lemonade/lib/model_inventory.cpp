#include "model_inventory.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <QCommandLineParser>
#include <QStandardPaths>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <spdlog/spdlog.h>
#include <string>
#include <utility>

namespace lemonade {

ModelInventory::ModelInventory() : logger_("inventory") {

  int argc = 0;
  char **argv = {};
  QCoreApplication(argc, argv);
  QCoreApplication::setApplicationName("lemonade");

  modelsJSON_ =
      QStandardPaths::locate(QStandardPaths::AppConfigLocation, "models.json")
          .toStdString();

  inventory_ = readInventoryFromDisk(modelsJSON_);

  modelsDir_ = QStandardPaths::locate(QStandardPaths::AppDataLocation, "models",
                                      QStandardPaths::LocateDirectory)
                   .toStdString();
  // LEMONADE_ABORT_IF(!inventory_.HasMember("models"), "No models found");
  const rapidjson::Value &models = inventory_["models"];

  for (size_t i = 0; i < models.Size(); i++) {
    const rapidjson::Value &entry = models[i];
    std::string type = entry["type"].GetString();
    if (type == "tiny") {
      LanguageDirection direction =
          std::make_pair(entry["src"].GetString(), entry["trg"].GetString());

      ModelInfo modelInfo{entry["name"].GetString(), entry["type"].GetString(),
                          entry["code"].GetString(), direction};

      languageDirections_[direction] = modelInfo;
      logger_.log(fmt::format("Found model {} ({} -> {})", modelInfo.code,
                              modelInfo.direction.first,
                              modelInfo.direction.second));
    }
  }
}

std::optional<ModelInventory::ModelInfo>
ModelInventory::query(const std::string &source,
                      const std::string &target) const {
  auto query = languageDirections_.find(LanguageDirection{source, target});
  if (query != languageDirections_.end()) {
    return query->second;
  }
  return std::nullopt;
}

size_t
ModelInventory::Hash::operator()(const LanguageDirection &direction) const {
  auto hash_combine = [](size_t &seed, size_t next) {
    seed ^= std::hash<size_t>{}(next) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  };

  size_t seed = std::hash<std::string>{}(direction.first);
  hash_combine(seed, std::hash<std::string>{}(direction.second));
  return seed;
}

rapidjson::Document
ModelInventory::readInventoryFromDisk(const std::string &modelsJSON) {
  FILE *fp = fopen(modelsJSON.c_str(), "r"); // non-Windows use "r"
  char readBuffer[65536];
  rapidjson::FileReadStream fReadStream(fp, readBuffer, sizeof(readBuffer));
  rapidjson::Document d;
  d.ParseStream(fReadStream);
  fclose(fp);
  return d;
}

std::string ModelInventory::configFile(const ModelInfo &modelInfo) {
  std::string configFilePath =
      fmt::format("{}/{}/config.bergamot.yml", modelsDir_, modelInfo.code);
  return configFilePath;
}

} // namespace lemonade
