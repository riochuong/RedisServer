
#include <memory>
#include <catch2/catch_test_macros.hpp>
#include "server.h"
#include "async_read_write_mock.h"
#include "asio_test_client.h"
#include "resp_protocol/command_parser.h"

using namespace std::string_literals;
using MockServer = RedisServer::Server<MockAcceptor, MockSocket, MockAsyncReadWrite> ;
using MockConnection = RedisServer::Connection<MockSocket, MockAsyncReadWrite>;

bool WaitForRecvWriteMessage(int num_trials, std::string_view message, uint32_t cycle_wait_s = 1){
    for (int trial = 1; trial <= num_trials; trial++){
        logger::info("Wait for recv message to be {}. trial {}/{}", message, trial, num_trials);
        if (MockAsyncReadWrite::messageToWrite == message){
            return true;
        }
        logger::info("Recv msg to write {}", MockAsyncReadWrite::messageToWrite);
        sleep(cycle_wait_s);
    }
    return false;
}


// TEST_CASE("Test PING Command","Test Mock Server"){

//     MockAsyncReadWrite::setMessageToRead("*1\r\n$4\r\nPING\r\n"s);     
//     auto mock_server = MockServer(1234, 5, false);
//     mock_server.Run();
//     int num_trials = 2;
//     REQUIRE(WaitForRecvWriteMessage(2, "*1\r\n$4\r\nPONG\r\n"s, 1));    
// }

// TEST_CASE("Test ECHO command basic", "Test Mock Server"){

//     MockAsyncReadWrite::setMessageToRead("*2\r\n$4\r\nECHO\r\n$5\r\nTest1\r\n"s);     
//     auto mock_server = MockServer(1234, 5, false);
//     mock_server.Run();
//     REQUIRE(WaitForRecvWriteMessage(2, "*1\r\n$5\r\nTest1\r\n"s, 1));    
// }


// TEST_CASE("Test ECHO command long", "Test Mock Server"){

//     MockAsyncReadWrite::setMessageToRead("*2\r\n$4\r\nECHO\r\n$20\r\nTest12345678910 awsn\r\n"s);     
//     auto mock_server = MockServer(1234, 5, false);
//     mock_server.Run();
//     REQUIRE(WaitForRecvWriteMessage(2, "*1\r\n$20\r\nTest12345678910 awsn\r\n"s, 1));    
// }

// ======= INTEGRATED TESTING WITH REAL SERVER ===================================
using RealRedisServer =  RedisServer::Server<asio::ip::tcp::acceptor, 
                                             asio::ip::tcp::socket, 
                                             RedisServer::AsioAsyncReadWriteWrapper>;

std::shared_ptr<RealRedisServer> StartRealRedisServer(uint16_t port_number, std::string server_addr) {
    auto server = std::make_shared<RealRedisServer>(port_number, 5, true);
    std::thread st {
        [server](){
            server->Run();
        }
    };
    st.detach();
    while(!server->IsRunning()){
        sleep(1);
    }
    return server;
}

// TEST_CASE("Test PING Command with real server", "Test Real Server"){
//     uint16_t port_number = 5551;
//     std::string server_addr = "127.0.0.1"; // localhost address
//     logger::info("ready for client !!!!!");
//     TestClient tc {port_number, server_addr};

//     auto server = StartRealRedisServer(port_number, server_addr); 

//     REQUIRE(tc.Connect());
//     REQUIRE(tc.Send("*1\r\n$4\r\nPING\r\n"s));
//     std::string recv = "";
//     REQUIRE(tc.Recv(recv));
//     REQUIRE(recv == "*1\r\n$4\r\nPONG\r\n"s);
//     RedisServer::RespProtocol::ParserError ec = RedisServer::RespProtocol::ParserError::kOk;
//     auto cmds = RedisServer::RespProtocol::CommandParser::Parse(recv, ec);
//     REQUIRE(ec == RedisServer::RespProtocol::ParserError::kOk);
//     REQUIRE(cmds.size() == 1);
//     REQUIRE(cmds[0] == "PONG");
//     server->Stop();
// }

// TEST_CASE("Test basic short ECHO Command with real server", "Test Real Server"){
//     uint16_t port_number = 5552;
//     std::string server_addr = "127.0.0.1"; // localhost address
//     logger::info("ready for client !!!!!");
//     TestClient tc {port_number, server_addr};

//     auto server = StartRealRedisServer(port_number, server_addr); 

