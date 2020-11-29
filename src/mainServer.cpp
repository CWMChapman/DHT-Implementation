#include <inttypes.h>
#include <asio.hpp>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <algorithm>

#include "../include/functions.hpp"

using asio::ip::tcp;

/* The Tracker Server (Main Server)

This server acts as a tracker server in a traditional P2P network. The  client first communicates with the tacker server 
to determine which server in the DHT network it needs to communicate with.

The message format will be a 4 byte message that contains the key of type int (4 byte, 32 bits).


Tracker Server Port is 3000
Port 3001 is open waiting for servers to message about joining or quiting 

*/

// void rehash() { 
//     activeServers++;
//     //Get all keys from all servers and rehash them with new value
//     //Need a way to get all items from each server
// }


void initDHT() {
	// initializes the DHTin servers with data
	printf("INITIALIZING DHT\n");
	
	return;
}


// bool serverStatus = false; // True means its been initialized with data


std::vector<addressInfo> activeServers;

int getNumActiveServers() {
    return activeServers.size();
}
void printActiveServers() {
    if (activeServers.size() > 0) {
        std::cout << "ACTIVE SERVERS: " << std::endl;
        std::cout << "=============================" << std::endl;
        std::cout << "Server\t|    Address" << std::endl;
            for(int i=0; i < activeServers.size(); i++)
                std::cout << "   " << i << "\t|    " << ip_tostr(activeServers.at(i).IPAddress) << ":" << activeServers.at(i).port << std::endl;
        std::cout << "=============================" << std::endl;
    }
    else
        std::cout << "NO ACTIVE SERVERS" << std::endl;
    return;
}
void addServer(addressInfo serverInfo) {
    activeServers.push_back(serverInfo);
}
void deleteServer(addressInfo serverInfo) {
    // havent tested this yet
    int index = 0;
    for (int i = 0; i < activeServers.size(); i++) {
        if (memcmp(&activeServers[i], &serverInfo, sizeof(addressInfo))) {
            index = i;
            break;
        }
    }
    activeServers.erase(activeServers.begin() + index, activeServers.end()); 
}
void listenForServers() {
    asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 3001));

    while (true) {
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        std::array<uint8_t, 8> buf;
        asio::error_code error;
        size_t len = socket.read_some(asio::buffer(buf), error);
        
        serverAction message;
        memcpy(&message, &buf, sizeof(serverAction));
        // short action = message.action;
        // std::cout << message.action << std::endl;
        
        if (message.action == 0)
            addServer(message.info);
        else{
            deleteServer(message.info);
        }
        if (activeServers.size() >= 10)
            printActiveServers();
    }
    
    return;
}
addressInfo getServer(int key) {
    return activeServers[hash(key) % getNumActiveServers()];
}
addressInfo getNeighborServer1(int key) {
    int numActiveServers = getNumActiveServers();
    int index = (hash(key) % numActiveServers);
    if (index == numActiveServers-1)
        index = 0;
    else
        index++;
    return activeServers[index];
}
addressInfo getNeighborServer2(int key) {
    int numActiveServers = getNumActiveServers();
    int index = (hash(key) % numActiveServers);
    if (index == 0)
        index = numActiveServers - 1;
    else
        index--;
    return activeServers[index];
}

/* 
1. get the that is leaving
2. remove from the activeServers vector
3. hash all of the entries of the server that's leaving
4. 



*/





/* ***     PORT 3000     *** */
int main() {
    std::thread serverListenerThread (listenForServers);

    asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 3000));

    

    while (true) {
        // Wait for client
        // std::cout << "Blocked for read" << std::endl;
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        std::array<uint8_t, 4> buf;
        asio::error_code error;
        size_t len = socket.read_some(asio::buffer(buf), error);
        
        // int numActiveServers = getNumActiveServers();
        int key;
        memcpy(&key, &buf, sizeof(int));
        std::array<addressInfo,3> response = {getServer(key), getNeighborServer1(key), getNeighborServer2(key)};
        // response[0] = getServer(key);
        // response[1] = getNeighborServer2(key);       
        // response[2] = getNeighborServer2(key);

        std::array<uint8_t, 18> responseMessage;
        memcpy(&responseMessage, &response, 3*sizeof(addressInfo));
        asio::write(socket, asio::buffer(responseMessage), error);
    }

    return 0;
}