#include <cstdlib>

#include <spdlog/spdlog.h>

#include "command_line.hpp"
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

        spdlog::info("Void Crew Dedicated Server {}", void_crew::engineVersion());
        void_crew::server::installSignalHandlers();

        auto config = void_crew::server::loadConfig(*args);
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
