#ifndef functions
#define functions

#include <array>
#include <iostream>

int main_server_decode(std::array<uint8_t, 4>* message); // FOR MAIN/TRACKER SERVER: decode incoming message into an int
void main_server_encode(std::array<uint8_t, 4>* message, int port); // FOR MAIN/TRACKER SERVER: encode port into an message for TCP

int decode_9byte(std::array<uint8_t, 9> *message, int section); // decodes TCP message into its different compoents
void encode_9byte(std::array<uint8_t, 9> *message, int action, int key, int value); // encodes action, key, and value into a tcp message

uint64_t xorshift(const uint64_t& n,int i); // helper for hash function
uint64_t hash(const uint64_t& n); // hash function
int hashToServerPort(int key, int numServers); // return the server port based on key

int getNumServers(); // get the number of active servers

#endif