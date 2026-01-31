#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "command_line.hpp"
#include "server_config.hpp"
#include "signal_handler.hpp"

using namespace void_crew::server;

// Helper: convert string args to argc/argv
static std::optional<CommandLineArgs> parse(std::vector<std::string> tokens) {
    std::vector<char *> argv;
    for (auto &t : tokens) {
        argv.push_back(t.data());
    }
    return parseCommandLine(static_cast<int>(argv.size()), argv.data());
}

// --- parseCommandLine ---

TEST_CASE("parseCommandLine: no arguments yields defaults", "[server][cli]") {
    auto args = parse({"server"});
    REQUIRE(args.has_value());
    REQUIRE_FALSE(args->port.has_value());
    REQUIRE(args->configPath == DEFAULT_CONFIG_PATH);
}

TEST_CASE("parseCommandLine: --port sets port", "[server][cli]") {
    auto args = parse({"server", "--port", "9999"});
    REQUIRE(args.has_value());
    REQUIRE(args->port.has_value());
    REQUIRE(*args->port == 9999);
}

TEST_CASE("parseCommandLine: -p short flag works", "[server][cli]") {
    auto args = parse({"server", "-p", "443"});
    REQUIRE(args.has_value());
    REQUIRE(*args->port == 443);
}

TEST_CASE("parseCommandLine: --config sets path", "[server][cli]") {
    auto args = parse({"server", "--config", "custom.toml"});
    REQUIRE(args.has_value());
    REQUIRE(args->configPath == "custom.toml");
}

TEST_CASE("parseCommandLine: combined flags", "[server][cli]") {
    auto args = parse({"server", "-p", "8080", "-c", "other.toml"});
    REQUIRE(args.has_value());
    REQUIRE(*args->port == 8080);
    REQUIRE(args->configPath == "other.toml");
}

TEST_CASE("parseCommandLine: --help returns nullopt", "[server][cli]") {
    auto args = parse({"server", "--help"});
    REQUIRE_FALSE(args.has_value());
}

TEST_CASE("parseCommandLine: --version returns nullopt", "[server][cli]") {
    auto args = parse({"server", "--version"});
    REQUIRE_FALSE(args.has_value());
}

TEST_CASE("parseCommandLine: invalid port throws", "[server][cli]") {
    REQUIRE_THROWS_AS(parse({"server", "--port", "0"}), std::runtime_error);
    REQUIRE_THROWS_AS(parse({"server", "--port", "99999"}), std::runtime_error);
    REQUIRE_THROWS_AS(parse({"server", "--port", "abc"}), std::runtime_error);
}

TEST_CASE("parseCommandLine: missing port value throws", "[server][cli]") {
    REQUIRE_THROWS_AS(parse({"server", "--port"}), std::runtime_error);
}

TEST_CASE("parseCommandLine: unknown argument throws", "[server][cli]") {
    REQUIRE_THROWS_AS(parse({"server", "--bogus"}), std::runtime_error);
}

// --- loadConfig ---

class TempConfigFile {
public:
    explicit TempConfigFile(const std::string &content, const std::string &name = "test_config.toml")
        : m_path(std::filesystem::temp_directory_path() / name) {
        std::ofstream f(m_path);
        f << content;
    }
    ~TempConfigFile() { std::filesystem::remove(m_path); }
    std::string path() const { return m_path.string(); }

private:
    std::filesystem::path m_path;
};

TEST_CASE("loadConfig: missing file uses defaults", "[server][config]") {
    CommandLineArgs args;
    args.configPath = "nonexistent_12345.toml";
    auto cfg = loadConfig(args);
    REQUIRE(cfg.port == DEFAULT_PORT);
    REQUIRE(cfg.maxPlayers == DEFAULT_MAX_PLAYERS);
    REQUIRE(cfg.tickRate == DEFAULT_TICK_RATE);
    REQUIRE(cfg.name == "Void Crew Server");
    REQUIRE(cfg.logLevel == "info");
}

TEST_CASE("loadConfig: reads values from TOML", "[server][config]") {
    TempConfigFile file("[server]\nname = \"Test\"\nport = 9000\nmax_players = 4\ntick_rate = 30\n\n[logging]\nlevel = "
                        "\"debug\"\n");
    CommandLineArgs args;
    args.configPath = file.path();
    auto cfg = loadConfig(args);
    REQUIRE(cfg.name == "Test");
    REQUIRE(cfg.port == 9000);
    REQUIRE(cfg.maxPlayers == 4);
    REQUIRE(cfg.tickRate == 30);
    REQUIRE(cfg.logLevel == "debug");
}

TEST_CASE("loadConfig: partial TOML uses defaults for missing keys", "[server][config]") {
    TempConfigFile file("[server]\nname = \"Partial\"\n");
    CommandLineArgs args;
    args.configPath = file.path();
    auto cfg = loadConfig(args);
    REQUIRE(cfg.name == "Partial");
    REQUIRE(cfg.port == DEFAULT_PORT);
    REQUIRE(cfg.maxPlayers == DEFAULT_MAX_PLAYERS);
}

TEST_CASE("loadConfig: CLI --port overrides TOML", "[server][config]") {
    TempConfigFile file("[server]\nport = 30000\n");
    CommandLineArgs args;
    args.configPath = file.path();
    args.port = 5555;
    auto cfg = loadConfig(args);
    REQUIRE(cfg.port == 5555);
}

TEST_CASE("loadConfig: CLI --port 27015 overrides TOML", "[server][config]") {
    TempConfigFile file("[server]\nport = 30000\n");
    CommandLineArgs args;
    args.configPath = file.path();
    args.port = DEFAULT_PORT;
    auto cfg = loadConfig(args);
    REQUIRE(cfg.port == DEFAULT_PORT);
}

TEST_CASE("loadConfig: invalid TOML throws", "[server][config]") {
    TempConfigFile file("this is not [valid toml");
    CommandLineArgs args;
    args.configPath = file.path();
    REQUIRE_THROWS_AS(loadConfig(args), std::runtime_error);
}

// --- signal handler ---

TEST_CASE("wasSignalReceived: false before any signal", "[server][signal]") {
    // Note: can only test the initial state; sending real signals in tests is fragile
    // installSignalHandlers() is not called here to avoid side effects
    // The default-initialized atomic is false, verified by the function
    REQUIRE_FALSE(wasSignalReceived());
}
