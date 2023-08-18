#pragma once
#include <cstdio>
#include <memory>
#include <string>

namespace lemonade {

class Logger {
public:
  Logger(const std::string &name, const std::string &file_path = "");
  void set_log_path(const std::string &file_path);
  FILE *sink() { return sink_; }

  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;
  ~Logger();

private:
  std::string path_;
  std::string name_;
  FILE *sink_ = NULL;
  bool owns_ = false;
};

extern Logger LOGGER;

inline void setup_logging(const std::string log_path) {
  LOGGER.set_log_path(log_path);
}

#define LOG(...) fprintf(LOGGER.sink(), __VA_ARGS__)

} // namespace lemonade
