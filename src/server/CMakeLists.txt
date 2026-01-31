add_executable(server
    main.cpp
    command_line.cpp
    server.cpp
)

target_link_libraries(server PRIVATE common)
