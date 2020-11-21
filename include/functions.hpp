#ifndef functions
#define functions

#include <array>
#include <iostream>

int main_server_decode(std::array<uint8_t, 4>* message);
void main_server_encode(std::array<uint8_t, 4>* message, int port);

int decode_9byte(std::array<uint8_t, 9> *message, int section);
void encode_9byte(std::array<uint8_t, 9> *message, int action, int key, int value);


#endif