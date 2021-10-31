#define CROW_MAIN
#include "crow.h"
#include "lemonade/lib/data.h"
#include "lemonade/lib/json_interop.h"
#include "lemonade/lib/model_inventory.h"
#include "lemonade/lib/static_translator.h"
#include <string>
#include <vector>

int main() {
  /// This is a workaround of sorts.
  lemonade::ModelInventory inventory;
  using LanguageDirection = std::pair<std::string, std::string>;
  std::vector<LanguageDirection> directions = {
      {"German", "English"},
      {"English", "German"},
      {"Czech", "English"},
  };

  std::vector<std::string> configFiles;
  for (auto &direction : directions) {
    auto modelInfo = inventory.query(direction.first, direction.second);
    if (!modelInfo.has_value()) {
      std::abort();
    }
    configFiles.push_back(inventory.configFile(*modelInfo));
  }

  // Reference code begins here.
  lemonade::StaticTranslator::Service::Config config;
  config.numWorkers = 40;
  lemonade::StaticTranslator translator(config, directions, configFiles);

  crow::SimpleApp app;
  CROW_ROUTE(app, "/translate")
  ([&translator](const crow::request &request) {
    using namespace lemonade;
    using Response = marian::bergamot::Response;
    Payload payload;
    fromJSON<Payload>(request.body, payload);
    std::optional<Response> maybeResponse =
        translator.translate({payload.source, payload.target}, payload.query);
    if (maybeResponse.has_value()) {
      return crow::response(toJSON<Response>(*maybeResponse));
    } else {
      return crow::response(400);
    }
  });

  app.port(18080).run();
}
