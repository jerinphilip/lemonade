#include "utils.h"
#include <bits/types/time_t.h> // for time_t
#include <ctime>               // for localtime, time

namespace lemonade {

std::string currentTime() {
  // https://stackoverflow.com/a/16358264/4565794
  time_t rawtime;
  struct tm *timeinfo;
  char buffer[80];

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
  return std::string(buffer);
}

std::string home() {
  const char *envHome = std::getenv("HOME");
  return std::string(envHome);
}

std::string modelsJSON() {
  return fmt::format("{}/.config/lemonade/models.json", home());
}

std::string modelsDir() { return fmt::format("{}/.lemonade/models", home()); }

} // namespace lemonade
