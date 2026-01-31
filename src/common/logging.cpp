#include "logging.hpp"

#include <string>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace void_crew {

constexpr std::size_t LOG_MAX_FILE_SIZE = 5 * 1024 * 1024; // 5 MB
constexpr std::size_t LOG_MAX_FILES = 3;

void initLogging(std::string_view level, const std::filesystem::path& logFile) {
    auto logDir = logFile.parent_path();
    if (!logDir.empty()) {
        std::filesystem::create_directories(logDir);
    }

    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        logFile.string(), LOG_MAX_FILE_SIZE, LOG_MAX_FILES);

    auto logger = std::make_shared<spdlog::logger>(
        "void_crew",
        spdlog::sinks_init_list{consoleSink, fileSink});

    // [2024-01-15 14:30:22.123456] [info] [void_crew] message
    // %f = microseconds. %n = logger name. %^ / %$ color markers only affect the console sink.
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%^%l%$] [%n] %v");
    logger->set_level(spdlog::level::from_str(std::string(level)));
    logger->flush_on(spdlog::level::warn);

    spdlog::set_default_logger(std::move(logger));
}

std::shared_ptr<spdlog::logger> getLogger(std::string_view tag) {
    std::string name(tag);
    auto existing = spdlog::get(name);
    if (existing) {
        return existing;
    }

    auto def = spdlog::default_logger();
    auto& sinks = def->sinks();
    auto logger = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
    logger->set_level(def->level());
    logger->flush_on(def->flush_level());

    // Another thread might have registered the same name between get() and here.
    try {
        spdlog::register_logger(logger);
    } catch (const spdlog::spdlog_ex&) {
        return spdlog::get(name);
    }
    return logger;
}

} // namespace void_crew
