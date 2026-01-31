#include "logging.hpp"
#include "version.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <entt/entt.hpp>
#include <flatbuffers/flatbuffers.h>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

struct Position {
    glm::vec3 value;
};

TEST_CASE("Engine version is not empty", "[common]") {
    std::string_view version = void_crew::engineVersion();
    REQUIRE_FALSE(version.empty());
}

TEST_CASE("EnTT registry with glm::vec3 component", "[ecs]") {
    entt::registry registry;
    auto entity = registry.create();
    registry.emplace<Position>(entity, glm::vec3{1.0f, 2.0f, 3.0f});

    auto* pos = registry.try_get<Position>(entity);
    REQUIRE(pos != nullptr);

    glm::vec3 expected{1.0f, 2.0f, 3.0f};
    REQUIRE(glm::all(glm::epsilonEqual(pos->value, expected, 1e-6f)));
}

TEST_CASE("glm::length computes correctly", "[math]") {
    glm::vec3 a{3.0f, 0.0f, 4.0f};
    REQUIRE(glm::epsilonEqual(glm::length(a), 5.0f, 1e-6f));
}

TEST_CASE("FlatBuffers build/read round-trip", "[serialization]") {
    flatbuffers::FlatBufferBuilder builder(256);
    auto nameOffset = builder.CreateString("VoidCrew");
    builder.Finish(nameOffset);

    auto* buf = builder.GetBufferPointer();
    REQUIRE(buf != nullptr);
    REQUIRE(builder.GetSize() > 0);

    auto* str = flatbuffers::GetRoot<flatbuffers::String>(buf);
    REQUIRE(str->str() == std::string("VoidCrew"));
}

TEST_CASE("spdlog formats without throwing", "[logging]") {
    REQUIRE_NOTHROW(spdlog::info("Catch2 test: spdlog {}.{}.{}",
        SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH));
}

TEST_CASE("initLogging creates log file and writes to it", "[logging]") {
    auto logDir = std::filesystem::temp_directory_path() / "void_crew_test_write";
    auto logFile = logDir / "test.log";
    std::error_code ec;
    std::filesystem::remove_all(logDir, ec);

    REQUIRE_NOTHROW(void_crew::initLogging("debug", logFile));
    spdlog::info("logging init test");
    spdlog::default_logger()->flush();

    REQUIRE(std::filesystem::exists(logFile));

    // Read file content while sink is still open (shared access on Windows)
    std::ifstream f(logFile);
    std::string contents((std::istreambuf_iterator<char>(f)),
                         std::istreambuf_iterator<char>());
    f.close();
    REQUIRE(contents.find("logging init test") != std::string::npos);

    // Restore a plain default logger so subsequent tests can still log
    spdlog::drop_all();
    spdlog::set_default_logger(
        std::make_shared<spdlog::logger>("default",
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>()));
    // Temp files left for OS cleanup
}

TEST_CASE("initLogging respects level filter", "[logging]") {
    auto logDir = std::filesystem::temp_directory_path() / "void_crew_test_level";
    auto logFile = logDir / "level.log";
    std::error_code ec;
    std::filesystem::remove_all(logDir, ec);

    void_crew::initLogging("warn", logFile);
    spdlog::info("should be filtered");
    spdlog::warn("should appear");
    spdlog::default_logger()->flush();

    std::ifstream f(logFile);
    std::string contents((std::istreambuf_iterator<char>(f)),
                         std::istreambuf_iterator<char>());
    f.close();
    REQUIRE(contents.find("should be filtered") == std::string::npos);
    REQUIRE(contents.find("should appear") != std::string::npos);

    spdlog::drop_all();
    spdlog::set_default_logger(
        std::make_shared<spdlog::logger>("default",
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>()));
}
