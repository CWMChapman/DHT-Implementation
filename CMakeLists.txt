cmake_minimum_required(VERSION 3.1)
project(sockets)

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CLIENT_FILES
  src/client.cpp
)

set(SERVER_FILES
  src/server0.cpp
)

add_executable(client ${CLIENT_FILES})
target_include_directories(client PRIVATE include)
target_include_directories(client PRIVATE third_party/asio-1.18.0/include)
target_link_libraries(client PRIVATE Threads::Threads)

add_executable(server ${SERVER_FILES})
target_include_directories(server PRIVATE include)
target_include_directories(server PRIVATE third_party/asio-1.18.0/include)
target_link_libraries(server PRIVATE Threads::Threads)
