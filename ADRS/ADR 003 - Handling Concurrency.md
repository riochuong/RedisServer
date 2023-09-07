## ADR 003 - Handling Concurrency Requests from Clients

#### Status
Accepted on 2023 - 09 - 07
 
#### Context
Multiple Redis client can submit request concurrently. The faster the server is able to handle concurrent request 
the better user experience and performance.

#### Decision
Each request will be accepted and assigned to a separate thread worker that part of the asio thread pool 
library. 
 
#### Consequences
In order to support concurrent clients, we also need to provide locking mechanism or serialize accesses to shared resource
like the the key/value database. As a result, this might need careful consideration and efficient implementation to avoid 
deadlock or hang situation. 




