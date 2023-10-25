#pragma once
#include "rapidjson/document.h"
#include "slimt/slimt.hh"
#include "yaml-cpp/yaml.h"
#include <QStandardPaths>
#include <cstddef>
#include <memory>
#include <optional>

namespace lemonade {

using Direction = std::pair<std::string, std::string>;

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
  std::pair<std::string, std::string> config_file(const Info &info);

private:
  struct Hash {
    size_t operator()(const Direction &direction) const;
  };

  std::unordered_map<Direction, Info, Hash> directions_;
  rapidjson::Document inventory_;
  std::string models_dir_;
  std::string models_json_;

  static rapidjson::Document load(const std::string &json);
};

template <class T> using Package = slimt::Package<T>;

class Translator {
public:
  explicit Translator(const slimt::Config &config) : service_(config) {}
  void set_direction(const std::string &source, const std::string &target);
  std::string translate(const std::string &source);

  bool pivot() { return m1_ != nullptr and m2_ != nullptr; }

private:
  std::shared_ptr<slimt::Model> get_model(const Info &info);
  Inventory inventory_;

  const std::string source_;
  const std::string target_;

  slimt::Async service_;
  std::shared_ptr<slimt::Model> m1_;
  std::shared_ptr<slimt::Model> m2_;
};

class FakeTranslator {
public:
  explicit FakeTranslator(const slimt::Config &config){};
  void set_direction(const std::string &source, const std::string &target);
  std::string translate(std::string input);
};

} // namespace lemonade
