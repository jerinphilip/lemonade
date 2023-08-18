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
  if (m1_ && m2_) {
    // Pivoting.
    std::string pivot = m1_->translate(source);
    std::string translation = m2_->translate(pivot);
    return translation;
  }

  assert(m1_.get() != nullptr);

  return m1_->translate(source);
}

std::unique_ptr<Model> Translator::get_model(const Info &info) {
  std::string config_path = inventory_.configFile(info);
  LOG("Model file %s", config_path.c_str());
  YAML::Node config = YAML::LoadFile(config_path);

  LOG("Model building from bundle took %f seconds.\n", -1.0f);
  return std::make_unique<Model>(config);
}

Inventory::Inventory() {
  int argc = 0;
  char **argv = {};
  QCoreApplication(argc, argv); // NOLINT
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

void FakeTranslator::set_direction(const std::string &source,
                                   const std::string &target) {}

std::string FakeTranslator::translate(std::string input) {

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

std::string Model::translate(std::string input) {
  using namespace slimt;
  bool add_eos = true;
  auto [words, views] = vocabulary_.encode(input, add_eos);
  uint64_t batch_size = 1;
  uint64_t sequence_length = words.size();
  Batch batch(batch_size, sequence_length, vocabulary_.pad_id());
  batch.add(words);

  auto sentences = model_.translate(batch);
  auto sentence = sentences[0];
  auto [result, tgt_views] = vocabulary_.decode(sentence);
  return result;
}

Record<std::string> Model::load_path(YAML::Node &config) {
  auto prefix_browsermt = [](const std::string &path) { return path; };

  std::string model_path = prefix_browsermt(config["model"].as<std::string>());

  using Strings = std::vector<std::string>;
  Strings vocab_paths = config["vocab"].as<Strings>();
  std::string vocab_path = prefix_browsermt(vocab_paths[0]);

  Strings shortlist_args = config["shortlist"].as<Strings>();
  std::string shortlist_path = prefix_browsermt(shortlist_args[0]);

  Record<std::string> path{
      .model = model_path,        //
      .vocab = vocab_path,        //
      .shortlist = shortlist_path //
  };
  return path;
}

Record<slimt::io::MmapFile> Model::mmap_from(Record<std::string> &path) {
  using namespace slimt;
  auto prefix_browsermt = [](const std::string &path) { return path; };
  return {
      .model = io::MmapFile(path.model),         //
      .vocab = io::MmapFile(path.vocab),         //
      .shortlist = io::MmapFile(path.shortlist), //
  };
}

slimt::Model Model::load_model(slimt::Vocabulary &vocabulary,
                               Record<slimt::io::MmapFile> &mmap) {
  using namespace slimt;
  auto items = io::loadItems(mmap_.model.data());
  ShortlistGenerator shortlist_generator(             //
      mmap_.shortlist.data(), mmap_.shortlist.size(), //
      vocabulary, vocabulary                          //
  );

  size_t encoder_layers = 6;
  size_t decoder_layers = 2;
  size_t ffn_depth = 2;

  slimt::Model model(                //
      Tag::tiny11,                   //
      vocabulary,                    //
      std::move(items),              //
      std::move(shortlist_generator) //
  );
  return model;
}

} // namespace lemonade
