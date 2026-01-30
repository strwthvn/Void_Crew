add_library(common STATIC
    version.cpp
    version.hpp
)

target_include_directories(common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
