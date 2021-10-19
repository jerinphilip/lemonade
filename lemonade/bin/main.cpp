#include <iostream>
#include <sstream>
#include <string>

#include "3rd_party/CLI/CLI.hpp"
#include "lemonade/lib/json_interop.h"
#include "lemonade/lib/translator.h"

std::string readFromStdin() {
  std::stringstream readStream;
  readStream << std::cin.rdbuf();
  std::string source = readStream.str();
  return source;
}

int main(int argc, char **argv) {
  using App = CLI::App;
  App app;
  std::string source, target;
  app.add_option("-s,--source", source, "Source language")->required();
  app.add_option("-t,--target", target, "Target language")->required();
  app.parse(argc, argv);
  lemonade::Translator translator(/*maxModels=*/4, /*numWorkers=*/40);
  std::string input = readFromStdin();
  using Response = marian::bergamot::Response;
  Response response = translator.btranslate(std::move(input), source, target);

  std::cout << lemonade::toJSON<Response>(response) << std::endl;
  return 0;
}
