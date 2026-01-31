#pragma once

#include <atomic>

#include <entt/entt.hpp>

#include "command_line.hpp"

namespace void_crew::server {

class Server {
public:
    explicit Server(CommandLineArgs args);
    ~Server();

    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;

    void run();
    void shutdown();

    bool isRunning() const;
    entt::registry &registry();
    const CommandLineArgs &args() const;

private:
    CommandLineArgs m_args;
    std::atomic<bool> m_running{false};
    entt::registry m_registry;
};

} // namespace void_crew::server
