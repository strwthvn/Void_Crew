#include "logging.hpp"

#include <string>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace void_crew {

void initLogging(std::string_view level, const std::filesystem::path& logFile) {
    auto logDir = logFile.parent_path();
    if (!logDir.empty()) {
        std::filesystem::create_directories(logDir);
    }

    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
        logFile.string(), /*truncate=*/false);

    auto logger = std::make_shared<spdlog::logger>(
        "void_crew",
        spdlog::sinks_init_list{consoleSink, fileSink});

    // [2024-01-15 14:30:22.123] [info] message
    // %^ / %$ color markers only affect the console sink
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    logger->set_level(spdlog::level::from_str(std::string(level)));
    logger->flush_on(spdlog::level::warn);

    spdlog::set_default_logger(std::move(logger));
}

} // namespace void_crew
