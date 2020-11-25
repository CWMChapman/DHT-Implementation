#ifndef functions
#define functions

#include <array>
#include <iostream>
#include <string>

struct addressInfo {
	std::array<uint8_t,4> IPAddress;
    short port;
};

struct DHT_action {
    int action;
    int key;
    int value;
};

std::string ip_tostr(std::array<uint8_t,4> IPAddress);

uint64_t xorshift(const uint64_t& n,int i); // helper for hash function
uint64_t hash(const uint64_t& n); // hash function
short hashToServerPort(int key, int numServers); // return the server port based on key

int getNumServers(); // get the number of active servers

#endif