# Distributed Hash Table



### How to use the DHT
First compile the program (from top level directory):
```bash
$ mkdir build && cd build
$ cmake ..
$ make
```

There are three executables: mainServer, server, and client.

First start the mainServer from the build directory:
```bash
$ ./mainServer
```

Next spin up some servers:
```bash
$ ./server
```

By default 10 servers are created in 10 different threads, but if you wish to set the number of servers to start you can run:
```bash
$ ./server <number of servers to create>
```

If you want to add a server and specify its port (default IP is 127.0.0.1):
```bash
$ ./server add_server <port> 
```

Now that are servers are up and have joined the DHT we can start the client and interact with the DHT. We can insert key value pairs or search for key (which returns its value) or delete key value pairs:
```bash
$ ./client insert <key> <value>
$ ./client search <key> 
$ ./client delete <key> 
```

To test the server with a lot of keys, we have created the ability to range insert and range search for keys. 
```bash
$ ./client range_insert <max key>
$ ./client range_search <max key>
```
Keyword range_insert inserts key value pairs from zero to specified max key such that the first <key, value> pair it inserts is <0, 0> and the last is <max key, max key>. Keyword range_search searches for keys in range zero to max key.


The simplest way to implement a server leaving the DHT was to initiate that from the client. We do this by:
```bash
$ ./client rm_server <port of server to be removed> 
```
See Main Server terminal output to see which servers are currently active. 

To add a server to the DHT:
```bash
$ ./server add_server <port of server to be added>
```

When a server is added to the DHT that requires redistribution of all of the DHT data, and there are currently a lot of key-value pairs in the DHT, you may need to wait a few seconds for the keys to be redistributed before interacting with the server as that will likely result in undefined behavior. Also note: you can execute the client back to back since it does not hang while it is active like mainServer and server.

Here is an example of a possible test of the DHT (opening multiple terminal windows):
```bash
$ ./mainServer
$ ./server 
$ ./client range_insert 300
$ ./client range_search 300
$ ./client rm_server 3002
$ ./client range_search 300
$ ./client rm_server 3003
$ ./client rm_server 3007
$ ./client range_search 300
$ ./server add_server 3002
$ ./server add_server 3456
$ ./client range_search 300
```

In each case, “./client range_search 300” should return with no error messages indicating that each key-value pair was located exactly where it was expected to be based on our DHT implementation. 

## Project Overview
A distributed hash table essentially is a hash table that is distributed over a network of servers. In our implementation we have a central server (we’ve called it Main Server, which acts like a tracker server in a traditional P2P network) that is responsible for maintaining the DHT and keeping a record of servers that are currently participating in the DHT. This is a format in which we are able to store data and retrieve data easily from multiple different servers. The server’s themselves store key value (int, int) pairs on local hashmaps. 

A client interacts with the DHT by requesting a server address from the Main Server given an integer key. The client will be able to insert, search for, and delete information (with a unique key). In each of these DHT interactions, the key is first sent to the Main Server, where it will be used to determine the server that contains this information. The Main Server will then return to the client the 3 server addresses (ip address + port) of the server to which the client will forward the request. The first of the three servers is the server that is primarily responsible for the given key, while the other two servers will be used for data redundancy.

Now that the client has the address information of the server responsible for the key (as well as its two neighbors), the client will now send the key value pair to the three servers. When all is done, the server will return a message based upon the given command, for example, if the client was searching for the value of a key the server that contains that information will return the value associated with the key.

