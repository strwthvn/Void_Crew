find_package(Catch2 CONFIG REQUIRED)

add_executable(tests
    main.cpp
)

target_link_libraries(tests PRIVATE common Catch2::Catch2WithMain)
