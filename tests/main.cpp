#include "version.hpp"

#include <cstdio>
#include <cstdlib>
#include <string_view>

#include <entt/entt.hpp>

struct Position {
    float x;
    float y;
    float z;
};

int main() {
    // Verify engine version is not empty
    std::string_view version = void_crew::engineVersion();
    if (version.empty()) {
        std::printf("FAIL: engineVersion() returned empty string\n");
        return EXIT_FAILURE;
    }
    std::printf("PASS: engineVersion() = \"%.*s\"\n",
        static_cast<int>(version.size()), version.data());

    // Verify EnTT registry: create entity, attach and read component
    entt::registry registry;
    auto entity = registry.create();
    registry.emplace<Position>(entity, 1.0f, 2.0f, 3.0f);

    auto* pos = registry.try_get<Position>(entity);
    if (pos == nullptr) {
        std::printf("FAIL: Position component not found on entity\n");
        return EXIT_FAILURE;
    }
    if (pos->x != 1.0f || pos->y != 2.0f || pos->z != 3.0f) {
        std::printf("FAIL: Position values mismatch\n");
        return EXIT_FAILURE;
    }
    std::printf("PASS: EnTT registry create/emplace/try_get\n");

    return EXIT_SUCCESS;
}
