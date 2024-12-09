cmake_minimum_required(VERSION 3.16)
project(simple_cpp_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-pthread")

find_package(Boost REQUIRED COMPONENTS system)
find_package(nlohmann_json REQUIRED)

add_library(logger src/logger.cpp)
add_library(shared src/shared/structs.cpp)

add_library(server_lib src/server/server.cpp)
target_link_libraries(server_lib logger shared Boost::system nlohmann_json::nlohmann_json)

add_executable(server src/server/server_main.cpp)
target_link_libraries(server server_lib)

add_library(client_lib src/client/client.cpp)
target_link_libraries(client_lib logger shared Boost::system nlohmann_json::nlohmann_json)

add_executable(client src/client/client_main.cpp)
target_link_libraries(client client_lib)

# Tests
find_package(doctest CONFIG REQUIRED)

add_executable(tests
        tests/acceptance_tests.cpp
        tests/logger_tests.cpp
        tests/tests_main.cpp
)
target_include_directories(tests PRIVATE src)
target_link_libraries(tests server_lib client_lib doctest::doctest)

enable_testing()
add_test(NAME test COMMAND tests)
