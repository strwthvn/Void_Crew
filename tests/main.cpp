#include "version.hpp"

#include <cstdlib>
#include <string_view>

#include <entt/entt.hpp>
#include <spdlog/spdlog.h>

struct Position {
    float x;
    float y;
    float z;
};

int main() {
    // Verify engine version is not empty
    std::string_view version = void_crew::engineVersion();
    if (version.empty()) {
        spdlog::error("engineVersion() returned empty string");
        return EXIT_FAILURE;
    }
    spdlog::info("PASS: engineVersion() = \"{}\"", version);

    // Verify EnTT registry: create entity, attach and read component
    entt::registry registry;
    auto entity = registry.create();
    registry.emplace<Position>(entity, 1.0f, 2.0f, 3.0f);

    auto* pos = registry.try_get<Position>(entity);
    if (pos == nullptr) {
        spdlog::error("Position component not found on entity");
        return EXIT_FAILURE;
    }
    if (pos->x != 1.0f || pos->y != 2.0f || pos->z != 3.0f) {
        spdlog::error("Position values mismatch");
        return EXIT_FAILURE;
    }
    spdlog::info("PASS: EnTT registry create/emplace/try_get");

    // Verify spdlog formatted output
    spdlog::info("PASS: spdlog {}.{}.{}", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);

    return EXIT_SUCCESS;
}
