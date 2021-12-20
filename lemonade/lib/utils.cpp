#include "utils.h"
#include <ctime>               // for localtime, time

namespace lemonade {

std::string currentTime() {
  // https://stackoverflow.com/a/16358264/4565794
  std::time_t rawtime;
  struct tm *timeinfo;
  char buffer[80];

  std::time(&rawtime);
  timeinfo = std::localtime(&rawtime);

  strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
  return std::string(buffer);
}

} // namespace lemonade
