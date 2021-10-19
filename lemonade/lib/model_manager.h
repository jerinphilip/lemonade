#pragma once
#include "translator/translation_model.h"
#include <list>
#include <memory>

namespace lemonade {
/// Manages models, LRU
class ModelManager {
  using Model = marian::bergamot::TranslationModel;
  using Entry = std::pair<std::string, std::shared_ptr<Model>>;
  using Container = std::list<Entry>;

public:
  ModelManager(size_t maxModels) : maxModels_(maxModels) {}

  void cacheModel(const std::string &key, std::shared_ptr<Model> model) {
    while (1 + models_.size() > maxModels_) {
      auto toRemoveItr = models_.begin();
      lookup_.erase(toRemoveItr->first);
      models_.erase(toRemoveItr);
    }
    auto modelItr = models_.emplace(models_.end(), std::make_pair(key, model));
    lookup_[key] = modelItr;
  }

  std::shared_ptr<Model> lookup(const std::string &key) {
    auto query = lookup_.find(key);
    if (query != lookup_.end()) {
      auto entryItr = query->second;
      auto entry = *entryItr;

      models_.erase(entryItr);
      auto ref = models_.emplace(models_.end(), entry);
      lookup_[key] = ref;
      return entry.second;
    }
    return nullptr;
  };

private:
  size_t maxModels_{0};
  Container models_;
  std::unordered_map<std::string, Container::iterator> lookup_;
};

} // namespace lemonade