//     REQUIRE(tc.Connect());
//     REQUIRE(tc.Send("*2\r\n$4\r\nECHO\r\n$5\r\n1234c\r\n"s));
//     std::string recv = "";
//     REQUIRE(tc.Recv(recv));
//     REQUIRE(recv == "*1\r\n$5\r\n1234c\r\n"s);
//     RedisServer::RespProtocol::ParserError ec = RedisServer::RespProtocol::ParserError::kOk;
//     auto cmds = RedisServer::RespProtocol::CommandParser::Parse(recv, ec);
//     REQUIRE(ec == RedisServer::RespProtocol::ParserError::kOk);
//     REQUIRE(cmds.size() == 1);
//     REQUIRE(cmds[0] == "1234c");
//     server->Stop();
// }

// TEST_CASE("Test basic long ECHO Command with real server", "Test Real Server"){
//     uint16_t port_number = 5553;
//     std::string server_addr = "127.0.0.1"; // localhost address
//     logger::info("ready for client !!!!!");
//     TestClient tc {port_number, server_addr};

//     auto server = StartRealRedisServer(port_number, server_addr); 

//     REQUIRE(tc.Connect());
//     REQUIRE(tc.Send("*2\r\n$4\r\nECHO\r\n$21\r\n12345678901234567890c\r\n"s));
//     std::string recv = "";
//     REQUIRE(tc.Recv(recv));
//     REQUIRE(recv == "*1\r\n$21\r\n12345678901234567890c\r\n"s);
//     RedisServer::RespProtocol::ParserError ec = RedisServer::RespProtocol::ParserError::kOk;
//     auto cmds = RedisServer::RespProtocol::CommandParser::Parse(recv, ec);
//     REQUIRE(ec == RedisServer::RespProtocol::ParserError::kOk);
//     REQUIRE(cmds.size() == 1);
//     REQUIRE(cmds[0] == "12345678901234567890c");
//     server->Stop();
// }


TEST_CASE("Test basic PING Command with redis cli", "Test Real Server RedisCLI"){
    uint16_t port_number = 5555;
    std::string server_addr = "127.0.0.1"; // localhost address
    logger::info("ready for client !!!!!");
    TestClient tc {port_number, server_addr};
    auto server = StartRealRedisServer(port_number, server_addr); 
    std::string cmd {
        "echo -n $(/usr/bin/redis-cli -p " + std::to_string(port_number) + " PING" +")"
    };
    std::string output = "";
    REQUIRE(ExecCmdWithOutput(cmd, output));
    REQUIRE(output == "PONG");
    server->Stop();
}

TEST_CASE("Test basic ECHO long Command with redis cli", "Test Real Server RedisCLI"){
    uint16_t port_number = 5556;
    std::string server_addr = "127.0.0.1"; // localhost address
    logger::info("ready for client !!!!!");
    TestClient tc {port_number, server_addr};
    auto server = StartRealRedisServer(port_number, server_addr);
    std::string echo_string {"12345678901234567890c"}; 
    std::string cmd {
        "echo -n $(/usr/bin/redis-cli -p " + std::to_string(port_number) + " ECHO " + echo_string +")"
    };
    std::string output = "";
    REQUIRE(ExecCmdWithOutput(cmd, output));
    REQUIRE(output == echo_string);
    server->Stop();
}

TEST_CASE("Stress Test ECHO command with redis cli", "Test Real Server RedisCLI"){

    uint16_t port_number = 5557;
    std::string server_addr = "127.0.0.1"; // localhost address
    logger::info("ready for client !!!!!");
    uint32_t seed = time(NULL);
    logger::info("Stress Test ECHO command with rediscli Seed: {}", seed); 
    TestClient tc {port_number, server_addr};
    auto server = StartRealRedisServer(port_number, server_addr);

    size_t num_cycles = 100;
    const size_t MAX_ECHO_CMD_SIZE = 256;
    std::string random_cmd = "";
    for (int i = 1; i <= num_cycles; i++){
        size_t cmd_size = std::rand() % MAX_ECHO_CMD_SIZE;
        logger::info("Stress test cycle {} with command size {}", i, cmd_size);
        random_cmd.clear();
        // generate random echo command 
        for (int j = 0; j < cmd_size; j++){
            char new_char = 'a' + rand() % 26; 
            random_cmd.push_back(new_char);
        }
        logger::info("Created cmd: {}", random_cmd);
        std::string cmd{
            "echo -n $(/usr/bin/redis-cli -p " + std::to_string(port_number) + " ECHO " + random_cmd + ")"};
        std::string output = "";
        REQUIRE(ExecCmdWithOutput(cmd, output));
        REQUIRE(output == random_cmd);
    }
    server->Stop();
}