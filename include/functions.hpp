#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <asio.hpp>

typedef struct addressInfo {
	std::array<uint8_t,4> IPAddress;
    short port;

    bool operator==(const addressInfo& other) const
    {
        return (IPAddress == other.IPAddress && port == other.port);
    }

} addressInfo;

typedef struct serverAction {
    short action; // 0 if joining, 1 if leaving
    addressInfo info;
} serverAction;

typedef struct DHT_action {
    int action;
    int key;
    int value;
}DHT_action;

void initDHT();

std::string ip_tostr(std::array<uint8_t,4> IPAddress);
std::string addressInfo_tostr(addressInfo info);
std::string dht_action_tostr(addressInfo info);

void Server_Request(addressInfo server, DHT_action akv);
void DHT_Request(addressInfo trackerServerInfo, DHT_action akv);
void connectToDHT(addressInfo serverInfo, short action);

uint64_t xorshift(const uint64_t& n,int i); // helper for hash function
uint64_t hash(const uint64_t& n); // hash function
short hashToServerPort(int key, int numServers); // return the server port based on key

int getNumServers(); // get the number of active servers
void connectToDHT();
std::vector<addressInfo> setUpServerAddresses();

#endif