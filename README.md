# DHT-Implementation
CPS373 -- Professor Brad McDanel

Juan Huerta and Christopher Chapman 

### Project:
Implement a [Distributed Hash Table (DHT)](https://www.coursera.org/lecture/data-structures/distributed-hash-tables-tvH8H)

### Thoughts:

So I'm thinking we're gonna have a server that the client queries to determine which server has the data that the client wants (based on the key), or it determines which server the <key,value> pairs belongs in (based on the key). Almost like the P2P tracker server that keeps track of the IP addresses. 


The "tracker" server is responsible for all of the interactions between the client and the DHT servers.



## To-Do:
1. Create this tracker server
2. Create this hash function that determines which server to look for the key value pair or to put the key value pair.
