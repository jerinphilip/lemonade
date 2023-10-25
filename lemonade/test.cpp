#include "logging.h"
#include "translator.h"
#include <iostream>

template <class Translator> void repl() {
  std::cout << "Type in: "
            << "\n";
  std::cout << "   "
            << "<src_lang> <tgt_lang> <input> \n";

  struct Direction {
    std::string src;
    std::string tgt;
  };

  std::string input;
  Direction old;
  Direction current;
  size_t max_models = 1;
  size_t workers = 4;
  slimt::Config config;
  Translator translator(config);

  while (!std::cin.eof()) {
    std::cout << " $ ";
    std::cin >> current.src;
    std::cin >> current.tgt;
    std::getline(std::cin, input);
    if (current.src != old.src || current.tgt != old.tgt) {
      translator.set_direction(current.src, current.tgt);
      old = current;
    }
    auto translation = translator.translate(input);
    std::cout << translation << "\n";
    LOG("Direction %s -> %s: %s / %s", current.src.c_str(), current.tgt.c_str(),
        input.c_str(), translation.c_str());
  }
}

int main(int argc, char **argv) {
  std::string mode((argc == 2) ? argv[1] : "");
  if (mode == "fake") {
    repl<lemonade::FakeTranslator>();
  } else {
    std::string log_path = std::getenv("HOME");
    log_path += "/.ibus-engine-lemonade.log";
    lemonade::setup_logging(log_path);
    repl<lemonade::Translator>();
  }

  return 0;
}
