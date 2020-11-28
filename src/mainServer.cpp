#include <inttypes.h>
#include <asio.hpp>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <algorithm>
// #include <unordered_map>

#include "../include/functions.hpp"

using asio::ip::tcp;

/* The Tracker Server (Main Server)

This server acts as a tracker server in a traditional P2P network. The  client first communicates with the tacker server 
to determine which server in the DHT network it needs to communicate with.

The message format will be a 4 byte message that contains the key of type int (4 byte, 32 bits).


Tracker Server Port is 3000
Port 3001 is open waiting for servers to message about joining or quiting 




*/

// std::vector<int> availableServers;
// int activeServers;

// void rehash(){
//     activeServers++;
//     //Get all keys from all servers and rehash them with new value
//     //Need a way to get all items from each server
// }

std::vector<addressInfo> activeServers;

void printActiveServers() {
    if (activeServers.size() > 0) {
        std::cout << "ACTIVE SERVERS: " << std::endl;
        std::cout << "=============================" << std::endl;
        std::cout << "Server\t|    Address" << std::endl;

            for(int i=0; i < activeServers.size(); i++){
                std::cout << "   " << i << "\t|    " << ip_tostr(activeServers.at(i).IPAddress) << ":" << activeServers.at(i).port << std::endl;
            }

        std::cout << "=============================" << std::endl;
    }
    else
        std::cout << "NO ACTIVE SERVERS" << std::endl;

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
            activeServers.push_back(message.info);
        else{
            // havent tested this yet
            int index = 0;
            for (int i = 0; i < activeServers.size(); i++) {
                if (memcmp(&activeServers[i], &message.info, sizeof(addressInfo))) {
                    index = i;
                    break;
                }
            }
            activeServers.erase(activeServers.begin() + index, activeServers.end()); 
        }
        if (activeServers.size() >= 10)
            printActiveServers();
    }
    
    return;
}


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
        
        int numServers = getNumServers();
        int key;
        memcpy(&key, &buf, sizeof(int));
        short port = hashToServerPort(key, numServers);

        short neighborPort1 = port - 1; 
        short neighborPort2 = port + 1;

        if (port - 1 == 3001 - 1) {
            neighborPort1 = 3001 + numServers - 1;
        }
        if (port + 1 == numServers + 3001) {
            neighborPort2 = 3001;
        }

        printf("port: %d, neighborPort1: %d, neighborPort2: %d\n", port, neighborPort1, neighborPort2);

        struct addressInfo serverAddress = { .IPAddress = {127, 0, 0, 1}, .port = port };
        struct addressInfo neighborAddress1 = { .IPAddress = {127, 0, 0, 1}, .port = neighborPort1 };
        struct addressInfo neighborAddress2 = { .IPAddress = {127, 0, 0, 1}, .port = neighborPort2 };

        std::array<addressInfo,3> response = {serverAddress, neighborAddress1, neighborAddress2};

        std::array<uint8_t, 18> responseMessage;
        memcpy(&responseMessage, &response, 3*sizeof(addressInfo));
        asio::write(socket, asio::buffer(responseMessage), error);
    }

    //return 0;
}