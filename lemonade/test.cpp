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
  Direction old, current;
  size_t maxModels = 1, workers = 4;
  Translator translator;

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
  }
}

int main(int argc, char **argv) {
  std::string mode((argc == 2) ? argv[1] : "");
  if (mode == "fake") {
    repl<lemonade::FakeTranslator>();
  } else {
    repl<lemonade::Translator>();
  }

  return 0;
}
