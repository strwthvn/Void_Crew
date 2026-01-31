#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace void_crew::server {

constexpr uint16_t DEFAULT_PORT = 27015;
constexpr const char *DEFAULT_CONFIG_PATH = "server.toml";

struct CommandLineArgs {
    std::optional<uint16_t> port;
    std::string configPath = DEFAULT_CONFIG_PATH;
};

// Parses argc/argv into CommandLineArgs.
// Returns nullopt when the program should exit immediately (--help, --version).
// Throws std::runtime_error on invalid arguments.
std::optional<CommandLineArgs> parseCommandLine(int argc, char *argv[]);

} // namespace void_crew::server
