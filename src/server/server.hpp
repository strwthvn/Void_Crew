#pragma once

#include <atomic>

#include <entt/entt.hpp>

#include "server_config.hpp"

namespace void_crew::server {

class Server {
public:
    explicit Server(ServerConfig config);
    ~Server();

    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;

    void run();
    void shutdown();

    bool isRunning() const;
    entt::registry &registry();
    const ServerConfig &config() const;

private:
    ServerConfig m_config;
    std::atomic<bool> m_running{false};
    entt::registry m_registry;
};

} // namespace void_crew::server
