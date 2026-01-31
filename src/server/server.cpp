#include "server.hpp"

#include <spdlog/spdlog.h>

#include "signal_handler.hpp"

namespace void_crew::server {

Server::Server(ServerConfig config) : m_config(std::move(config)) {
    spdlog::info("Server '{}' initialized on port {}", m_config.name, m_config.port);
    spdlog::info("Max players: {}, Tick rate: {} Hz", m_config.maxPlayers, m_config.tickRate);
}

Server::~Server() {
    if (m_running.load(std::memory_order_acquire)) {
        shutdown();
    }
}

void Server::run() {
    m_running.store(true, std::memory_order_release);
    spdlog::info("Server started");

    // TODO(#0): game loop (1.3)
    while (m_running.load(std::memory_order_acquire) && !wasSignalReceived()) {
        break; // placeholder until game loop is implemented
    }

    m_running.store(false, std::memory_order_release);

    if (wasSignalReceived()) {
        spdlog::info("Received shutdown signal");
    }
    spdlog::info("Server stopped");
}

void Server::shutdown() {
    spdlog::info("Server shutting down...");
    m_running.store(false, std::memory_order_release);
}

bool Server::isRunning() const {
    return m_running.load(std::memory_order_acquire);
}

entt::registry &Server::registry() {
    return m_registry;
}

const ServerConfig &Server::config() const {
    return m_config;
}

} // namespace void_crew::server
