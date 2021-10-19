#include "3rd_party/CLI/CLI.hpp"
#include "lemonade/lib/translation_server.h"

int main(int argc, char **argv) {
  using App = CLI::App;
  size_t port, maxModels, numWorkers;
  App app;
  app.add_option("-p,--port", port, "Port to run websocket server on")
      ->required();
  app.add_option("-N,--max-models", maxModels,
                 "Maximum number of models to keep active at at a time")
      ->required();

  app.add_option("-w,--num-workers", numWorkers,
                 "Number of translation worker threads to spawn")
      ->required();

  app.parse(argc, argv);

  lemonade::TranslationServer server(port, maxModels, numWorkers);
  server.run();
  return 0;
}
