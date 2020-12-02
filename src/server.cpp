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


void rehash(addressInfo serverInfo, std::unordered_map<int, int> serverMap) {
	std::cout << "REHASHING SERVER: " << addressInfo_tostr(serverInfo) << std::endl;
	DHT_action rehashKV;

	std::unordered_map<int, int>::iterator itr; 

	itr = serverMap.begin();
	while(itr != serverMap.end()){
		rehashKV.action = 0;
		rehashKV.key = itr->first;
		rehashKV.value = itr->second;
		DHT_Request(rehashKV);
		itr++;
	}

	return;
}

void printMap(std::unordered_map<int, int> serverMap) {
	std::unordered_map<int, int>::iterator itr; 
    std::cout << "\nAll Elements : \n"; 
    for (itr = serverMap.begin(); itr != serverMap.end(); itr++)  
        std::cout << itr->first << "  " << itr->second << std::endl;
	return;
}

void server(addressInfo serverInfo) {
	std::unordered_map<int, int> serverMap;
	connectToDHT(serverInfo, 0);

	asio::io_context io_context;
	tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), serverInfo.port));

	uint16_t counter = 0;

	while (true) {
		// Wait for client (Blocked for read)
		tcp::socket socket(io_context);
		acceptor.accept(socket);

		// RECIEVE MESSAGE
		std::array<uint8_t, sizeof(DHT_action)> client_message;
		asio::error_code error;
		size_t len = socket.read_some(asio::buffer(client_message), error);
		DHT_action message;
		memcpy(&message, &client_message, sizeof(DHT_action));


		// std::cout << "MESSAGE RECIEVED ON SERVER: " << addressInfo_tostr(serverInfo) << std::endl;
		int action = message.action;
		int key = message.key;
		int value = message.value;
		// printf("action: %d, key: %d, value: %d\n", action, key, value);

		std::cout << "MESSAGE RECIEVED ON SERVER <" << addressInfo_tostr(serverInfo) << ">: " << dht_action_tostr(message) << std::endl;

		// INSERT / LOOKUP / DELETE FROM SERVER'S UNORDERED MAP
		if(action == 0) {
			// INSERT
			serverMap[key] = value;
			printMap(serverMap);
			std::cout << "\n\n\n";

			// for now, just write back the same information to the client...
			struct DHT_action return_message = {.action = action, .key = key, .value = value};
			memcpy(&client_message, &return_message, sizeof(DHT_action));
			asio::write(socket, asio::buffer(client_message), error);
		} 
		else if(action == 1){
			// LOOKUP KEY
			if (serverMap.find(key) == serverMap.end()) value = -1;
			else value = serverMap.at(key);
			
			// for now, just write back the same information to the client...
			struct DHT_action return_message = {.action = action, .key = key, .value = value};
			memcpy(&client_message, &return_message, sizeof(DHT_action));
			asio::write(socket, asio::buffer(client_message), error);
		}
		else if(action == 2){
			// DELETE
			serverMap.erase(key);
			printMap(serverMap);
			std::cout << "\n\n\n";
			
			// for now, just write back the same information to the client...
			struct DHT_action return_message = {.action = action, .key = key, .value = value};
			memcpy(&client_message, &return_message, sizeof(DHT_action));
			asio::write(socket, asio::buffer(client_message), error);
		}
		else if(action == 4) {
			// REHASH 
			DHT_action return_message = {.action = action, .key = key, .value = value};
			memcpy(&client_message, &return_message, sizeof(DHT_action));
			asio::write(socket, asio::buffer(client_message), error);
			if (!serverMap.empty()){
				std::unordered_map<int,int> mapCopy = serverMap;
				serverMap.clear();
				std::thread rehashTread (rehash, serverInfo, mapCopy);
				rehashTread.detach(); // allow function server() to continue to run while keys are being rehashed such that they can be received again
			}
		} 
	}

	return;
}

std::vector<addressInfo> setUpServerAddresses(int numServers) {
	std::vector<addressInfo> serverAddresses;

	short firstPort = 3002;

	std::array<uint8_t,4> IPAddress = {127, 0, 0, 1}; // for now all the servers share the same ip address
	for (short i = 0; i < numServers; i++){
		short port = firstPort + i;
		serverAddresses.push_back((addressInfo){.IPAddress = IPAddress, .port = port});
	}
	
	return serverAddresses; 
}

int main(int argc, char** argv) {
	int numServers = 10;
	
	if (argc == 3 && strcmp(argv[1], "add_server") == 0) {
		server((addressInfo){.IPAddress = {127, 0, 0, 1}, .port = static_cast<short>(std::stoi(argv[2]))});
		return 0;
	}
	else if (argc == 2) numServers = std::stoi(argv[1]);
	
	std::vector<std::thread> servers;
	std::vector<addressInfo> serverAddresses = setUpServerAddresses(numServers); // see functions.cpp for this functions

	std::cout << "Ports open on " << serverAddresses[0].port << " through " << serverAddresses[numServers-1].port << "." << std::endl;
	for (int i = 0; i < numServers; ++i)
		servers.push_back(std::thread(server, serverAddresses[i])); // t(function, a0, a1, ...)
	for (auto &server : servers) 
		server.join();

	return 0;
}

	// for (const auto &element : serverMap) {
	// 	// std::cout << element.first << ": " << element.second << std::endl;
	// 	rehashKV.action = 0;
	// 	rehashKV.value = element.second;
	// 	std::cout << "element.second = " << element.second << std::endl; 
	// 	rehashKV.key = element.first;
	// 	// std::cout << "element.first = " << element.first << std::endl; 
	// 	serverMap.erase(rehashKV.key);
    //     // std::cout << "REHASHING KEY: " << rehashKV.key << ", VALUE: " << rehashKV.value << std::endl;
		
	// 	DHT_Request(rehashKV);

	// }
	// for (itr = serverMap.begin(); itr != serverMap.end(); itr++) {
	// 	rehashKV.action = 0;
	// 	rehashKV.key = itr->first;
	// 	rehashKV.value = itr->second;
	// 	serverMap.erase(rehashKV.key);
    //     // std::cout << "REHASHING KEY: " << rehashKV.key << ", VALUE: " << rehashKV.value << std::endl;
		
	// 	DHT_Request(rehashKV);
		
	// }