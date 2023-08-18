#include "logging.h"
#include "translator.h"
#include <iostream>

template <class Translator> void repl() {
  std::cout << "Type in: "
            << "\n";
  std::cout << "   "
            << "<src_lang> <tgt_lang> <input> \n";
  std::string src_lang, tgt_lang, input;
  size_t maxModels = 1, workers = 4;
  Translator translator(maxModels, workers);

  while (!std::cin.eof()) {
    std::cout << " $ ";
    std::cin >> src_lang;
    std::cin >> tgt_lang;
    std::getline(std::cin, input);
    auto translation = translator.translate(input, src_lang, tgt_lang);
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
