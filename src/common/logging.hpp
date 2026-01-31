#pragma once

#include <filesystem>
#include <string_view>

namespace void_crew {

// Initializes spdlog with console (colored) and file sinks.
// Must be called once, early in main(), before any log calls.
// - level: one of "trace", "debug", "info", "warn", "error", "critical"
// - logFile: path to the log file; parent directories are created if needed
void initLogging(std::string_view level = "info",
                 const std::filesystem::path& logFile = "logs/void_crew.log");

} // namespace void_crew
