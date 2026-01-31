#include <cstdlib>

#include <spdlog/spdlog.h>

#include "command_line.hpp"
#include "logging.hpp"
#include "server.hpp"
#include "server_config.hpp"
#include "signal_handler.hpp"
#include "version.hpp"

int main(int argc, char *argv[]) {
    try {
        auto args = void_crew::server::parseCommandLine(argc, argv);
        if (!args) {
            return EXIT_SUCCESS;
        }

        void_crew::initLogging("info", "logs/server.log");
        spdlog::info("Void Crew Dedicated Server {}", void_crew::engineVersion());
        void_crew::server::installSignalHandlers();

        auto config = void_crew::server::loadConfig(*args);
        spdlog::set_level(spdlog::level::from_str(config.logLevel));

        void_crew::server::Server server(std::move(config));
        server.run();

        spdlog::info("Server shut down cleanly");
        return EXIT_SUCCESS;
    } catch (const std::exception &e) {
        spdlog::critical("Fatal error: {}", e.what());
        return EXIT_FAILURE;
    } catch (...) {
        spdlog::critical("Fatal error: unknown exception");
        return EXIT_FAILURE;
    }
}
