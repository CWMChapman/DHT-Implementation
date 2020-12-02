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

int main(int argc, char** argv) {
	if (argc == 1) {
		std::cout << "ERROR: Incorrect CL arguments provided!" << std::endl;
		return 1;
	}

	DHT_action akv;

	// INSERT
	if (argc == 4 && (strcmp(argv[1], "insert") == 0 || strcmp(argv[1], "0") == 0)) {
		if (std::stoi(argv[3]) < 0) {
			std::cout << "NEGATIVE VALUES ARE NOT ALLOWED" << std::endl;
			return 1;
		}
		akv = { .action = 0, .key = std::stoi(argv[2]), .value = std::stoi(argv[3]) };
		DHT_action response = DHT_Request(akv);
		if(response == akv){ 
			std::cout << "INSERTED: <" << akv.key << ", " << akv.value << "> SUCCESSFULLY" << std::endl;
		}
		else {
			std::cout << "INSERT <" << akv.key << ", " << akv.value << "> FAILED" << std::endl;
		}
	}	// ./client insert <key> <value>

	// SEARCH
	else if (argc == 3 && (strcmp(argv[1], "search") == 0 || strcmp(argv[1], "1") == 0)) {
		akv = { .action = 1, .key = std::stoi(argv[2]) };
		DHT_action response = DHT_Request(akv);
		if(response.value != -1){ 
			std::cout << "SUCCESSFUL SEARCH: KEY <" << akv.key << "> RETRIEVED <" << akv.value << ">" << std::endl;
		}
		else {
			std::cout << "KEY DOES NOT EXIST IN DHT"<< std::endl;
		}
	} 	// ./client lookup <key>

	// DELETE
	else if (argc == 3 && (strcmp(argv[1], "delete") == 0 || strcmp(argv[1], "2") == 0)) {
		akv = { .action = 2, .key = std::stoi(argv[2]) };
		DHT_Request(akv);
		std::cout << "DELETED: <" << akv.key << ", " << akv.value << ">" << std::endl;
	} 	// ./client delete <key>

	// REMOVE SERVER
	else if (argc == 3 && (strcmp(argv[1], "rm_server") == 0 || strcmp(argv[1], "3") == 0)) {
		addressInfo serverToRemove = {.IPAddress = {127, 0, 0, 1}, .port = static_cast<short>(std::stoi(argv[2]))};
		connectToDHT(serverToRemove, 1);
		std::cout << "REMOVED SERVER: <" << addressInfo_tostr(serverToRemove) << ">" << std::endl;
		return 0;
	} 	// ./client rm_server <server port>

	// RANGE INSERT
	else if (argc == 3 && (strcmp(argv[1], "range_insert") == 0)) {
		for (int i = 0; i < std::stoi(argv[2]); i++) {
			akv = { .action = 0, .key = i, .value = i };
			DHT_action response = DHT_Request(akv);
			if(!(response == akv)){ 
				std::cout << "INSERT <" << akv.key << ", " << akv.value << "> FAILED" << std::endl;
			}
		}
		return 0;
	} 	// ./client range_insert <num key,value pairs to insert in range [0, this arguemnt)>

	//RANGE SEARCH
	else if (argc == 3 && (strcmp(argv[1], "range_search") == 0)) {
		for (int i = 0; i < std::stoi(argv[2]); i++) {
			akv = { .action = 1, .key = i};
			DHT_action response = DHT_Request(akv);
			if(response.value == -1){ 
				std::cout << "KEY <" << akv.key << "> DOES NOT EXIST IN DHT"<< std::endl;
			}
		}
		return 0;
	} 	// ./client add_server <server port>

	// WRONG COMMAND LINE ENTRY
	else {
		std::cout << "ERROR: Incorrect CL arguments provided!" << std::endl;
		return 1;
	}

	return 0;
}