#include "game_loop.hpp"

#include <algorithm>
#include <thread>

#include "logging.hpp"

namespace void_crew::server {

namespace {

constexpr uint32_t MIN_TICK_RATE = 1;
constexpr uint32_t MAX_TICK_RATE = 300;
constexpr double EMA_ALPHA = 0.1;

} // namespace

GameLoop::GameLoop(uint32_t tickRate)
    : m_tickRate(std::clamp(tickRate, MIN_TICK_RATE, MAX_TICK_RATE)), m_dt(1.0 / static_cast<double>(m_tickRate)) {
    if (tickRate != m_tickRate) {
        TLOG_WARN("loop", "Tick rate {} clamped to {}", tickRate, m_tickRate);
    }
    TLOG_DEBUG("loop", "Game loop configured: {} Hz, dt = {:.6f}s", m_tickRate, m_dt);
}

void GameLoop::run(std::function<bool()> shouldRun, std::function<void(float)> onTick) {
    TLOG_INFO("loop", "Game loop started at {} Hz", m_tickRate);

    Timer frameTimer;
    double accumulator = 0.0;
    double timeSinceMetricsLog = 0.0;
    const auto fixedDtFloat = static_cast<float>(m_dt);

    while (shouldRun()) {
        double elapsed = frameTimer.restart();

        // Death-spiral protection: clamp elapsed time so we don't try to
        // run an unbounded number of catch-up ticks after a long stall.
        if (elapsed > MAX_FRAME_TIME) {
            TLOG_WARN("loop", "Frame time {:.3f}s exceeds limit, clamped to {:.3f}s", elapsed, MAX_FRAME_TIME);
            elapsed = MAX_FRAME_TIME;
        }

        accumulator += elapsed;
        timeSinceMetricsLog += elapsed;

        // Also check shouldRun between ticks for responsive shutdown.
        // Without this, a signal or shutdown() call during the inner loop
        // would only take effect after all accumulated ticks are drained.
        while (accumulator >= m_dt && shouldRun()) {
            Timer tickTimer;

            onTick(fixedDtFloat);
            m_currentTick++;

            double tickDuration = tickTimer.elapsedSeconds();

            // Update metrics
            m_metrics.totalTicks = m_currentTick;
            m_metrics.lastTickDuration = tickDuration;
            m_metrics.maxTickDuration = std::max(m_metrics.maxTickDuration, tickDuration);

            if (m_currentTick == 1) {
                m_metrics.averageTickDuration = tickDuration;
            } else {
                m_metrics.averageTickDuration =
                    EMA_ALPHA * tickDuration + (1.0 - EMA_ALPHA) * m_metrics.averageTickDuration;
            }
            m_metrics.load = (m_metrics.averageTickDuration / m_dt) * 100.0;

            accumulator -= m_dt;
        }

        // Log metrics periodically
        if (timeSinceMetricsLog >= METRICS_LOG_INTERVAL) {
            logMetrics();
            timeSinceMetricsLog = 0.0;
            m_metrics.maxTickDuration = 0.0;
        }

        // Sleep for the remaining time before the next tick is due.
        // The accumulator holds the leftover time that didn't fill a full dt,
        // so we need to wait (dt - accumulator) before the next tick fires.
        double remainingSec = m_dt - accumulator;
        if (remainingSec > 0.001) {
            std::this_thread::sleep_for(std::chrono::duration<double>(remainingSec));
        }
    }

    TLOG_INFO("loop", "Game loop stopped after {} ticks", m_currentTick);
}

uint64_t GameLoop::currentTick() const noexcept {
    return m_currentTick;
}

float GameLoop::fixedDt() const noexcept {
    return static_cast<float>(m_dt);
}

const TickMetrics &GameLoop::metrics() const noexcept {
    return m_metrics;
}

void GameLoop::logMetrics() {
    TLOG_DEBUG("loop",
               "tick={} avg={:.3f}ms max={:.3f}ms load={:.1f}%",
               m_currentTick,
               m_metrics.averageTickDuration * 1000.0,
               m_metrics.maxTickDuration * 1000.0,
               m_metrics.load);
}

} // namespace void_crew::server
