#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <array>
#include <iostream>
#include <string>
#include <vector>

typedef struct addressInfo {
	std::array<uint8_t,4> IPAddress;
    short port;
} addressInfo; // SIZE = 6 BYTES

typedef struct serverAction {
    short action; // 0 if joining, 1 if leaving
    addressInfo info;
} serverAction; // SIZE = 8 BYTES

typedef struct DHT_action {
    int action;
    int key;
    int value;
}DHT_action; // SIZE = 12 BYTES

void initDHT();

std::string ip_tostr(std::array<uint8_t,4> IPAddress);

uint64_t xorshift(const uint64_t& n,int i); // helper for hash function
uint64_t hash(const uint64_t& n); // hash function
short hashToServerPort(int key, int numServers); // return the server port based on key

int getNumServers(); // get the number of active servers
void connectToDHT();
std::vector<addressInfo> setUpServerAddresses();

#endif