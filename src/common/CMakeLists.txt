find_package(EnTT CONFIG REQUIRED)
find_package(flatbuffers CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_library(common STATIC
    version.cpp
    version.hpp
)

target_include_directories(common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(common PUBLIC
    EnTT::EnTT
    flatbuffers::flatbuffers
    glm::glm
    spdlog::spdlog
)
