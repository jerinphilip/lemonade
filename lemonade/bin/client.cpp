#include "3rd_party/CLI/CLI.hpp"
#include "lemonade/lib/translation_client.h"

int main(int argc, char **argv) {
  using App = CLI::App;
  App app;
  std::string url, source, target;
  app.add_option("-u,--url", url, "URL server is listening to")->required();
  app.add_option("-s,--source", source, "Source Language")->required();
  app.add_option("-t,--target", target, "Target Language")->required();
  app.parse(argc, argv);
  lemonade::TranslationClient client(url, source, target);
  client.run();
  return 0;
}
