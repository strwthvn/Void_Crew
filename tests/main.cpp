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
    REQUIRE_NOTHROW(LOG_INFO("Catch2 test: spdlog {}.{}.{}",
        SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH));
}

// Helper: restore a plain default logger so subsequent tests can still log
static void resetDefaultLogger() {
    spdlog::drop_all();
    spdlog::set_default_logger(
        std::make_shared<spdlog::logger>("default",
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>()));
}

TEST_CASE("initLogging creates log file and writes to it", "[logging]") {
    auto logDir = std::filesystem::temp_directory_path() / "void_crew_test_write";
    auto logFile = logDir / "test.log";
    std::error_code ec;
    std::filesystem::remove_all(logDir, ec);

    REQUIRE_NOTHROW(void_crew::initLogging("debug", logFile));
    LOG_INFO("logging init test");
    spdlog::default_logger()->flush();

    REQUIRE(std::filesystem::exists(logFile));

    std::ifstream f(logFile);
    std::string contents((std::istreambuf_iterator<char>(f)),
                         std::istreambuf_iterator<char>());
    f.close();
    REQUIRE(contents.find("logging init test") != std::string::npos);

    resetDefaultLogger();
}

TEST_CASE("initLogging respects level filter", "[logging]") {
    auto logDir = std::filesystem::temp_directory_path() / "void_crew_test_level";
    auto logFile = logDir / "level.log";
    std::error_code ec;
    std::filesystem::remove_all(logDir, ec);

    void_crew::initLogging("warn", logFile);
    LOG_INFO("should be filtered");
    LOG_WARN("should appear");
    spdlog::default_logger()->flush();

    std::ifstream f(logFile);
    std::string contents((std::istreambuf_iterator<char>(f)),
                         std::istreambuf_iterator<char>());
    f.close();
    REQUIRE(contents.find("should be filtered") == std::string::npos);
    REQUIRE(contents.find("should appear") != std::string::npos);

    resetDefaultLogger();
}

TEST_CASE("LOG macros write through to file sink", "[logging]") {
    auto logDir = std::filesystem::temp_directory_path() / "void_crew_test_macros";
    auto logFile = logDir / "macros.log";
    std::error_code ec;
    std::filesystem::remove_all(logDir, ec);

    void_crew::initLogging("trace", logFile);
    LOG_TRACE("t_msg");
    LOG_DEBUG("d_msg");
    LOG_INFO("i_msg");
    LOG_WARN("w_msg");
    LOG_ERROR("e_msg");
    LOG_CRITICAL("c_msg");
    spdlog::default_logger()->flush();

    std::ifstream f(logFile);
    std::string contents((std::istreambuf_iterator<char>(f)),
                         std::istreambuf_iterator<char>());
    f.close();

    REQUIRE(contents.find("i_msg") != std::string::npos);
    REQUIRE(contents.find("w_msg") != std::string::npos);
    REQUIRE(contents.find("e_msg") != std::string::npos);
    REQUIRE(contents.find("c_msg") != std::string::npos);

    resetDefaultLogger();
}

TEST_CASE("TLOG macros include subsystem tag in output", "[logging]") {
    auto logDir = std::filesystem::temp_directory_path() / "void_crew_test_tag";
    auto logFile = logDir / "tag.log";
    std::error_code ec;
    std::filesystem::remove_all(logDir, ec);

    void_crew::initLogging("trace", logFile);
    TLOG_INFO("physics", "step done");
    TLOG_WARN("network", "high latency");
    spdlog::default_logger()->flush();
    void_crew::getLogger("physics")->flush();
    void_crew::getLogger("network")->flush();

    std::ifstream f(logFile);
    std::string contents((std::istreambuf_iterator<char>(f)),
                         std::istreambuf_iterator<char>());
    f.close();

    // Tag name appears in brackets before the message
    REQUIRE(contents.find("[physics]") != std::string::npos);
    REQUIRE(contents.find("step done") != std::string::npos);
    REQUIRE(contents.find("[network]") != std::string::npos);
    REQUIRE(contents.find("high latency") != std::string::npos);

    resetDefaultLogger();
}

TEST_CASE("getLogger returns same instance for same tag", "[logging]") {
    void_crew::initLogging();
    auto a = void_crew::getLogger("test_dedup");
    auto b = void_crew::getLogger("test_dedup");
    REQUIRE(a.get() == b.get());
    resetDefaultLogger();
}
