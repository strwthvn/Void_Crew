#include "signal_handler.hpp"

#include <atomic>
#include <csignal>

namespace void_crew::server {

namespace {

std::atomic<bool> s_signalReceived{false};
static_assert(std::atomic<bool>::is_always_lock_free, "atomic<bool> must be lock-free for signal handler");

void handleSignal(int /*signal*/) {
    s_signalReceived.store(true, std::memory_order_release);
}

} // namespace

void installSignalHandlers() {
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);
}

bool wasSignalReceived() {
    return s_signalReceived.load(std::memory_order_acquire);
}

} // namespace void_crew::server
