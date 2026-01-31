#include <cstdio>

#include "version.hpp"

int main() {
    std::printf("Void Crew Client %.*s\n",
                static_cast<int>(void_crew::engineVersion().size()),
                void_crew::engineVersion().data());
    return 0;
}
