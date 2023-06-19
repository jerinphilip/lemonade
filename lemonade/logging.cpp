#include "logging.h"
namespace lemonade {

void setupLogging(const std::string &file_path) {

  std::vector<spdlog::sink_ptr> sinks;

  if (file_path.empty()) {
    auto stderr_sink = spdlog::sinks::stderr_sink_mt::instance();
    sinks.push_back(stderr_sink);
  }

  else {
    auto file_sink =
        std::make_shared<spdlog::sinks::simple_file_sink_st>(file_path, true);
    sinks.push_back(file_sink);
  }

  auto logger =
      std::make_shared<spdlog::logger>(kLoggerName, begin(sinks), end(sinks));

  std::string pattern = fmt::format("[{}] {}", kLoggerName, "[%Y-%m-%d %T] %v");
  logger->set_pattern(pattern);

  spdlog::register_logger(logger);
}

std::shared_ptr<spdlog::logger> getLogger() {
  // Just fetch by name.
  std::shared_ptr<spdlog::logger> instance = spdlog::get(kLoggerName);
  assert(instance != nullptr);
  return instance;
}
} // namespace lemonade
