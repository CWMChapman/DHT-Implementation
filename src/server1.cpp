#include <inttypes.h>
#include <asio.hpp>
#include <ctime>
#include <iostream>
#include <string>
#include <unordered_map>

using asio::ip::tcp;

int main() {
  asio::io_context io_context;
  tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 3001));

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

  return 0;
}