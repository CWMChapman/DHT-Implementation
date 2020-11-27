#include <inttypes.h>
#include <asio.hpp>
#include <ctime>
#include <iostream>
#include <string>
#include <unordered_map>
#include <thread>

#include "../include/functions.hpp"

/* *** MESSAGE FORMAT
2 bit message action, 6 bits of padding, 32 bit key, 32 bit value


M is a 2-bit message action, corresponding to 4 possible messages given shortly, K is a 32-bit key, V is a 32-bit value. The four message types in the API are:

M=00: Insert
M=01: Lookup
M=10: Delete
M=11: Do Nothing. For later, maybe we do this to reorganize the servers keys and redistribute things in case a server is added or removed.

*** */

using asio::ip::tcp;

void server(int port) {
	std::unordered_map<int, int> serverMap;

	asio::io_context io_context;
	tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

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

		// adding/finding/deleting keys from map and letting us know if the actions were completed or not
		if(action == 0){
			serverMap[key] = value;
		}
		else if(action == 1){
			value = serverMap[key];
		}
		else if(action == 2){
			serverMap.erase(key);
		}

		struct DHT_action return_message = {.action = 0, .key = 0, .value = value};


		// for now, just write back the same information to the client...
		memcpy(&client_message, &return_message, sizeof(DHT_action));
		asio::write(socket, asio::buffer(client_message), error);
	}

	return;
}

// Function called to check if changes were done correctly to the map
// bool checkChanges(int action, int key, int value, std::unordered_map<int, int> serverMap){
// 	auto find = serverMap.find(key);

// 	if(action == 0){
// 		if (find == serverMap.end()) 
// 			return false; 
// 		else{
// 			return true;
// 		}
// 	}
// 	else if(action == 2){
// 		if (find == serverMap.end()) 
// 			return true; 
// 		else{
// 			return false;
// 		}
// 	}
// }

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