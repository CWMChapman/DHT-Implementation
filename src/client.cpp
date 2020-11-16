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

int decode_9byte(std::array<uint8_t, 9> *message, int section) {
	int array_index = 0; // remember that the array is of lenght 4, not 4 * 8.
	int bit_index = 0;	 // which bit within each element of the array we are refering to
	int action = 0;		 // message's action	-- section 0
	int key = 0;		 // message's key	-- section 1
	int value = 0;		 // message's value	-- section 2


	for (int i = 0; i < 8; ++i) {
		action |= (((*message)[array_index] >> (bit_index)) & 1) << (7-i);
		++bit_index;
	}
	++array_index;
	for (int i = 8; i < 40; ++i) {
		key |= (((*message)[array_index] >> bit_index) & 1) << (31 + 8 - i);

		if ((i + 1) % 8 == 0) {
			++array_index;
			bit_index = 0;
		}
		else {
			++bit_index;
		}
	}
	for (int i = 40; i < 72; ++i) {
		value |= (((*message)[array_index] >> bit_index) & 1) << (31 + 40 - i);

		if ((i + 1) % 8 == 0) {
			++array_index;
			bit_index = 0;
		}
		else {
			++bit_index;
		}
	}

	if (section == 0) {
		return action;
	}
	else if (section == 1) {
		return key;
	}
	else if (section == 2) {
		return value;
	}
	else {
		std::cout << "ERROR: That section does not exist, please choose section 0 through 2." << std::endl;
		return -1;
	}
}

void encode_9byte(std::array<uint8_t, 9> *message, int action, int key, int value) {
	int array_index = 0; // remember that the array is of lenght 4, not 4 * 8.
	int bit_index = 0;	 // which bit within each element of the array we are refering to
	

	std::cout << "action: " << std::endl;

	for (int i = 0; i < 32; ++i) {
		std::cout << ((action >> i) & 1);
		if ((i+1)%8 == 0) 
			std::cout << " ";
	}
	std::cout << std::endl;

	// for (int i = 0; i < 32; ++i) {
	// 	action &= ~(1 << i);
	// }
	for (int i = 0; i < 8; ++i) {
		std::cout << ((action >> i) & 1);
		(*message)[array_index] |= ((action >> i) & 1) << (7-bit_index);
		// std::cout << (((*message)[array_index] >> bit_index) & 1);
		++bit_index;
	}
	std::cout << std::endl;
	++array_index;
	for (int i = 8; i < 40; ++i) {
		(*message)[array_index] &= ~(1 << bit_index);
		(*message)[array_index] |= ((key >> (31 + 8 - i)) & 1) << bit_index;

		if ((i + 1) % 8 == 0) {
			++array_index;
			bit_index = 0;
		}
		else {
			++bit_index;
		}
	}
	for (int i = 40; i < 72; ++i) {
		(*message)[array_index] &= ~(1 << bit_index);
		(*message)[array_index] |= ((value >> (31 + 40 - i)) & 1) << bit_index;

		if ((i + 1) % 8 == 0) {
			++array_index;
			bit_index = 0;
		}
		else {
			++bit_index;
		}
	}
	
	

	array_index = 0;
	bit_index = 0;
	std::cout << array_index << ": ";
	for (int i = 0; i < 72; ++i) {
		std::cout << (((*message)[array_index] >> bit_index) & 1);
		if ((i + 1) % 8 == 0) {
			++array_index;
			bit_index = 0;
			if (array_index != 9)
				std::cout << "   " << array_index << ": ";
		}
		else {
			++bit_index;
		}
	}
	std::cout << std::endl;

	return;
}

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
	std::cout << "Action: " << a << ", Key: " << k << ", Value: " << v << std::endl;
	encode_9byte(&message, a, k, v);
	int action = decode_9byte(&message, 0);
	int key = decode_9byte(&message, 1);
	int value = decode_9byte(&message, 2);
	std::cout << "Action: " << action << ", Key: " << key << ", Value: " << value << std::endl;
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