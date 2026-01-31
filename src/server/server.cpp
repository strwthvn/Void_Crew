#include "server.hpp"

#include <spdlog/spdlog.h>

#include "signal_handler.hpp"

namespace void_crew::server {

Server::Server(CommandLineArgs args) : m_args(std::move(args)) {
    spdlog::info("Server initialized on port {}", m_args.port);
}

Server::~Server() {
    if (m_running.load(std::memory_order_relaxed)) {
        shutdown();
    }
}

void Server::run() {
    m_running.store(true, std::memory_order_relaxed);
    spdlog::info("Server started");

    // TODO(#0): game loop (1.3)
    while (m_running.load(std::memory_order_relaxed) && !wasSignalReceived()) {
        break; // placeholder until game loop is implemented
    }

    m_running.store(false, std::memory_order_relaxed);

    if (wasSignalReceived()) {
        spdlog::info("Received shutdown signal");
    }
    spdlog::info("Server stopped");
}

void Server::shutdown() {
    spdlog::info("Server shutting down...");
    m_running.store(false, std::memory_order_relaxed);
}

bool Server::isRunning() const {
    return m_running.load(std::memory_order_relaxed);
}

entt::registry &Server::registry() {
    return m_registry;
}

const CommandLineArgs &Server::args() const {
    return m_args;
}

} // namespace void_crew::server
