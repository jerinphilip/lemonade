#include "translator.h"
#include "logging.h"
#include <future>
#include <optional>
#include <random>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <QCommandLineParser>
#include <QStandardPaths>

namespace lemonade {

std::string Translator::translate(std::string input, const std::string &source,
                                  const std::string &target) {

  // I don't even know why added this.
  std::promise<std::string> p;
  std::future<std::string> f = p.get_future();
  auto callback = [&p](std::string &&response) {
    p.set_value(std::move(response));
  };

  if (source == "English" or target == "English") {
    std::optional<Info> info = inventory_.query(source, target);

    if (info) {
    } else {
      LOG("No model found for %s -> %s\n", source.c_str(), target.c_str());
    }
  } else {
    // Try to translate by pivoting.
    std::optional<Info> first = inventory_.query(source, "English");
    std::optional<Info> second = inventory_.query("English", target);

    Model *source_to_pivot = get_model(first.value());
    Model *pivot_to_target = get_model(second.value());
  }

  f.wait();
  return f.get();
}

Model *Translator::get_model(const Info &info) {
  Model *model = manager_.lookup(info.code);
  if (!model) {
    LOG("Model file %s", inventory_.configFile(info).c_str());
    LOG("Model building from bundle took %f seconds.\n", -1.0f);
    // manager_.cacheModel(info.code, model);
  }

  return model;
}

Inventory::Inventory() {
  int argc = 0;
  char **argv = {};
  QCoreApplication(argc, argv);
  QCoreApplication::setApplicationName("bergamot");

  modelsJSON_ = QStandardPaths::locate(QStandardPaths::AppConfigLocation,
                                       "browsermt/models.json")
                    .toStdString();

  inventory_ = readInventoryFromDisk(modelsJSON_);

  modelsDir_ = QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                      "models/browsermt",
                                      QStandardPaths::LocateDirectory)
                   .toStdString();
  // LEMONADE_ABORT_IF(!inventory_.HasMember("models"), "No models found");
  const rapidjson::Value &models = inventory_["models"];

  for (size_t i = 0; i < models.Size(); i++) {
    const rapidjson::Value &entry = models[i];
    std::string type = entry["type"].GetString();
    if (type == "tiny") {
      Direction direction =
          std::make_pair(entry["src"].GetString(), entry["trg"].GetString());

      Info info{entry["name"].GetString(), entry["type"].GetString(),
                entry["code"].GetString(), direction};

      directions_[direction] = info;
      LOG("Found model %s (%s -> %s)", info.code.c_str(),
          info.direction.first.c_str(), info.direction.second.c_str());
    }
  }
}

std::optional<Info> Inventory::query(const std::string &source,
                                     const std::string &target) const {
  auto query = directions_.find(Direction{source, target});
  if (query != directions_.end()) {
    return query->second;
  }
  return std::nullopt;
}

size_t Inventory::Hash::operator()(const Direction &direction) const {
  auto hash_combine = [](size_t &seed, size_t next) {
    seed ^= std::hash<size_t>{}(next) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  };

  size_t seed = std::hash<std::string>{}(direction.first);
  hash_combine(seed, std::hash<std::string>{}(direction.second));
  return seed;
}

rapidjson::Document
Inventory::readInventoryFromDisk(const std::string &modelsJSON) {
  FILE *fp = fopen(modelsJSON.c_str(), "r"); // non-Windows use "r"

  if (!fp) {
    LOG("File %s not found", modelsJSON.c_str());
    std::abort();
  }
  char readBuffer[65536];
  rapidjson::FileReadStream fReadStream(fp, readBuffer, sizeof(readBuffer));
  rapidjson::Document d;
  d.ParseStream(fReadStream);
  fclose(fp);
  return d;
}

std::string Inventory::configFile(const Info &info) {
  std::string configFilePath =
      modelsDir_ + "/" + info.code + "/config.bergamot.yml";
  return configFilePath;
}
void ModelManager::cacheModel(const std::string &key, Model &&model) {
  // LRU cache.
  while (1 + models_.size() > max_models_to_cache_) {
    // Remove from the front.
    auto toRemoveItr = models_.begin();
    lookup_.erase(toRemoveItr->first);
    models_.erase(toRemoveItr);
  }
  auto modelItr =
      models_.emplace(models_.end(), std::make_pair(key, std::move(model)));
  lookup_[key] = modelItr;
}
Model *ModelManager::lookup(const std::string &key) {
  auto query = lookup_.find(key);
  if (query != lookup_.end()) {
    auto entryItr = query->second;
    auto entry = std::move(*entryItr);
    models_.erase(entryItr);
    auto ref = models_.emplace(models_.end(), std::move(entry));
    lookup_[key] = ref;
    return &(ref->second);
  }
  return nullptr;
};

std::string FakeTranslator::translate(std::string input,
                                      const std::string &source_lang,
                                      const std::string &target_lang) {

  std::string response;
  if (input.empty()) {
    return response;
  }

  // For a given length, generates a 6 length set of tokens.
  // Entire string is changed by seeding with length each time.
  // Simulates translation in some capacity.
  auto transform = [](size_t length) -> std::string {
    std::mt19937_64 generator;
    size_t truncate_length = 6;
    generator.seed(length);
    std::string target;
    for (size_t i = 0; i < length; i++) {
      if (i != 0) {
        target += " ";
      }
      size_t value = generator();
      std::string hex(' ', 20);
      std::sprintf(hex.data(), "%x", static_cast<unsigned int>(value));
      // 2 to truncate 0x.
      target += hex.substr(2, truncate_length);
    }
    return target;
  };

  auto token_count = [](const std::string &input) -> size_t {
    std::string token;
    size_t count = 0;
    for (size_t i = 0; i < input.size(); i++) {
      char c = input[i];
      if (isspace(c)) {
        // Check for space.
        if (!token.empty()) {
          ++count;
          token = "";
        }
      } else {
        token += std::string(1, c);
      }
    }
    // Non space-detected overhang.
    if (!token.empty()) {
      count += 1;
    }

    return count;
  };

  size_t count = token_count(input);
  std::string target = transform(count);
  return target;
}

} // namespace lemonade
