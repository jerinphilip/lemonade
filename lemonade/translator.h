#pragma once
#include "rapidjson/document.h"
#include "slimt/slimt.hh"
#include <QStandardPaths>
#include <cstddef> // for size_t
#include <memory>  // for shared_ptr
#include <optional>

namespace lemonade {

using Direction = std::pair<std::string, std::string>;
using Model = slimt::Model;

struct Info {
  std::string name;
  std::string type;
  std::string code;
  Direction direction;
};

class Inventory {
public:
  Inventory();
  std::optional<Info> query(const std::string &source,
                            const std::string &target) const;
  std::string configFile(const Info &info);

private:
  struct Hash {
    size_t operator()(const Direction &direction) const;
  };

  std::unordered_map<Direction, Info, Hash> directions_;
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
  ModelManager(size_t max_models_to_cache)
      : max_models_to_cache_(max_models_to_cache) {}
  void cacheModel(const std::string &key, Model &&model);
  Model *lookup(const std::string &key);

private:
  size_t max_models_to_cache_{0};
  Container models_;
  std::unordered_map<std::string, Container::iterator> lookup_;
};

class Translator {
public:
  Translator(size_t max_models_to_cache, size_t numWorkers)
      : manager_(max_models_to_cache), inventory_() {}

  std::string translate(std::string input, const std::string &source,
                        const std::string &target);

private:
  Model *get_model(const Info &info);
  ModelManager manager_;
  Inventory inventory_;
};

class FakeTranslator {
public:
  FakeTranslator(size_t /*max_models_to_cache*/, size_t /*numWorkers*/){};
  std::string translate(std::string input, const std::string &source_lang,
                        const std::string &target_lang);
};

} // namespace lemonade
