#include "command_line.hpp"
#include "version.hpp"

#include <cstdlib>

#include <spdlog/spdlog.h>

int main(int argc, char* argv[]) {
    try {
        auto args = void_crew::server::parseCommandLine(argc, argv);
        if (!args) {
            return EXIT_SUCCESS;
        }

        spdlog::info("Void Crew Dedicated Server {}", void_crew::engineVersion());
        spdlog::info("Port: {}, Config: {}", args->port, args->configPath);

        // TODO(#0): create Server instance (1.1)
        // TODO(#0): run server main loop (1.3)

        spdlog::info("Server shut down cleanly");
        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        spdlog::critical("Fatal error: {}", e.what());
        return EXIT_FAILURE;
    } catch (...) {
        spdlog::critical("Fatal error: unknown exception");
        return EXIT_FAILURE;
    }
}
