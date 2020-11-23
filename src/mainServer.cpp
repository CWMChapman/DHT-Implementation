#include <inttypes.h>
#include <asio.hpp>
#include <ctime>
#include <iostream>
#include <string>
#include <unordered_map>

#include "../include/functions.hpp"

using asio::ip::tcp;

/* The Tracker Server (Main Server)

This server acts as a tracker server in a traditional P2P network. The  client first communicates with the tacker server 
to determine which server in the DHT network it needs to communicate with.

The message format will be a 4 byte message that contains the key of type int (4 byte, 32 bits).

An array of 4 elements, each element is a uint8_t (8 bits).

[31 ... 0], such that the most significant bit is the on the left.

*/
std::vector<int> availableServers;
int activeServers;

/* EXAMPLE ENCODE DECODE USAGE
std::array<uint8_t, 4> message;
main_server_encode(&message, 3001);
int port = main_server_decode(&message);
*/

/* EXAMPLE hashToServerPort USAGE
int key = 5;
int numServers = 10;
int port = hashToServerPort(key, numServers);


for (int i = 0; i < 100; ++i) {
    printf("i: %d, port: %d\n", i, hashToServerPort(i, 10));
}
*/

void rehash(){
    activeServers++;
    //Get all keys from all servers and rehash them with new value
    //Need a way to get all items from each server
}

/* ***     PORT 3000     *** */

int main() {
    asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 3000));

    // Use this bakery to handle queries from the client
    // Bakery bakery = text_deserializer("../data/bakery.txt");

    uint16_t counter = 0;

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
        int port = hashToServerPort(key, numServers);

        int neighborPort1 = port - 1; 
        int neighborPort2 = port + 1;

        if (port - 1 == 3001 - 1) {
            neighborPort1 = 3001 + numServers - 1;
        }
        if (port + 1 == numServers + 3001) {
            neighborPort2 = 3001;
        }

        printf("port: %d, neighborPort1: %d, neighborPort2: %d\n", port, neighborPort1, neighborPort2);

        memcpy(&buf, &port, sizeof(int));
        asio::write(socket, asio::buffer(buf), error);
    }

    //return 0;
}