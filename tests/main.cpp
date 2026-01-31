#include "version.hpp"

#include <cstdlib>
#include <string_view>

#include <entt/entt.hpp>
#include <flatbuffers/flatbuffers.h>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <spdlog/spdlog.h>

struct Position {
    glm::vec3 value;
};

int main() {
    // Verify engine version is not empty
    std::string_view version = void_crew::engineVersion();
    if (version.empty()) {
        spdlog::error("engineVersion() returned empty string");
        return EXIT_FAILURE;
    }
    spdlog::info("PASS: engineVersion() = \"{}\"", version);

    // Verify EnTT registry with glm::vec3 component
    entt::registry registry;
    auto entity = registry.create();
    registry.emplace<Position>(entity, glm::vec3{1.0f, 2.0f, 3.0f});

    auto* pos = registry.try_get<Position>(entity);
    if (pos == nullptr) {
        spdlog::error("Position component not found on entity");
        return EXIT_FAILURE;
    }
    glm::vec3 expected{1.0f, 2.0f, 3.0f};
    if (!glm::all(glm::epsilonEqual(pos->value, expected, 1e-6f))) {
        spdlog::error("Position values mismatch");
        return EXIT_FAILURE;
    }
    spdlog::info("PASS: EnTT + glm vec3 component");

    // Verify glm operations
    glm::vec3 a{3.0f, 0.0f, 4.0f};
    float len = glm::length(a);
    if (!glm::epsilonEqual(len, 5.0f, 1e-6f)) {
        spdlog::error("glm::length mismatch: expected 5, got {}", len);
        return EXIT_FAILURE;
    }
    spdlog::info("PASS: glm::length");

    // Verify FlatBuffers: build a buffer with a string, read it back
    flatbuffers::FlatBufferBuilder builder(256);
    auto nameOffset = builder.CreateString("VoidCrew");
    builder.Finish(nameOffset);
    auto* buf = builder.GetBufferPointer();
    auto size = builder.GetSize();
    if (buf == nullptr || size == 0) {
        spdlog::error("FlatBufferBuilder produced empty buffer");
        return EXIT_FAILURE;
    }
    auto* str = flatbuffers::GetRoot<flatbuffers::String>(buf);
    if (str->str() != std::string("VoidCrew")) {
        spdlog::error("FlatBuffers round-trip mismatch");
        return EXIT_FAILURE;
    }
    spdlog::info("PASS: FlatBuffers build/read round-trip");

    // Verify spdlog formatted output
    spdlog::info("PASS: spdlog {}.{}.{}", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);

    return EXIT_SUCCESS;
}
