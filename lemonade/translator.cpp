#include "translator.h"
#include "logging.h"
#include <future>
#include <optional>
#include <random>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "yaml-cpp/yaml.h"
#include <QCommandLineParser>
#include <QStandardPaths>

namespace lemonade {

Inventory::Inventory() {
  int argc = 0;
  char **argv = {};
  QCoreApplication(argc, argv); // NOLINT
  QCoreApplication::setApplicationName("bergamot");

  models_json_ = QStandardPaths::locate(QStandardPaths::AppConfigLocation,
                                        "browsermt/models.json")
                     .toStdString();

  inventory_ = load(models_json_);

  models_dir_ = QStandardPaths::locate(QStandardPaths::AppDataLocation,
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
    seed ^= (std::hash<size_t>{}(next) //
             + 0x9e3779b9              // NOLINT
             + (seed << 6)             // NOLINT
             + (seed >> 2)             // NOLINT
    );
  };

  size_t seed = std::hash<std::string>{}(direction.first);
  hash_combine(seed, std::hash<std::string>{}(direction.second));
  return seed;
}

rapidjson::Document Inventory::load(const std::string &json) {
  FILE *fp = fopen(json.c_str(), "r"); // non-Windows use "r"

  if (!fp) {
    LOG("File %s not found", json.c_str());
    std::abort();
  }

  constexpr size_t kMaxBufferSize = 65536;
  char buffer[kMaxBufferSize];
  rapidjson::FileReadStream stream(fp, buffer, sizeof(buffer));
  rapidjson::Document document;
  document.ParseStream(stream);
  fclose(fp);

  return document;
}

std::pair<std::string, std::string> Inventory::config_file(const Info &info) {
  return std::make_pair(models_dir_ + "/" + info.code, "config.bergamot.yml");
}

std::shared_ptr<slimt::Model> make_model(const std::string &root,
                                         YAML::Node &config) {
  auto prefix_browsermt = [&root](const std::string &path) {
    return root + "/" + path;
  };

  using Strings = std::vector<std::string>;

  auto model_paths = config["models"].as<Strings>();
  std::string model_path = prefix_browsermt(model_paths[0]);
  LOG("model_path: %s", model_path.c_str());

  auto vocab_paths = config["vocabs"].as<Strings>();
  std::string vocab_path = prefix_browsermt(vocab_paths[0]);

  auto shortlist_args = config["shortlist"].as<Strings>();
  std::string shortlist_path = prefix_browsermt(shortlist_args[0]);

  slimt::Package<std::string> path{
      .model = model_path,        //
      .vocabulary = vocab_path,   //
      .shortlist = shortlist_path //
  };

  slimt::Model::Config model_config;
  return std::make_shared<slimt::Model>(model_config, path);
}

void Translator::set_direction(const std::string &source,
                               const std::string &target) {
  if (source == "English" or target == "English") {
    std::optional<Info> info = inventory_.query(source, target);
    if (info) {
      m1_ = get_model(info.value());
      LOG("Found model %s (%s -> %s)", info->code.c_str(),
          info->direction.first.c_str(), info->direction.second.c_str());
    } else {
      LOG("No model found for %s -> %s", source.c_str(), target.c_str());
    }
  } else {
    // Try to translate by pivoting.
    std::optional<Info> first = inventory_.query(source, "English");
    std::optional<Info> second = inventory_.query("English", target);

    m1_ = get_model(first.value());
    m2_ = get_model(second.value());
  }
}

std::string Translator::translate(const std::string &source) {
  slimt::Options options{.html = false};

  if (m1_ && m2_) {
    // Pivoting.
    std::future<slimt::Response> future =
        service_.pivot(m1_, m2_, source, options);
    slimt::Response response = future.get();
    return response.target.text;
  }

  assert(m1_ != nullptr);

  std::future<slimt::Response> future =
      service_.translate(m1_, source, options);
  slimt::Response response = future.get();
  return response.target.text;
}

std::shared_ptr<slimt::Model> Translator::get_model(const Info &info) {
  auto [model_root, config] = inventory_.config_file(info);
  LOG("Model model_root %s, config %s", model_root.c_str(), config.c_str());
  YAML::Node tree = YAML::LoadFile(model_root + "/" + config);
  LOG("Model building from bundle took %f seconds.\n", -1.0F);
  return make_model(model_root, tree);
}

void FakeTranslator::set_direction(const std::string &source,
                                   const std::string &target) {}

std::string FakeTranslator::translate(std::string input) { // NOLINT

  std::string response;
  if (input.empty()) {
    return response;
  }

  // For a given length, generates a 6 length set of tokens.
  // Entire string is changed by seeding with length each time.
  // Simulates translation in some capacity.
  auto transform = [](size_t length) -> std::string {
    std::mt19937_64 generator;
    constexpr size_t kTruncateLength = 6;
    generator.seed(length);
    std::string target;
    for (size_t i = 0; i < length; i++) {
      if (i != 0) {
        target += " ";
      }
      size_t value = generator();
      constexpr size_t kMaxLength = 20;
      std::string hex(kMaxLength, ' ');
      std::sprintf(hex.data(), "%x", static_cast<unsigned int>(value));
      // 2 to truncate 0x.
      target += hex.substr(2, kTruncateLength);
    }
    return target;
  };

  auto token_count = [](const std::string &input) -> size_t {
    std::string token;
    size_t count = 0;
    for (char c : input) {
      if (isspace(c)) {
        // Check for space.
        if (!token.empty()) {
          // Start of a new word.
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
