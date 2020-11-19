#include <inttypes.h>
#include <asio.hpp>
#include <ctime>
#include <iostream>
#include <string>
#include <unordered_map>

using asio::ip::tcp;

/* The Main Server 

This server acts as a tracker server in a traditional P2P network. The  client first communicates with the tacker server 
to determine which server in the DHT network it needs to communicate with.

The message format will be a 4 byte message that contains the key of type int (4 byte, 32 bits).

An array of 4 elements, each element is a uint8_t (8 bits).

[31 ... 0], such that the most significant bit is the on the left.

*/
std::vector<int> availableServers;
int activeServers;

int main_server_decode(std::array<uint8_t, 4>* message) {
    int array_index = 0; // remember that the array is of lenght 4, not 4 * 8.
    int bit_index = 0;   // which bit within each element of the array we are refering to
    int ret = 0;         // what we will return

    for (int i = 0; i < 32; ++i) {
        ret |= (((*message)[array_index] >> bit_index) & 1) << (32 - i);  

        if ((i + 1) % 8 == 0) {
             ++array_index;
            bit_index = 0;
        }
        else {
            ++bit_index;
        }
    }
    return ret;
}

void main_server_encode(std::array<uint8_t, 4>* message, int port) {
    int array_index = 0; // remember that the array is of lenght 4, not 4 * 8.
    int bit_index = 0;   // which bit within each element of the array we are refering to

    for (int i = 0; i < 32; ++i) {
        (*message)[array_index] &= ~(1 << bit_index);
        (*message)[array_index] |= ((port >> (32-i)) & 1) << bit_index;

        if ((i + 1) % 8 == 0) {
            ++array_index;
            bit_index = 0;
        }
        else {
            ++bit_index;
        }
    }
    return;
}

void rehash(){
    activeServers++;
    //Get all keys from all servers and rehash them with new value
    //Need a way to get all items from each server
}

int proxy(std::array<uint8_t, 4>* message){
    int server; //index the pos of bits that are the given key
    std::cout << "Send to server " << server % activeServers << std::endl;
    return server % activeServers;
} 

int main() {
  
    for(int i = 1; i < 4; i++){
        availableServers.push_back(3000 + i);
    }

    activeServers = 2;

    std::array<uint8_t, 4> message;
    
    main_server_encode(&message, 3001);
    int port = main_server_decode(&message);
    std::cout << "The port is: " << port << std::endl;


    asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 3000));

    // Use this bakery to handle queries from the client
    // Bakery bakery = text_deserializer("../data/bakery.txt");

    uint16_t counter = 0;

    while (true) {
        // Wait for client
        std::cout << "Blocked for read" << std::endl;
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        std::array<uint8_t, 3> buf;
        asio::error_code error;
        size_t len = socket.read_some(asio::buffer(buf), error);

        // Example of error handling
        // if (error != asio::error::eof)
        //   throw asio::system_error(error);

        // Add x to counter
        auto x = uint8_t(buf[0]);
        counter += x;
        std::cout << +x << " " << counter << std::endl;

        buf.fill(0);

        std::memcpy(&buf, &counter, sizeof(uint16_t));

        asio::write(socket, asio::buffer(buf), error);
    }

    //return 0;
}