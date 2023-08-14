
#include <asio.hpp>
#include "server.h"

using namespace RedisServer;
using asio::ip::tcp;

int main(void){
    Server<tcp::socket, tcp::acceptor>(6073, 100).run();   
}