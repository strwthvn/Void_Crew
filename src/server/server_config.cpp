#include "server_config.hpp"

#include <filesystem>
#include <stdexcept>

#include <toml++/toml.hpp>

#include "logging.hpp"

namespace void_crew::server {

namespace {

ServerConfig parseToml(const std::string &path) {
    auto tbl = toml::parse_file(path);
    ServerConfig cfg;

    if (auto server = tbl["server"].as_table()) {
        cfg.name = (*server)["name"].value_or(cfg.name);
        cfg.port = static_cast<uint16_t>((*server)["port"].value_or(static_cast<int64_t>(cfg.port)));
        cfg.maxPlayers = static_cast<uint32_t>((*server)["max_players"].value_or(static_cast<int64_t>(cfg.maxPlayers)));
        cfg.tickRate = static_cast<uint32_t>((*server)["tick_rate"].value_or(static_cast<int64_t>(cfg.tickRate)));
    }

    if (auto logging = tbl["logging"].as_table()) {
        cfg.logLevel = (*logging)["level"].value_or(cfg.logLevel);
    }

    return cfg;
}

} // namespace

ServerConfig loadConfig(const CommandLineArgs &args) {
    ServerConfig cfg;

    if (std::filesystem::exists(args.configPath)) {
        TLOG_INFO("config", "Loading config from '{}'", args.configPath);
        try {
            cfg = parseToml(args.configPath);
        } catch (const toml::parse_error &e) {
            throw std::runtime_error(fmt::format("Failed to parse config '{}': {}", args.configPath, e.description()));
        }
    } else {
        TLOG_WARN("config", "Config file '{}' not found, using defaults", args.configPath);
    }

    // CLI --port overrides config file
    if (args.port.has_value()) {
        cfg.port = *args.port;
    }

    return cfg;
}

} // namespace void_crew::server
