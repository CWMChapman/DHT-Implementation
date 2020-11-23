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

	// int action = std::stoi(argv[1]);
	// int key = std::stoi(argv[2]);
	// int value = std::stoi(argv[3]);
*/





int main(int argc, char** argv) {
	struct DHT_action akv = {.action = 0, .key = 12345, .value = 54321};
	// struct addressInfo trackerServerInfo;
	// trackerServerInfo.IPAddress = {127, 0, 0, 1};
	// https://stackoverflow.com/questions/54512286/how-to-join-an-int-array-to-string-in-c-separated-by-a-dot
	// char buffer[99];
	// sprintf(buffer, "%d.%d.%d.%d", trackerServerInfo.IPAddress[0], trackerServerInfo.IPAddress[1], trackerServerInfo.IPAddress[2], trackerServerInfo.IPAddress[3]);
	// std::string ip_address(buffer);

	std::string trackerServerIP = "127.0.0.1";
	std::string trackerServerPort = "3000";


	
	// FIRST CONNECT TO TRACKER SERVER TO GET PORT(S):

	asio::io_context io_context;
	tcp::resolver resolver(io_context);
	tcp::resolver::results_type endpoints = resolver.resolve(trackerServerIP, trackerServerPort);

	tcp::socket socket(io_context);
	asio::connect(socket, endpoints);

	asio::error_code error;
	std::array<uint8_t, 4> tracker_message;
	memcpy(&tracker_message, &akv.key, sizeof(int));

	asio::write(socket, asio::buffer(tracker_message), error);

	size_t len = socket.read_some(asio::buffer(tracker_message), error);
	int port;
	memcpy(&port, &tracker_message, sizeof(int));
	std::cout << "key: " << akv.key << ", port: " << port << std::endl;






	// NOW THAT WE HAVE THE PORT, CONNECT TO SERVER

	endpoints = resolver.resolve("127.0.0.1", std::to_string(port));

	asio::connect(socket, endpoints);

	std::array<uint8_t, 12> server_message;
	
	memcpy(&server_message, &akv, sizeof(DHT_action));
	asio::write(socket, asio::buffer(server_message), error);

	struct DHT_action received;
	len = socket.read_some(asio::buffer(server_message), error);
	memcpy(&received, &server_message, sizeof(DHT_action));

	printf("action: %d, %d \nkey: %d, %d \nvalue: %d, %d \n\n", akv.action, received.action, akv.key, received.key, akv.value, received.value); // testing to make sure it returns what i expect
	printf("numServers: %d\n", getNumServers());



	return 0;
}