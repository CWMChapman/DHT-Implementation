#include <asio.hpp>
#include <iostream>

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
#include "../include/functions.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("ERROR: DONT FORGET THE PORT IS A COMMAND LINE ARGUMENT!");
		return 1;
	}

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
	/* EXAMPLE ENCODE DECODE USAGE
	*/

	asio::io_context io_context;
	tcp::resolver resolver(io_context);
	tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", argv[1]);

	tcp::socket socket(io_context);
	asio::connect(socket, endpoints);

	asio::error_code error;
	std::array<uint8_t, 3> buf;
	buf.fill(0);
	buf[0] |= 1 << 7;
	asio::write(socket, asio::buffer(buf), error);

	size_t len = socket.read_some(asio::buffer(buf), error);
	uint16_t response = *reinterpret_cast<uint16_t *>(&buf.data()[0]);
	std::cout << response << std::endl;

	return 0;
}