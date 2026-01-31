#include "server_config.hpp"

#include <filesystem>
#include <stdexcept>

#include <spdlog/spdlog.h>
#include <toml++/toml.hpp>

namespace void_crew::server {

namespace {

ServerConfig parseToml(const std::string &path) {
    auto tbl = toml::parse_file(path);
    ServerConfig cfg;

    if (auto server = tbl["server"].as_table()) {
        cfg.name = server->get("name")->value_or(cfg.name);
        cfg.port = static_cast<uint16_t>(server->get("port")->value_or(static_cast<int64_t>(cfg.port)));
        cfg.maxPlayers =
            static_cast<uint32_t>(server->get("max_players")->value_or(static_cast<int64_t>(cfg.maxPlayers)));
        cfg.tickRate = static_cast<uint32_t>(server->get("tick_rate")->value_or(static_cast<int64_t>(cfg.tickRate)));
    }

    if (auto logging = tbl["logging"].as_table()) {
        cfg.logLevel = logging->get("level")->value_or(cfg.logLevel);
    }

    return cfg;
}

} // namespace

ServerConfig loadConfig(const CommandLineArgs &args) {
    ServerConfig cfg;

    if (std::filesystem::exists(args.configPath)) {
        spdlog::info("Loading config from '{}'", args.configPath);
        try {
            cfg = parseToml(args.configPath);
        } catch (const toml::parse_error &e) {
            throw std::runtime_error(fmt::format("Failed to parse config '{}': {}", args.configPath, e.description()));
        }
    } else {
        spdlog::warn("Config file '{}' not found, using defaults", args.configPath);
    }

    // CLI --port overrides config file
    if (args.port != DEFAULT_PORT) {
        cfg.port = args.port;
    }

    return cfg;
}

} // namespace void_crew::server
