#pragma once

namespace void_crew::server {

// Installs handlers for SIGINT and SIGTERM.
// Must be called once, from the main thread, before Server::run().
void installSignalHandlers();

// Returns true if a shutdown signal was received. Async-signal-safe.
bool wasSignalReceived();

} // namespace void_crew::server
