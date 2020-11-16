# DHT-Implementation
CPS373 -- Professor Brad McDanel

Juan Huerta and Christopher Chapman 

### Project:
Implement a [Distributed Hash Table (DHT)](https://www.coursera.org/lecture/data-structures/distributed-hash-tables-tvH8H)

### Thoughts:

So I'm thinking we're gonna have a server that the client queries to determine which server has the data that the client wants (based on the key), or it determines which server the <key,value> pairs belongs in (based on the key). Almost like the P2P tracker server that keeps track of the IP addresses. 


The "tracker" server is responsible for all of the interactions between the client and the DHT servers.



## To-Do:
1. Create this tracker (proxy) server
  1. Forward the message to the server based on the 32 bit key.
2. Create this hash function that determines which server to look for the key value pair or to put the key value pair.


## Message Type
2 bit message type, 32 bit key, 32 bit value


M is a 2-bit message type, corresponding to 4 possible messages given shortly, K is a 32-bit key, V is a 32-bit value. The four message types in the API are:
* M=00: Insert
* M=01: Lookup
* M=10: Delete
* M=11: Do Nothing. For later, maybe we do this to reorganize the servers keys and redistribute things in case a server is added or removed.

## Proxy Server Message Type
K is a 32-bit key. Whole message is just 4 bytes.


## Encode and Decode Message
#### Section:
* 0 = message action
* 1 = key
* 2 = value


int main_server_decode(std::array<uint8_t, 4>* message) { /* *** */ }

void main_server_encode(std::array<uint8_t, 4>* message, int port) { /* *** */ }


int decode_9byte(std::array<uint8_t, 9> *message, int section) { /* *** */ }

void encode_9byte(std::array<uint8_t, 9> *message, int action, int key, int value) { /* *** */ }

## Hash Function
int hash(int key) { 
  return key % numServers;
}

## Tracker (Proxy) Server
Gets the message, find the key, return back to the client the port of the server. Client will then use that port to connect to the actual server.
Return message with port number so its still a 4 byte message. 

int ports [numServers];






