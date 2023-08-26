
#include <asio.hpp>
#include "server.h"

using namespace RedisServer;
using asio::ip::tcp;

int main(void){
    Server<tcp::acceptor, tcp::socket, AsioAsyncReadWriteWrapper>(6073, 100).Run();   
}