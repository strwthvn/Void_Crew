#include "command_line.hpp"

#include <charconv>
#include <stdexcept>
#include <string>
#include <string_view>

#include <fmt/format.h>

#include "version.hpp"

namespace void_crew::server {

namespace {

void printUsage(std::string_view programName) {
    fmt::print("Usage: {} [options]\n"
               "\n"
               "Options:\n"
               "  -p, --port <port>      Server port (default: {})\n"
               "  -c, --config <path>    Config file path (default: {})\n"
               "  -h, --help             Show this help message\n"
               "  -v, --version          Show version\n",
               programName,
               DEFAULT_PORT,
               DEFAULT_CONFIG_PATH);
}

void printVersion() {
    fmt::print("Void Crew Dedicated Server {}\n", void_crew::engineVersion());
}

uint16_t parsePort(std::string_view value) {
    uint32_t port = 0;
    auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), port);
    if (ec != std::errc{} || ptr != value.data() + value.size()) {
        throw std::runtime_error(fmt::format("invalid port number: '{}'", value));
    }
    if (port == 0 || port > 65535) {
        throw std::runtime_error(fmt::format("port must be in range 1-65535, got {}", port));
    }
    return static_cast<uint16_t>(port);
}

} // namespace

std::optional<CommandLineArgs> parseCommandLine(int argc, char *argv[]) {
    CommandLineArgs args;

    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return std::nullopt;
        }

        if (arg == "-v" || arg == "--version") {
            printVersion();
            return std::nullopt;
        }

        if (arg == "-p" || arg == "--port") {
            if (i + 1 >= argc) {
                throw std::runtime_error("--port requires a value");
            }
            args.port = parsePort(argv[++i]);
            continue;
        }

        if (arg == "-c" || arg == "--config") {
            if (i + 1 >= argc) {
                throw std::runtime_error("--config requires a value");
            }
            args.configPath = argv[++i];
            continue;
        }

        throw std::runtime_error(fmt::format("unknown argument: '{}'", arg));
    }

    return args;
}

} // namespace void_crew::server
