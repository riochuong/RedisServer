## ADR 002 - Select Data Structures to use for REDIS

#### Status
Accepted on 2023 - 08 - 07
 
#### Context
With this lite version of Redis we need to implement an efficient method to store/retrieve key-value pairs.
The data-structure plays an important role here to guarantee peformance and consistency of data.

#### Decision
Selected Data Structures:

- `std::string` - storing key and string value and represet int value as Redis does not have a dedicated integer type.
- `std::list` - storing list of string value
- `std::unordered_map` - main databse to keep track of all stored key/value pair.
- `uint64_t` - storing inserted timestamp and expired timestamp 
 
#### Consequences
- `std::unordered_map` might not have good performance with sequences of ordered keys but work well with random 
key insert. 
- Need some lock mechanism or serialized access via event queue in order to avoid data corruption. 
