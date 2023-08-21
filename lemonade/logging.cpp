#include "logging.h"
#include <cstdio>

namespace lemonade {

Logger::Logger(const std::string &name, const std::string &file_path)
    : name_(name), path_(file_path) {
  if (file_path.empty()) {
    sink_ = stderr;
  } else {
    set_log_path(file_path);
  }
}

Logger::~Logger() {
  if (owns_) {
    owns_ = false;
    fclose(sink_);
  }
}

void Logger::set_log_path(const std::string &file_path) {
  owns_ = true;
  path_ = file_path;
  sink_ = fopen(path_.c_str(), "a+");
}

} // namespace lemonade
