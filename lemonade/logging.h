#pragma once
#include "spdlog/spdlog.h"
#include <memory>

namespace lemonade {

static const char *kLoggerName = "ibus-engine-lemonade";

void setupLogging(const std::string &file_path = "");

std::shared_ptr<spdlog::logger> getLogger();

} // namespace lemonade
