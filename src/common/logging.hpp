#pragma once

#include <filesystem>
#include <memory>
#include <string_view>

// SPDLOG_ACTIVE_LEVEL is set via CMake (TRACE in Debug, INFO in Release).
// It must be defined before including spdlog — the compile definition handles this.
#include <spdlog/spdlog.h>

namespace void_crew {

// Initializes spdlog with console (colored) and file sinks.
// Must be called once, early in main(), before any log calls.
// - level: one of "trace", "debug", "info", "warn", "error", "critical"
// - logFile: path to the log file; parent directories are created if needed
void initLogging(std::string_view level = "info",
                 const std::filesystem::path& logFile = "logs/void_crew.log");

// Returns a logger tagged with the given subsystem name.
// Creates it on first call for a given tag, sharing sinks with the root logger.
// Thread-safe. Subsequent calls with the same tag return the cached logger.
std::shared_ptr<spdlog::logger> getLogger(std::string_view tag);

} // namespace void_crew

// ---------------------------------------------------------------------------
// Untagged macros — log via the default ("void_crew") logger.
// ---------------------------------------------------------------------------
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOG_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

// ---------------------------------------------------------------------------
// Tagged macros — log via a subsystem-specific logger.
//   TLOG_INFO("config", "Loaded '{}'", path);
//   -> [2024-01-15 14:30:22.123] [info] [config] Loaded 'server.toml'
// For hot paths, cache the logger in a local static instead:
//   static auto s_log = void_crew::getLogger("physics");
//   SPDLOG_LOGGER_INFO(s_log, "tick");
// ---------------------------------------------------------------------------
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TLOG_TRACE(tag, ...) SPDLOG_LOGGER_TRACE(::void_crew::getLogger(tag), __VA_ARGS__)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TLOG_DEBUG(tag, ...) SPDLOG_LOGGER_DEBUG(::void_crew::getLogger(tag), __VA_ARGS__)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TLOG_INFO(tag, ...) SPDLOG_LOGGER_INFO(::void_crew::getLogger(tag), __VA_ARGS__)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TLOG_WARN(tag, ...) SPDLOG_LOGGER_WARN(::void_crew::getLogger(tag), __VA_ARGS__)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TLOG_ERROR(tag, ...) SPDLOG_LOGGER_ERROR(::void_crew::getLogger(tag), __VA_ARGS__)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TLOG_CRITICAL(tag, ...) SPDLOG_LOGGER_CRITICAL(::void_crew::getLogger(tag), __VA_ARGS__)
