#pragma once

#include <chrono>

namespace void_crew {

/// Stopwatch-style high-resolution timer backed by steady_clock.
///
/// Starts counting from construction. Not thread-safe; intended for
/// single-threaded use within a game loop or similar context.
class Timer {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    /// Construct a timer that starts counting from now.
    Timer() noexcept : m_start(Clock::now()) {}

    /// Seconds elapsed since construction or last reset/restart.
    double elapsedSeconds() const noexcept {
        return std::chrono::duration<double>(Clock::now() - m_start).count();
    }

    /// Milliseconds elapsed since construction or last reset/restart.
    double elapsedMilliseconds() const noexcept {
        return std::chrono::duration<double, std::milli>(Clock::now() - m_start).count();
    }

    /// Reset the timer to start counting from now.
    void reset() noexcept {
        m_start = Clock::now();
    }

    /// Return elapsed seconds and reset the timer in a single now() call.
    double restart() noexcept {
        auto now = Clock::now();
        double elapsed = std::chrono::duration<double>(now - m_start).count();
        m_start = now;
        return elapsed;
    }

    /// Raw access to the start time point.
    TimePoint startTime() const noexcept {
        return m_start;
    }

private:
    TimePoint m_start;
};

} // namespace void_crew
