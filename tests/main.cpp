#include "version.hpp"

#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <entt/entt.hpp>
#include <flatbuffers/flatbuffers.h>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <spdlog/spdlog.h>

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
