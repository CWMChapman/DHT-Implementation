#include <inttypes.h>
#include <asio.hpp>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <algorithm>
#include <unordered_map>

#include "../include/functions.hpp"

using asio::ip::tcp;

/* The Tracker Server (Main Server)

This server acts as a tracker server in a traditional P2P network. The  client first communicates with the tacker server 
to determine which server in the DHT network it needs to communicate with.

The message format will be a 4 byte message that contains the key of type int (4 byte, 32 bits).


Tracker Server Port is 3000
Port 3001 is open waiting for servers to message about joining or quiting 

*/



struct KeyHasher {
  std::size_t operator()(const addressInfo& k) const {
    using std::size_t;
    using std::hash;
    using std::string;

    return ((hash<string>()(ip_tostr(k.IPAddress))
             ^ (hash<short>()(k.port) << 1)) >> 1);
  }
};

// Special Unordered maps:
// https://stackoverflow.com/questions/17016175/c-unordered-map-using-a-custom-class-type-as-the-key
std::unordered_map<addressInfo, addressInfo, KeyHasher> serverOutageMap;
std::vector<addressInfo> allServers;
std::vector<addressInfo> activeServers; 



bool isServerDown(addressInfo key){
    if (serverOutageMap.empty()){
        // std::cout << "SERVER OUTAGE MAP IS EMPTY" << std::endl;
        return false;
    }
    if (serverOutageMap.find(key) != serverOutageMap.end()){
        // std::cout << "FOUND KEY IN SERVER OUTAGE MAP" << std::endl;
        return true;
    }
    // std::cout << "KEY IS NOT IN SERVER OUTAGE MAP" << std::endl;
    return false;
}

bool isServerActive(addressInfo server) {
    if(std::find(activeServers.begin(), activeServers.end(), server) != activeServers.end()) {
        return true;
    } else {
        return false;
    }
}
int getNumActiveServers() {
    return activeServers.size();
}
int getNumAllServers() {
    return allServers.size();
}
void printActiveServers() {
    if (getNumActiveServers() > 0) {
        std::cout << "ACTIVE SERVERS: (" << getNumActiveServers() << " active)" << std::endl;
        std::cout << "=============================" << std::endl;
        std::cout << "Server\t|    Address" << std::endl;
        for(int i=0; i < activeServers.size(); i++)
            std::cout << "   " << i << "\t|    " << ip_tostr(activeServers.at(i).IPAddress) << ":" << activeServers.at(i).port << std::endl;
        std::cout << "=============================" << std::endl;
    }
    else std::cout << "NO ACTIVE SERVERS" << std::endl;
    return;
}
void printAllServers() {
    if (getNumAllServers() > 0) {
        std::cout << "All SERVERS: (" << getNumAllServers() << " total)" << std::endl;
        std::cout << "=============================" << std::endl;
        std::cout << "Server\t|    Address" << std::endl;
        for(int i=0; i < allServers.size(); i++)
            std::cout << "   " << i << "\t|    " << ip_tostr(allServers.at(i).IPAddress) << ":" << allServers.at(i).port << std::endl;
        std::cout << "=============================" << std::endl;
    }
    else std::cout << "NO ACTIVE SERVERS" << std::endl;
    return;
}

addressInfo getServer(int key) {
    addressInfo primaryServer = allServers[hash(key) % getNumAllServers()];
    // std::cout << "GETTING SERVER ... \t" << addressInfo_tostr(primaryServer) << std::endl;
    if (isServerDown(primaryServer)) {
        // printf("\nSERVER IS DOWN\n");
        addressInfo backupServer = serverOutageMap[primaryServer];
        // std::cout << "server: " << addressInfo_tostr(backupServer) << std::endl;
        while (isServerDown(backupServer)) {
            backupServer = serverOutageMap.at(backupServer);
            // std::cout << "inside_server: " << addressInfo_tostr(backupServer) << std::endl;
        }
        return backupServer;
    }
    else return primaryServer;
}

addressInfo getNeighborServer1(int key) {
    int primaryServerindex;
    for (primaryServerindex = 0; primaryServerindex < getNumActiveServers(); primaryServerindex++)
        if (activeServers[primaryServerindex] == getServer(key)) break;
    int neighbor1Index = primaryServerindex - 1;
    if (neighbor1Index == -1) neighbor1Index = getNumActiveServers() - 1;
    return activeServers[neighbor1Index];
}

addressInfo getNeighborServer2(int key) {
    int primaryServerindex;
    for (primaryServerindex = 0; primaryServerindex < getNumActiveServers(); primaryServerindex++)
        if (activeServers[primaryServerindex] == getServer(key)) break;
    int neighbor2Index = primaryServerindex + 1;
    if (neighbor2Index == getNumActiveServers()) neighbor2Index = 0;
    return activeServers[neighbor2Index];
}


