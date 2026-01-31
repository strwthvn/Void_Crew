find_package(tomlplusplus CONFIG REQUIRED)

add_library(server_lib STATIC
    command_line.cpp
    server.cpp
    server_config.cpp
    signal_handler.cpp
)

target_include_directories(server_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(server_lib PUBLIC common tomlplusplus::tomlplusplus)

add_executable(server main.cpp)
target_link_libraries(server PRIVATE server_lib)
