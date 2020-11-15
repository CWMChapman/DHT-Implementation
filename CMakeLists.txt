cmake_minimum_required(VERSION 3.1)
project(sockets)

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CLIENT_FILES
  src/client.cpp
)

set(SERVER0_FILES
  src/server0.cpp
)

set(SERVER1_FILES
  src/server1.cpp
)

add_executable(client ${CLIENT_FILES})
target_include_directories(client PRIVATE include)
target_include_directories(client PRIVATE third_party/asio-1.18.0/include)
target_link_libraries(client PRIVATE Threads::Threads)

add_executable(server0 ${SERVER0_FILES})
target_include_directories(server0 PRIVATE include)
target_include_directories(server0 PRIVATE third_party/asio-1.18.0/include)
target_link_libraries(server0 PRIVATE Threads::Threads)

add_executable(server1 ${SERVER1_FILES})
target_include_directories(server1 PRIVATE include)
target_include_directories(server1 PRIVATE third_party/asio-1.18.0/include)
target_link_libraries(server1 PRIVATE Threads::Threads)
