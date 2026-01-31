add_executable(server
    main.cpp
    command_line.cpp
)

target_link_libraries(server PRIVATE common)
