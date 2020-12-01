#include "functions.hpp"






// source: https://stackoverflow.com/questions/54512286/how-to-join-an-int-array-to-string-in-c-separated-by-a-dot
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

void Server_Request(addressInfo server, DHT_action akv) {
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

	struct DHT_action received;
	size_t len = socket.read_some(asio::buffer(server_message), error);
	memcpy(&received, &server_message, sizeof(DHT_action));

	printf("action: %d, %d \nkey: %d, %d \nvalue: %d, %d \n\n", akv.action, received.action, akv.key, received.key, akv.value, received.value); // testing to make sure it returns what i expect
	printf("numServers: %d\n", getNumServers());
}

void DHT_Request(addressInfo trackerServerInfo, DHT_action akv) {
	std::cout << "SERVER: " << addressInfo_tostr(trackerServerInfo) << ", " << dht_action_tostr(akv) << std::endl;
	using asio::ip::tcp;
	// std::cout << "TEST 1" << std::endl;

	asio::io_context io_context;
	tcp::resolver resolver(io_context);
	tcp::resolver::results_type endpoints = resolver.resolve(ip_tostr(trackerServerInfo.IPAddress), std::to_string(trackerServerInfo.port));

	tcp::socket socket(io_context);
	asio::connect(socket, endpoints);
	// std::cout << "TEST 2" << std::endl;

	asio::error_code error;
	std::array<uint8_t, 4> key_message; // we're just sending the key to the tracker
	memcpy(&key_message, &akv.key, sizeof(int));

	asio::write(socket, asio::buffer(key_message), error);
	std::cout << "TEST 3" << std::endl;
	std::array<uint8_t, 3*sizeof(addressInfo)> return_tracker_message; // we're receiving an array of 2 addressInfo structs
	std::cout << "TEST 3.25" << std::endl;
	size_t len = socket.read_some(asio::buffer(return_tracker_message), error);
	std::cout << "TEST 3.5" << std::endl;
	std::array<addressInfo, 3> response;
	std::cout << "TEST 3.75" << std::endl;
	memcpy(&response, &return_tracker_message, 3*sizeof(addressInfo));
	std::cout << "TEST 4" << std::endl;
	struct addressInfo server = response[0];
	struct addressInfo neighbor1 = response[1];
	struct addressInfo neighbor2 = response[2];
	

	std::cout << "primaryServer: " << addressInfo_tostr(server) << std::endl;
	std::cout << "neighborServer1: " << addressInfo_tostr(neighbor1) << std::endl;
	std::cout << "neighborServer2: " << addressInfo_tostr(neighbor2) << std::endl;

	
	// NOW THAT WE HAVE THE PORT, CONNECT TO SERVER

	Server_Request(server, akv);
	Server_Request(neighbor1, akv);
	Server_Request(neighbor2, akv);
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


//hash function from this stackoverflow post: https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
uint64_t xorshift(const uint64_t& n,int i){
	return n^(n>>i);
}
uint64_t hash(const uint64_t& n){
	uint64_t p = 0x5555555555555555ull; // pattern of alternating 0 and 1
	uint64_t c = 17316035218449499591ull;// random uneven integer constant; 
	return c*xorshift(p*xorshift(n,32),32);
}

short hashToServerPort(int key, int numServers) {
	/* This function takes a key and returns (for now) 1 int that represents a port  */
	short port = hash(key) % numServers + 3001;
	return port;
}





/*
[ip1, ip2, ip3 ...]
*/

int getNumServers() {
	int numServers = 10;
	return numServers;
}

// void connectToDHT() {
// 	return;
// }

std::vector<addressInfo> setUpServerAddresses() {
	int numServers = getNumServers();
	std::vector<addressInfo> serverAddresses;

	short firstPort = 3002;

	std::array<uint8_t,4> IPAddress = {127, 0, 0, 1}; // for now all the servers share the same ip address
	for (short i = 0; i < numServers; i++){
		short port = firstPort + i;
		serverAddresses.push_back((addressInfo){.IPAddress = IPAddress, .port = port});
	}
	
	return serverAddresses; 
}

