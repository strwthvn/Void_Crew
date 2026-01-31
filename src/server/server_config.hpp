#pragma once

#include <cstdint>
#include <string>

#include "command_line.hpp"

namespace void_crew::server {

constexpr uint32_t DEFAULT_MAX_PLAYERS = 12;
constexpr uint32_t DEFAULT_TICK_RATE = 60;

struct ServerConfig {
    std::string name = "Void Crew Server";
    uint16_t port = DEFAULT_PORT;
    uint32_t maxPlayers = DEFAULT_MAX_PLAYERS;
    uint32_t tickRate = DEFAULT_TICK_RATE;
    std::string logLevel = "info";
};

// Loads config from a TOML file, then applies CLI overrides.
// Missing file is not an error — defaults are used and a warning is logged.
// Throws std::runtime_error on parse errors.
ServerConfig loadConfig(const CommandLineArgs &args);

} // namespace void_crew::server
