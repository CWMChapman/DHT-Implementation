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

void connectToDHT(addressInfo serverInfo, short action) {
	addressInfo trackerServerInfo = { .IPAddress = {127, 0, 0, 1}, .port = 3001 };
	serverAction sA = {.action = action, .info = serverInfo};
	// action = 0 -> join DHT
	// action = 1 -> leave DHT

	asio::io_context io_context;
	tcp::resolver resolver(io_context);
	tcp::resolver::results_type endpoints = resolver.resolve(ip_tostr(trackerServerInfo.IPAddress), std::to_string(trackerServerInfo.port));

	tcp::socket socket(io_context);
	asio::connect(socket, endpoints);

	asio::error_code error;
	std::array<uint8_t, 8> server_message;
	memcpy(&server_message, &sA, sizeof(serverAction));

	asio::write(socket, asio::buffer(server_message), error);
	
	// no expected response
	
	return;
}

void server(addressInfo serverInfo) {
	connectToDHT(serverInfo, 0);
	std::unordered_map<int, int> serverMap;

	asio::io_context io_context;
	tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), serverInfo.port));

	uint16_t counter = 0;

	while (true) {
		// Wait for client (Blocked for read)
		tcp::socket socket(io_context);
		acceptor.accept(socket);

		std::array<uint8_t, 12> client_message;
		asio::error_code error;
		size_t len = socket.read_some(asio::buffer(client_message), error);

		printf("\nSERVER PORT: %d\n", serverInfo.port);

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

int main() {
	std::vector<std::thread> servers;

	int numServers = getNumServers();
	std::vector<addressInfo> serverAddresses = setUpServerAddresses(); // see functions.cpp for this functions

	std::cout << "Ports open on " << serverAddresses[0].port << " through " << serverAddresses[numServers-1].port << "." << std::endl;
	for (int i = 0; i < numServers; ++i)
		servers.push_back(std::thread(server, serverAddresses[i])); // t(function, a0, a1, ...)
	for (auto &server : servers) 
		server.join();


	return 0;
}