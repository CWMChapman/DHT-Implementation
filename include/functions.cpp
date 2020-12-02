#include "functions.hpp"


// TO STRING FUNCTIONS 
// source: https://stackoverflow.com/questions/54512286/how-to-join-an-int-array-to-string-in-c-separated-by-a-do
std::string ip_tostr(std::array<uint8_t,4> IPAddress) {
	char buffer[99];
	sprintf(buffer, "%d.%d.%d.%d", IPAddress[0], IPAddress[1], IPAddress[2], IPAddress[3]);
	std::string ip_address(buffer);
	return ip_address;
}

std::string addressInfo_tostr(addressInfo info) {
	return ip_tostr(info.IPAddress) + ":" + std::to_string(info.port);
}

std::string dht_action_tostr(DHT_action action) {
	return "A: " + std::to_string(action.action) + ", K: " + std::to_string(action.key) + ", V: " + std::to_string(action.value);
}




// SERVER MESSAGING FUNCTIONS

DHT_action Server_Request(addressInfo server, DHT_action akv) {
	using asio::ip::tcp;

	asio::io_context io_context;
	tcp::resolver resolver(io_context);

	tcp::resolver::results_type endpoints = resolver.resolve(ip_tostr(server.IPAddress), std::to_string(server.port));

	tcp::socket socket(io_context);
	asio::connect(socket, endpoints);

	asio::error_code error;
	std::array<uint8_t, 12> server_message;
	
	memcpy(&server_message, &akv, sizeof(DHT_action));
	asio::write(socket, asio::buffer(server_message), error);

	DHT_action received;
	size_t len = socket.read_some(asio::buffer(server_message), error);
	memcpy(&received, &server_message, sizeof(DHT_action));

	// printf("action: %d, %d \nkey: %d, %d \nvalue: %d, %d \n\n", akv.action, received.action, akv.key, received.key, akv.value, received.value); // testing to make sure it returns what i expect

	return received;

}

DHT_action DHT_Request(DHT_action akv) {
	addressInfo trackerServerInfo = { .IPAddress = {127, 0, 0, 1}, .port = 3000 };
	// std::cout << "SERVER: " << addressInfo_tostr(trackerServerInfo) << ", " << dht_action_tostr(akv) << std::endl;
	using asio::ip::tcp;

	asio::io_context io_context;
	tcp::resolver resolver(io_context);
	tcp::resolver::results_type endpoints = resolver.resolve(ip_tostr(trackerServerInfo.IPAddress), std::to_string(trackerServerInfo.port));

	tcp::socket socket(io_context);
	asio::connect(socket, endpoints);

	asio::error_code error;
	std::array<uint8_t, 4> key_message; // we're just sending the key to the tracker
	memcpy(&key_message, &akv.key, sizeof(int));

	asio::write(socket, asio::buffer(key_message), error);
	std::array<uint8_t, 3*sizeof(addressInfo)> return_tracker_message; // we're receiving an array of 3 addressInfo structs
	size_t len = socket.read_some(asio::buffer(return_tracker_message), error);
	std::array<addressInfo, 3> response;
	memcpy(&response, &return_tracker_message, 3*sizeof(addressInfo));
	addressInfo server = response[0];
	addressInfo neighbor1 = response[1];
	addressInfo neighbor2 = response[2];
	
	// NOW THAT WE HAVE THE PORT, CONNECT TO SERVER
	DHT_action primaryServer = Server_Request(server, akv);
	DHT_action neighborServer1 = Server_Request(neighbor1, akv);
	DHT_action neighborServer2 = Server_Request(neighbor2, akv);
	
	if (primaryServer == neighborServer1  && primaryServer == neighborServer2) {
		return primaryServer;
	}
	else {
		std::cout << "\nSERVER RESPONSES DO NOT MATCH:" << std::endl;
		std::cout << "primaryServer: " << addressInfo_tostr(server) << "  --  " << dht_action_tostr(primaryServer) << std::endl;
		std::cout << "neighborServer1: " << addressInfo_tostr(neighbor1) << "  --  " << dht_action_tostr(neighborServer1) << std::endl;
		std::cout << "neighborServer2: " << addressInfo_tostr(neighbor2) << "  --  " << dht_action_tostr(neighborServer2) << std::endl;
		return (DHT_action){.action = -1, .key = -1, .value = -1};
	}
}

void connectToDHT(addressInfo serverInfo, short action) {
	using asio::ip::tcp;

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
	std::array<uint8_t, sizeof(serverAction)> DHT_server_action;
	memcpy(&DHT_server_action, &sA, sizeof(serverAction));

	asio::write(socket, asio::buffer(DHT_server_action), error);
	
	return;
}








// HASHING FUNCTIONS
uint64_t xorshift(const uint64_t& n,int i){
	return n^(n>>i);
}
uint64_t hash(const uint64_t& n){
	uint64_t p = 0x5555555555555555ull; // pattern of alternating 0 and 1
	uint64_t c = 17316035218449499591ull;// random uneven integer constant; 
	return c*xorshift(p*xorshift(n,32),32);
}
//hash function from this stackoverflow post: https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key

