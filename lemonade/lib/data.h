#pragma once

#include <string>

namespace lemonade {

struct Payload {
  std::string source;
  std::string target;
  std::string query;
};

} // namespace lemonade
