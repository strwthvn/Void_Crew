find_package(tomlplusplus CONFIG REQUIRED)

add_executable(server
    main.cpp
    command_line.cpp
    server.cpp
    server_config.cpp
    signal_handler.cpp
)

target_link_libraries(server PRIVATE common tomlplusplus::tomlplusplus)
