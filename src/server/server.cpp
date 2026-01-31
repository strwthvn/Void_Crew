#include "server.hpp"

#include "logging.hpp"
#include "signal_handler.hpp"

namespace void_crew::server {

Server::Server(ServerConfig config) : m_config(std::move(config)) {
    TLOG_INFO("server", "Server '{}' initialized on port {}", m_config.name, m_config.port);
    TLOG_INFO("server", "Max players: {}, Tick rate: {} Hz", m_config.maxPlayers, m_config.tickRate);
}

Server::~Server() {
    if (m_running.load(std::memory_order_acquire)) {
        shutdown();
    }
}

void Server::run() {
    m_running.store(true, std::memory_order_release);
    TLOG_INFO("server", "Server started");

    // TODO(#0): game loop (1.3)
    while (m_running.load(std::memory_order_acquire) && !wasSignalReceived()) {
        break; // placeholder until game loop is implemented
    }

    m_running.store(false, std::memory_order_release);

    if (wasSignalReceived()) {
        TLOG_INFO("server", "Received shutdown signal");
    }
    TLOG_INFO("server", "Server stopped");
}

void Server::shutdown() {
    TLOG_INFO("server", "Server shutting down...");
    m_running.store(false, std::memory_order_release);
}

bool Server::isRunning() const noexcept {
    return m_running.load(std::memory_order_acquire);
}

entt::registry &Server::registry() noexcept {
    return m_registry;
}

const ServerConfig &Server::config() const noexcept {
    return m_config;
}

} // namespace void_crew::server
