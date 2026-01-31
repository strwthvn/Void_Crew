#include "version.hpp"

#include <spdlog/spdlog.h>

int main() {
    spdlog::info("Void Crew Server {}", void_crew::engineVersion());
    return 0;
}
