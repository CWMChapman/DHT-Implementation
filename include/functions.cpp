#include "functions.hpp"
#include <cmath>



void initDHT() {
	// initializes the DHT servers with data
	printf("INITIALIZING DHT\n");
	

	return;
}




// source: https://stackoverflow.com/questions/54512286/how-to-join-an-int-array-to-string-in-c-separated-by-a-dot
std::string ip_tostr(std::array<uint8_t,4> IPAddress) {
	char buffer[99];
	sprintf(buffer, "%d.%d.%d.%d", IPAddress[0], IPAddress[1], IPAddress[2], IPAddress[3]);
	std::string ip_address(buffer);
	return ip_address;
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

