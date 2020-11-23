#include <inttypes.h>
#include <asio.hpp>
#include <ctime>
#include <iostream>
#include <string>
#include <unordered_map>
#include <thread>

#include "../include/functions.hpp"

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
    // std::cout << "Blocked for read" << std::endl;
    tcp::socket socket(io_context);
    acceptor.accept(socket);

    std::array<uint8_t, 9> server_message;
    asio::error_code error;
    size_t len = socket.read_some(asio::buffer(server_message), error);

    printf("SERVER PORT: %d", port);

    int action = decode_9byte(&server_message, 0);
    int key = decode_9byte(&server_message, 1);
    int value = decode_9byte(&server_message, 2);


    // for now, just write back the same information to the client...
    encode_9byte(&server_message, action, key, value);

    asio::write(socket, asio::buffer(server_message), error);
  }
  return;
}

int main() {
  std::vector<std::thread> servers;

  int firstServer = 3001;
  int maxServer = firstServer + getNumServers() - 1; // WHY DOES IT NEED TO BE MINUS 1???
  printf("Ports open on %d through %d.\n", firstServer, maxServer);
  for (int i = firstServer; i < maxServer; ++i)
    servers.push_back(std::thread(server, i)); // t(function, a0, a1, ...)
  for (auto &server : servers) 
    server.join();


  return 0;
}