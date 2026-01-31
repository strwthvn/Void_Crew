#include <cmath>
#include <thread>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "timer.hpp"

using namespace void_crew;
using Catch::Matchers::WithinAbs;

TEST_CASE("Timer: starts near zero", "[common][timer]") {
    Timer t;
    REQUIRE_THAT(t.elapsedSeconds(), WithinAbs(0.0, 0.01));
}

TEST_CASE("Timer: elapsed increases over time", "[common][timer]") {
    Timer t;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    REQUIRE(t.elapsedSeconds() >= 0.03);
}

TEST_CASE("Timer: elapsedMilliseconds matches elapsedSeconds * 1000", "[common][timer]") {
    Timer t;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    double sec = t.elapsedSeconds();
    double ms = t.elapsedMilliseconds();
    REQUIRE_THAT(ms, WithinAbs(sec * 1000.0, 5.0));
}

TEST_CASE("Timer: reset brings elapsed back near zero", "[common][timer]") {
    Timer t;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    REQUIRE(t.elapsedSeconds() >= 0.03);

    t.reset();
    REQUIRE_THAT(t.elapsedSeconds(), WithinAbs(0.0, 0.01));
}

TEST_CASE("Timer: restart returns elapsed and resets", "[common][timer]") {
    Timer t;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    double returned = t.restart();
    REQUIRE(returned >= 0.03);
    REQUIRE_THAT(t.elapsedSeconds(), WithinAbs(0.0, 0.01));
}

TEST_CASE("Timer: multiple restarts produce sequential intervals", "[common][timer]") {
    Timer t;

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    double first = t.restart();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    double second = t.restart();

    REQUIRE(first >= 0.03);
    REQUIRE(second >= 0.03);
}

TEST_CASE("Timer: startTime advances after reset", "[common][timer]") {
    Timer t;
    auto firstStart = t.startTime();

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    t.reset();
    auto secondStart = t.startTime();

    REQUIRE(secondStart > firstStart);
}
