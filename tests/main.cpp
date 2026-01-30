#include "version.hpp"

#include <cstdlib>
#include <cstdio>
#include <string_view>

int main() {
    // Verify engine version is not empty
    std::string_view version = void_crew::engineVersion();
    if (version.empty()) {
        std::printf("FAIL: engineVersion() returned empty string\n");
        return EXIT_FAILURE;
    }

    std::printf("PASS: engineVersion() = \"%.*s\"\n",
        static_cast<int>(version.size()), version.data());
    return EXIT_SUCCESS;
}
