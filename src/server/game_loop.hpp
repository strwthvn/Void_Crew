#pragma once

#include <cstdint>
#include <functional>

#include "timer.hpp"

namespace void_crew::server {

/// Performance metrics for the game loop, updated every tick.
struct TickMetrics {
    uint64_t totalTicks = 0;
    double lastTickDuration = 0.0;     // seconds
    double averageTickDuration = 0.0;  // exponential moving average, seconds
    double maxTickDuration = 0.0;      // seconds, reset each logging interval
    double load = 0.0;                 // avgTickDuration / dt * 100 (percentage)
};

/// Fixed-timestep game loop using the accumulator pattern.
///
/// The loop measures real elapsed time, accumulates it, and runs simulation
/// ticks at a fixed interval (1/tickRate seconds). Between ticks the server
/// thread sleeps to avoid busy-waiting.
///
/// Death-spiral protection: if real time exceeds MAX_FRAME_TIME per outer
/// iteration, the surplus is discarded so the simulation slows down instead
/// of spiralling into an ever-growing backlog.
///
/// Reference: Glenn Fiedler, "Fix Your Timestep!"
/// https://gafferongames.com/post/fix_your_timestep/
class GameLoop {
public:
    /// @param tickRate  Simulation ticks per second (clamped to [1, 300]).
    explicit GameLoop(uint32_t tickRate);

    /// Run the loop until @p shouldRun returns false.
    /// @p onTick is called once per fixed-step simulation tick with the
    /// constant delta time in seconds.
    void run(std::function<bool()> shouldRun, std::function<void(float)> onTick);

    uint64_t currentTick() const noexcept;
    float fixedDt() const noexcept;
    const TickMetrics& metrics() const noexcept;

private:
    void logMetrics();

    uint32_t m_tickRate;
    double m_dt;             // 1.0 / tickRate  (seconds, double for accumulator precision)
    uint64_t m_currentTick = 0;
    TickMetrics m_metrics;

    /// Maximum elapsed time accepted per outer-loop iteration (seconds).
    /// Anything above this is clamped, causing the simulation to slow down
    /// instead of running an unbounded number of catch-up ticks.
    static constexpr double MAX_FRAME_TIME = 0.25;

    /// How often to log performance metrics (seconds).
    static constexpr double METRICS_LOG_INTERVAL = 5.0;
};

} // namespace void_crew::server
