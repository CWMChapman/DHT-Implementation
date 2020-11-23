#include "functions.hpp"
#include <cmath>


//hash function from this stackoverflow post: https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
uint64_t xorshift(const uint64_t& n,int i){
  return n^(n>>i);
}
uint64_t hash(const uint64_t& n){
  uint64_t p = 0x5555555555555555ull; // pattern of alternating 0 and 1
  uint64_t c = 17316035218449499591ull;// random uneven integer constant; 
  return c*xorshift(p*xorshift(n,32),32);
}

int hashToServerPort(int key, int numServers) {
	/* This function takes a key and returns (for now) 1 int that represents a port  */
	int port = hash(key) % numServers + 3001;
	return port;
}




/*
[ip1, ip2, ip3 ...]
*/

int getNumServers() {
	int numServers = 10;
	return numServers;
}