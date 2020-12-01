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
        std::cout << "SERVER OUTAGE MAP IS EMPTY" << std::endl;
        return false;
    }
    if (serverOutageMap.find(key) != serverOutageMap.end()){
        std::cout << "FOUND KEY IN SERVER OUTAGE MAP" << std::endl;
        return true;
    }
    std::cout << "KEY IS NOT IN SERVER OUTAGE MAP" << std::endl;
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
        std::cout << "ACTIVE SERVERS: " << std::endl;
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
        std::cout << "All SERVERS: " << std::endl;
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
    std::cout << "GETTING SERVER ... \t" << addressInfo_tostr(primaryServer) << std::endl;
    if (isServerDown(primaryServer)) {
        printf("\nSERVER IS DOWN\n");
        addressInfo backupServer = serverOutageMap[primaryServer];
        while (isServerDown(backupServer)) {
            backupServer = serverOutageMap[backupServer];
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




void addServer(addressInfo server) {
    if (!isServerActive(server)) {
        activeServers.push_back(server);
        allServers.push_back(server);
    }
}
void deleteServer(addressInfo serverInfo) {
    int index;
    for (index = 0; index < getNumActiveServers(); index++)
        if (activeServers[index] == serverInfo) break;
    int neighbor1Index = index - 1;
    int neighbor2Index = index + 1;
    if (neighbor1Index == -1) neighbor1Index = getNumActiveServers() - 1;
    if (neighbor2Index == getNumActiveServers()) neighbor2Index = 0;
    addressInfo neighbor1 = activeServers[neighbor1Index];
    addressInfo neighbor2 = activeServers[neighbor2Index];
    // std::cout << "Setting serverOutageMap[ " << addressInfo_tostr(serverInfo) << " ] = " << addressInfo_tostr(neighbor1) << std::endl;
    serverOutageMap[serverInfo] = neighbor1; // set neighbor1 to be backup for deleted server in the serveroutagemap
    // std::cout << "Setting serverOutageMap[ " << addressInfo_tostr(serverInfo) << " ] = " << addressInfo_tostr(serverOutageMap[serverInfo]) << std::endl;
    activeServers.erase(activeServers.begin() + index); 
    // std::thread rehashN1 (Server_Request, neighbor1, (DHT_action){.action = 4, .key = 0, .value = 0});
    // std::thread rehashN2 (Server_Request, neighbor2, (DHT_action){.action = 4, .key = 0, .value = 0});
    // rehashN1.join();
    // rehashN2.join();
    
    Server_Request(neighbor1, (DHT_action){.action = 4}); // tell the server to rehash its keys
    Server_Request(neighbor2, (DHT_action){.action = 4});
    
    printActiveServers();

    return;
}



/* ***     PORT 3001     *** */

void listenForServers() {
    asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 3001));

    while (true) {
        tcp::socket socket(io_context);
        // std::cout << "l-test1" << std::endl;
        acceptor.accept(socket);

        std::array<uint8_t, 8> buf;
        asio::error_code error;
        size_t len = socket.read_some(asio::buffer(buf), error);
        // std::cout << "l-test2" << std::endl;
        
        serverAction message;
        memcpy(&message, &buf, sizeof(serverAction));
        // std::cout << "l-test3: message.action: " << message.action << std::endl;
        // short action = message.action;
        // std::cout << message.action << std::endl;
        
        if (message.action == 0) addServer(message.info);
        else{
            deleteServer(message.info);
        } 
        std::cout << "l-test4" << std::endl;

        if (activeServers.size() >= 10) {
            printActiveServers();
            // printAllServers();
        }
    }
    
    return;
}


/* ***     PORT 3000     *** */

int main() {
    std::thread serverListenerThread (listenForServers);

    asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 3000));

    

    while (true) {
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        std::array<uint8_t, 4> buf;
        asio::error_code error;
        std::cout << "about to read message in" << std::endl;
        size_t len = socket.read_some(asio::buffer(buf), error);
        std::cout << "just read message in" << std::endl;
        
        int key;
        memcpy(&key, &buf, sizeof(int));
        addressInfo primary = getServer(key);
        addressInfo neighbor1 = getNeighborServer1(key);
        addressInfo neighbor2 =  getNeighborServer2(key);
        std::cout << "primary: " << addressInfo_tostr(primary) << ", n1: " << addressInfo_tostr(neighbor1) << ", n2: " << addressInfo_tostr(neighbor2) << std::endl;
        
        std::array<addressInfo,3> response = {primary, neighbor1, neighbor2};
        // std::cout << "test1" << std::endl;

        std::array<uint8_t, 3*sizeof(addressInfo)> responseMessage;
        // std::cout << "test2" << std::endl;
        memcpy(&responseMessage, &response, 3*sizeof(addressInfo));

        // std::cout << "test3" << std::endl;
        asio::write(socket, asio::buffer(responseMessage), error);
        // std::cout << "test4" << std::endl;
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

// bool isEqual(addressInfo k1, addressInfo k2) {
//     if(k1.IPAddress == k2.IPAddress && k1.port == k2.port) return true;
//     else return false;
// }

// bool operator==(const MyStruct1& lhs, const MyStruct1& rhs)
// {
//     return /* your comparison code goes here */
// }

// {
//         bool operator==(const addressInfo &k1, const addressInfo &k2) const
//         { return (k1.IPAddress == k2.IPAddress
//                     && k1.port == k2.port);
//         }
// };

