#pragma once
#include "spdlog/spdlog.h"
#include <memory>

namespace lemonade {

class Logger {
public:
  using UnderlyingLogger = std::shared_ptr<spdlog::logger>;

  Logger(const std::string &name, const std::vector<std::string> &files = {},
         const std::string &level = "info");

  void log(const std::string &message, const std::string &level = "info");

  ~Logger();

private:
  bool setLoggingLevel(spdlog::logger &logger, std::string const level);

  const std::string name_;
  const std::string pattern_;
  const std::string level_;
  const std::vector<std::string> files_;
};

} // namespace lemonade
