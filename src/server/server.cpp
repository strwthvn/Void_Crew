#include "server.hpp"

#include "logging.hpp"
#include "signal_handler.hpp"

namespace void_crew::server {

Server::Server(ServerConfig config) : m_config(std::move(config)), m_gameLoop(m_config.tickRate) {
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

    m_gameLoop.run([this]() { return m_running.load(std::memory_order_acquire) && !wasSignalReceived(); },
                   [this](float dt) { tick(dt); });

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

void Server::tick(float dt) {
    // TODO(#0): update ECS systems (1.5)
    static_cast<void>(dt);
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

const GameLoop &Server::gameLoop() const noexcept {
    return m_gameLoop;
}

} // namespace void_crew::server
