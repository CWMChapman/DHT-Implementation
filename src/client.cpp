#include <asio.hpp>
#include <iostream>
#include <string> 
#include <locale>         // std::locale, std::tolower

#include "../include/functions.hpp"

using asio::ip::tcp;

/* MESSAGE FORMAT
2 bit message action, 6 bits of padding, 32 bit key, 32 bit value


M is a 2-bit message action, corresponding to 4 possible messages given shortly, K is a 32-bit key, V is a 32-bit value. The four message types in the API are:
ACTION:
M=00: Insert
M=01: Lookup
M=10: Delete
M=11: Rehash server's keys (PRIVATE ACTION FOR DHT, CLIENT SHOULDNT USE)

*/

/* TODO
Set it up such that the main server will also return 

	// int action = std::stoi(argv[1]);
	// int key = std::stoi(argv[2]);
	// int value = std::stoi(argv[3]);
*/


// DHT_action parseArguments(char** argv) {
// 	DHT_action akv;
// 	std::string arg1;
// 	std::transform(arg1.begin(), arg1.end(), arg1.begin(), ::tolower);
// 	// std::string arg1 = boost::tolower(argv[1]);

// 	if (arg1 == "insert")
// 		akv.action = 0;
// 	else if (arg1 == "lookup") {
// 		akv.action = 1;
// 		akv.value = 0;
// 	}
// 	else if (arg1 == "delete") {
// 		akv.action = 2;
// 		akv.value = 0;
// 	}
// 	else
// 		akv.action = 3;
	
// 	akv.key = std::stoi(argv[2]);
// 	akv.value = std::stoi(argv[3]);

// 	return akv;
// }


int main(int argc, char** argv) {
	if(argc != 4){
		std::cout << "ERROR: Incorrect CL arguments provided!" << std::endl;
		return 1;
	}
	
	if(std::stoi(argv[1]) == 5) {
		connectToDHT((addressInfo){.IPAddress = {127, 0, 0, 1}, .port = static_cast<short>(std::stoi(argv[2]))}, 1);
		return 0;
	}

	DHT_action akv = { .action = std::stoi(argv[1]), .key = std::stoi(argv[2]), .value = std::stoi(argv[3]) }; //DHT_action akv = { .action = 0, .key = 12345, .value = 54321 };

	addressInfo trackerServerInfo = { .IPAddress = {127, 0, 0, 1}, .port = 3000 };


	DHT_Request(trackerServerInfo, akv);
	// FIRST CONNECT TO TRACKER SERVER TO GET PORT(S):


	return 0;
}