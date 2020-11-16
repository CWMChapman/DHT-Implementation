#include <inttypes.h>
#include <asio.hpp>
#include <ctime>
#include <iostream>
#include <string>
#include <unordered_map>
#include <thread>

using asio::ip::tcp;

void server(int port) {
  //  std::cout << "Port: " << port << std::endl;
  asio::io_context io_context;
  tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

  // Use this bakery to handle queries from the client
  // Bakery bakery = text_deserializer("../data/bakery.txt");

  uint16_t counter = 0;

  while (true) {
    // Wait for client
    std::cout << "Blocked for read" << std::endl;
    tcp::socket socket(io_context);
    acceptor.accept(socket);

    std::array<uint8_t, 3> buf;
    asio::error_code error;
    size_t len = socket.read_some(asio::buffer(buf), error);

    // Example of error handling
    // if (error != asio::error::eof)
    //   throw asio::system_error(error);

    // Add x to counter
    auto x = uint8_t(buf[0]);
    counter += x;
    std::cout << +x << " " << counter << std::endl;

    buf.fill(0);

    std::memcpy(&buf, &counter, sizeof(uint16_t));

    asio::write(socket, asio::buffer(buf), error);
  }
  return;
}

int main() {
  // int port = 3003;
  std::vector<std::thread> servers;

  for (int i = 3001; i < 3004; ++i)
    servers.push_back(std::thread(server, i)); // t(function, a0, a1, ...)
  for (auto &server : servers) 
    server.join();


  return 0;
}