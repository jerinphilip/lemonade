#include "logging.h"
#include "spdlog/spdlog.h"
#include "utils.h"
#include <iostream>
#include <memory>

namespace lemonade {

Logger::Logger(const std::string &name,
               const std::vector<std::string> &files /*={}*/,
               const std::string &level /*=info*/)
    : name_(name), files_(files), level_(level),
      pattern_(fmt::format("[{}] {}", name, "[%Y-%m-%d %T] %v")) {

  UnderlyingLogger maybeLogger = spdlog::get(name_);
  if (maybeLogger) {
    std::cerr << fmt::format("Logger with name {} already exists.", name_)
              << std::endl;
    std::abort();
  }

  // Logging always happens in stderr.
  std::vector<spdlog::sink_ptr> sinks;
  auto stderr_sink = spdlog::sinks::stderr_sink_mt::instance();
  sinks.push_back(stderr_sink);

  // If files are given, we have a few more sinks.
  for (auto &&file : files) {
    auto file_sink =
        std::make_shared<spdlog::sinks::simple_file_sink_st>(file, true);
    sinks.push_back(file_sink);
  }

  auto logger =
      std::make_shared<spdlog::logger>(name, begin(sinks), end(sinks));

  spdlog::register_logger(logger);
  logger->set_pattern(pattern_);

  setLoggingLevel(*logger, level);
}

Logger::~Logger() {
  if (spdlog::get(name_)) {
    spdlog::drop(name_);
  }
}

bool Logger::setLoggingLevel(spdlog::logger &logger, std::string const level) {
  if (level == "trace")
    logger.set_level(spdlog::level::trace);
  else if (level == "debug")
    logger.set_level(spdlog::level::debug);
  else if (level == "info")
    logger.set_level(spdlog::level::info);
  else if (level == "warn")
    logger.set_level(spdlog::level::warn);
  else if (level == "err" || level == "error")
    logger.set_level(spdlog::level::err);
  else if (level == "critical")
    logger.set_level(spdlog::level::critical);
  else if (level == "off")
    logger.set_level(spdlog::level::off);
  else {
    logger.warn("Unknown log level '{}' for logger '{}'", level.c_str(),
                logger.name().c_str());
    return false;
  }
  return true;
}

void Logger::log(const std::string &message,
                 const std::string &level /*= "info"*/) {
#if 0

  std::cerr << fmt::format("[{}] [{}] {}", currentTime(), level, message)
            << std::endl;

#else

  UnderlyingLogger logger = spdlog::get(name_);
  if (!logger) {
    std::cerr << "Fatal. Logger not found. ";
    std::abort();
  }

  if (level == "trace")
    logger->trace(message);
  else if (level == "debug")
    logger->debug(message);
  else if (level == "info")
    logger->info(message);
  else if (level == "warn")
    logger->warn(message);
  else if (level == "error")
    logger->error();
  else if (level == "critical")
    logger->critical(message);
  else {
    logger->warn("Unknown log level '{}' for logger '{}'", level, name_);
  }
#endif
}

} // namespace lemonade
