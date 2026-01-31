#include "signal_handler.hpp"

#include <atomic>
#include <csignal>

namespace void_crew::server {

namespace {

std::atomic<bool> s_signalReceived{false};

void handleSignal(int /*signal*/) {
    s_signalReceived.store(true, std::memory_order_relaxed);
}

} // namespace

void installSignalHandlers() {
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);
}

bool wasSignalReceived() {
    return s_signalReceived.load(std::memory_order_relaxed);
}

} // namespace void_crew::server