void addServer(addressInfo server) {
    if (!isServerActive(server)) {
        activeServers.push_back(server);
        allServers.push_back(server);

        // every time a server is added, we must rehash all keys in the DHT, assumption is that this doesnt happen often
        // in the beginning, when they all join, there will be no keys on the servers to begin with, so nothing will happen
        // for (addressInfo server : activeServers) {
        //     Server_Request(server, (DHT_action){.action = 4}); // tell the server to rehash its keys
        // }

        for(int i=0; i < activeServers.size(); i++)
            Server_Request(activeServers.at(i), (DHT_action){.action = 4});


        printf("FINISHED ADDING THE SERVER!\n\n\n");
        return;
    }
    printf("SERVER ALREADY IN DHT!\n");
    return;
}

void deleteServer(addressInfo serverToDelete) {
    if (!isServerActive(serverToDelete)) {
        std::cout << "SERVER IS NOT ACTIVE AND THEREFORE CANNOT BE DELETED!" << std::endl;
        return;
    }

    int index;
    for (index = 0; index < getNumActiveServers(); index++)
        if (activeServers[index] == serverToDelete) break;
    activeServers.erase(activeServers.begin() + index); 

    int neighbor1Index = index-1; // now that the server being deleted is gone, neighbor 2 replaces it and we must rehash it so its new neighbors redudantly hold its keys
    int neighbor2Index = index; // now that the server being deleted is gone, neighbor 2 replaces it and we must rehash it so its new neighbors redudantly hold its keys
    if (neighbor1Index == -1) neighbor1Index = getNumActiveServers() - 1; // loop to front if you're incrementing past the last vetor in server
    if (neighbor2Index == getNumActiveServers()) neighbor2Index = 0; // loop to front if you're incrementing past the last vetor in server
    addressInfo neighbor1 = activeServers[neighbor1Index];
    addressInfo neighbor2 = activeServers[neighbor2Index];

    serverOutageMap[serverToDelete] = neighbor2; // set neighbor1 to be backup for deleted server in the serveroutagemap
    Server_Request(neighbor1, (DHT_action){.action = 4}); // tell the server to rehash its keys
    Server_Request(neighbor2, (DHT_action){.action = 4}); // tell the server to rehash its keys
    
    printf("FINISHED DELETING THE SERVER!\n");
    std::cout << "main: " << addressInfo_tostr(neighbor2) << std::endl;
    std::cout << "newPrimary: " << addressInfo_tostr(getServer(1)) << std::endl;
    std::cout << "newN1: " << addressInfo_tostr(getNeighborServer1(1)) << std::endl;
    std::cout << "newN2: " << addressInfo_tostr(getNeighborServer2(1)) << std::endl;
    printActiveServers();

    return;
}



/* ***     PORT 3001     *** */

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
        // std::cout << "recieved message with action " << message.action << " from server " << addressInfo_tostr(message.info) << std::endl;
        
        if (message.action == 0) addServer(message.info);
        else if (message.action == 1) deleteServer(message.info);
        // printf("finsihed add/delete server in serverlistener thread\n");

        printActiveServers();
    }
    
    return;
}


/* ***     PORT 3000     *** */

int main() {
    // listen for servers joining and leaving
    std::thread serverListenerThread (listenForServers); 

    asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 3000));

    while (true) {
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        std::array<uint8_t, 4> buf;
        asio::error_code error;
        size_t len = socket.read_some(asio::buffer(buf), error);
        
        int key;
        memcpy(&key, &buf, sizeof(int));
        addressInfo primary = getServer(key);
        addressInfo neighbor1 = getNeighborServer1(key);
        addressInfo neighbor2 =  getNeighborServer2(key);
        // std::cout << "primary: " << addressInfo_tostr(primary) << ", n1: " << addressInfo_tostr(neighbor1) << ", n2: " << addressInfo_tostr(neighbor2) << std::endl;
        
        std::array<addressInfo,3> response = {primary, neighbor1, neighbor2};

        std::array<uint8_t, 3*sizeof(addressInfo)> responseMessage;
        memcpy(&responseMessage, &response, 3*sizeof(addressInfo));

        asio::write(socket, asio::buffer(responseMessage), error);
    }

    return 0;
}




// void rehash() { 
//     activeServers++;
//     //Get all keys from all servers and rehash them with new value
//     //Need a way to get all items from each server
// }


// void initDHT() {
// 	// initializes the DHTin servers with data
// 	printf("INITIALIZING DHT\n");
	
// 	return;
// }

// void testServerOutageMap() {
//     addressInfo test1 = {.IPAddress = {1, 2, 3, 4}, .port = 1234};
//     addressInfo test2 = {.IPAddress = {4, 3, 2, 1}, .port = 4321};
//     serverOutageMap[test1] = test2;

//     std::cout << "First checking if the server is down: " << std::endl;
//     if (isServerDown(test1)) std::cout << "SERVER IS DOWN" << std::endl;
//     if (serverOutageMap.empty()) std::cout << "SERVER IS EMPTY" << std::endl;
//     else std::cout << "SERVER IS NOT EMPTY" << std::endl;

//     std::cout << "Next checking if we can get the value for: " << addressInfo_tostr(test1) << std::endl;
//     std::cout << "Got: " << addressInfo_tostr(serverOutageMap[test1]) << std::endl;



//     return;
// }