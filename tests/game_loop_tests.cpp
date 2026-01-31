#include <atomic>
#include <cmath>
#include <thread>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "game_loop.hpp"

using namespace void_crew::server;
using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;

// --- Construction ---

TEST_CASE("GameLoop: fixedDt matches tick rate", "[server][loop]") {
    GameLoop loop(60);
    REQUIRE_THAT(loop.fixedDt(), WithinRel(1.0f / 60.0f, 0.001f));
}

TEST_CASE("GameLoop: fixedDt at 30 Hz", "[server][loop]") {
    GameLoop loop(30);
    REQUIRE_THAT(loop.fixedDt(), WithinRel(1.0f / 30.0f, 0.001f));
}

TEST_CASE("GameLoop: tick rate is clamped to valid range", "[server][loop]") {
    GameLoop loopZero(0);
    REQUIRE(loopZero.fixedDt() > 0.0f);
    REQUIRE(loopZero.fixedDt() <= 1.0f);

    GameLoop loopHuge(10000);
    REQUIRE(loopHuge.fixedDt() >= 1.0f / 300.0f);
}

TEST_CASE("GameLoop: currentTick starts at zero", "[server][loop]") {
    GameLoop loop(60);
    REQUIRE(loop.currentTick() == 0);
}

// --- Fixed dt passed to callback ---

TEST_CASE("GameLoop: callback receives constant dt", "[server][loop]") {
    constexpr uint32_t TICK_RATE = 60;
    constexpr int TARGET_TICKS = 10;
    GameLoop loop(TICK_RATE);

    const float expectedDt = 1.0f / static_cast<float>(TICK_RATE);
    std::vector<float> receivedDts;
    int tickCount = 0;

    loop.run(
        [&]() { return tickCount < TARGET_TICKS; },
        [&](float dt) {
            receivedDts.push_back(dt);
            tickCount++;
        });

    REQUIRE(receivedDts.size() == TARGET_TICKS);
    for (float dt : receivedDts) {
        REQUIRE_THAT(dt, WithinRel(expectedDt, 0.001f));
    }
}

// --- Tick counting ---

TEST_CASE("GameLoop: currentTick matches number of ticks executed", "[server][loop]") {
    constexpr int TARGET_TICKS = 20;
    GameLoop loop(60);
    int callbackCount = 0;

    loop.run(
        [&]() { return callbackCount < TARGET_TICKS; },
        [&](float) { callbackCount++; });

    REQUIRE(loop.currentTick() == TARGET_TICKS);
    REQUIRE(callbackCount == TARGET_TICKS);
}

// --- Metrics ---

TEST_CASE("GameLoop: metrics are populated after ticks", "[server][loop]") {
    GameLoop loop(60);
    int ticks = 0;

    loop.run(
        [&]() { return ticks < 50; },
        [&](float) { ticks++; });

    const auto& m = loop.metrics();
    REQUIRE(m.totalTicks == 50);
    REQUIRE(m.lastTickDuration >= 0.0);
    REQUIRE(m.averageTickDuration >= 0.0);
    REQUIRE(m.load >= 0.0);
}

TEST_CASE("GameLoop: metrics load is reasonable for trivial ticks", "[server][loop]") {
    GameLoop loop(60);
    int ticks = 0;

    loop.run(
        [&]() { return ticks < 100; },
        [&](float) { ticks++; });

    // Trivial (no-op) ticks should have very low load
    REQUIRE(loop.metrics().load < 50.0);
}

// --- Shutdown ---

TEST_CASE("GameLoop: stops when shouldRun returns false", "[server][loop]") {
    GameLoop loop(1000); // High rate so we don't sleep long
    std::atomic<bool> running{true};
    int ticks = 0;

    // Stop after 5 ticks
    loop.run(
        [&]() { return running.load(); },
        [&](float) {
            ticks++;
            if (ticks >= 5) {
                running.store(false);
            }
        });

    REQUIRE(ticks == 5);
    REQUIRE(loop.currentTick() == 5);
}

TEST_CASE("GameLoop: stops immediately when shouldRun starts false", "[server][loop]") {
    GameLoop loop(60);
    int ticks = 0;

    loop.run(
        [&]() { return false; },
        [&](float) { ticks++; });

    REQUIRE(ticks == 0);
    REQUIRE(loop.currentTick() == 0);
}

// --- Approximate real-time tick count ---

TEST_CASE("GameLoop: tick count approximates real time", "[server][loop]") {
    constexpr uint32_t TICK_RATE = 60;
    constexpr auto RUN_DURATION = std::chrono::milliseconds(200);
    GameLoop loop(TICK_RATE);

    auto start = std::chrono::steady_clock::now();
    loop.run(
        [&]() {
            return (std::chrono::steady_clock::now() - start) < RUN_DURATION;
        },
        [](float) {});

    // Expect roughly tickRate * 0.2 = 12 ticks, but allow generous tolerance
    // due to OS scheduling variance. The important thing is that it's not 0
    // and not wildly off (e.g. 1000).
    uint64_t expected = TICK_RATE * 200 / 1000; // 12
    REQUIRE(loop.currentTick() >= expected / 2);
    REQUIRE(loop.currentTick() <= expected * 3);
}
