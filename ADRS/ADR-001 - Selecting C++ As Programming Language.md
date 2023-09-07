## ADR 001 - Select Programming Language for Redis Server Project

#### Status
Accepted on 2023 - 08 - 07
 
#### Context
There is a need for selecting a programming langugage that support network programming for the Redis Server. 
 
#### Decision
C++ is selected to be the programming language for this project
 
#### Consequences

C++ is a well-known language with strong support for Network Programing libraries such as Boost Beast library as well as testing library like Catch2 or GoogleTest.
As a result, there will be good amount of example and refereces to get familiar with libary of choice. 
C++ is a high-performance language that support multi-thread programming so we can fully take advantage of multi-cores system.  

The downside is C++ syntax is not expressive like other language like python and debugging coule be difficult in some cases where the error messages are really hard to interpret. 
Also, there is no official package manager so integration with C++ and things like Conan could be a bit of a hurdle at the beginning.





