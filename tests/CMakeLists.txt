find_package(Catch2 CONFIG REQUIRED)

add_executable(tests
    main.cpp
    server_tests.cpp
)

target_link_libraries(tests PRIVATE common server_lib Catch2::Catch2WithMain)
