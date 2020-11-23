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

		std::array<uint8_t, 12> client_message;
		asio::error_code error;
		size_t len = socket.read_some(asio::buffer(client_message), error);

		printf("\nSERVER PORT: %d\n", port);

		struct DHT_action message;
		memcpy(&message, &client_message, sizeof(DHT_action));
		int action = message.action;
		int key = message.key;
		int value = message.value;
		printf("action: %d, key: %d, value: %d\n", action, key, value);


		// for now, just write back the same information to the client...
		memcpy(&client_message, &message, sizeof(DHT_action));
		asio::write(socket, asio::buffer(client_message), error);
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

//Where will we be processing all of the work??

//We are gonna need certain things for all these servers
  //Need to create an unordered map
    // unordered_map<int, int> serverMap;

  //if the action is to create a key/val 
    /*void createKey( KEY, VALUE ){
      serverMap[VALUE]= KEY
    }*/

  //if the action is to find a key
    /*int find( KEY ){
      if (serverMap.find(key) == serverMap.end()) 
        cout << key << " not found\n"; 
      else
        return key
    }*/ 

  //if the action is to delete a key
    /*void delete( KEY ){
      serverMap.erase(KEY);
    }*/  //set key/val
