cmake_minimum_required(VERSION 3.25)
project(VoidCrew LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Allow IDE generators to group targets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Configurations
if(NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)
endif()
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Targets
add_subdirectory(src/common)
add_subdirectory(src/server)
add_subdirectory(src/client)
add_subdirectory(tests)

# Testing
enable_testing()
add_test(NAME VoidCrewTests COMMAND tests)
