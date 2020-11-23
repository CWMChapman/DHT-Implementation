#include <asio.hpp>
#include <iostream>
#include <string> 

#include "../include/functions.hpp"

using asio::ip::tcp;

/* MESSAGE FORMAT
2 bit message action, 6 bits of padding, 32 bit key, 32 bit value


M is a 2-bit message action, corresponding to 4 possible messages given shortly, K is a 32-bit key, V is a 32-bit value. The four message types in the API are:

M=00: Insert
M=01: Lookup
M=10: Delete
M=11: Do Nothing. For later, maybe we do this to reorganize the servers keys and redistribute things in case a server is added or removed.

decode sections
0 = get message type

1 = get key

2 = get value

*/

/* EXAMPLE ENCODE DECODE USAGE
std::array<uint8_t, 9> message;
int a = 59;
int k = 4321;
int v = 1234;
std::cout << "Before: \tAction: " << a << ", Key: " << k << ", Value: " << v << std::endl;
encode_9byte(&message, a, k, v);
int action = decode_9byte(&message, 0);
int key = decode_9byte(&message, 1);
int value = decode_9byte(&message, 2);
std::cout << "After: \t\tAction: " << action << ", Key: " << key << ", Value: " << value << std::endl;
*/

/* TODO
Set it up such that the main server will also return 

*/





int main(int argc, char** argv) {
	// int action = std::stoi(argv[1]);
	// int key = std::stoi(argv[2]);
	// int value = std::stoi(argv[3]);
	for (int i = 0; i < 10; ++i) {
		int action = 0; 
		int key = i; 
		int value = 54321;
	
	// FIRST CONNECT TO TRACKER SERVER TO GET PORT(S):
	asio::io_context io_context;
	tcp::resolver resolver(io_context);
	tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "3000");

	tcp::socket socket(io_context);
	asio::connect(socket, endpoints);

	asio::error_code error;
	std::array<uint8_t, 4> tracker_message;

	main_server_encode(&tracker_message, key);
	asio::write(socket, asio::buffer(tracker_message), error);

	size_t len = socket.read_some(asio::buffer(tracker_message), error);
	int port = main_server_decode(&tracker_message);
	std::cout << "key: " << key << ", port: " << port << std::endl;


	// NOW THAT WE HAVE THE PORT, CONNECT TO SERVER
	// asio::io_context io_context;
	// tcp::resolver resolver(io_context);
	endpoints = resolver.resolve("127.0.0.1", std::to_string(port));

	// tcp::socket socket(io_context);
	asio::connect(socket, endpoints);

	// asio::error_code error;
	std::array<uint8_t, 9> server_message;
	
	encode_9byte(&server_message, action, key, value);
	asio::write(socket, asio::buffer(server_message), error);

	len = socket.read_some(asio::buffer(server_message), error);
	int a = decode_9byte(&server_message, 0);
	int k = decode_9byte(&server_message, 1);
	int v = decode_9byte(&server_message, 2);
	printf("action: %d, %d \nkey: %d, %d \nvalue: %d, %d \n\n", action, a, key, k, value, v); // testing to make sure it returns what i expect
	}

	for (int i = 50; i < 150; ++i) {
		int port = hashToServerPort(i, getNumServers());
		printf("port[%d] = %d\n", i, port);
	}
	printf("numServers: %d\n", getNumServers());

	return 0;
}