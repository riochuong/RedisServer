
#ifndef __SERVER_H__
#define __SERVER_H__
#include <cstdint>
#include <string>
#include <memory>
#include <asio.hpp>
#include <unordered_map>

#include "logging.h"
#include "resp_protocol/command_parser.h"

using asio::ip::tcp;

namespace RedisServer{


    enum ClientCommandType {
        C_PING = 0,
        C_ECHO
    };

    enum ServerError {
        kOk = 0,
        kInvalidCommand

    };

    static const std::unordered_map<std::string, ClientCommandType> gStringCmdToEnumCmd {
        {"PING", ClientCommandType::C_PING},
        {"ECHO", ClientCommandType::C_ECHO},
    };

    /*! \brief Represent each connection from client 
     *
    */
    template<typename TCPSocket>
    class Connection {
        public:
            Connection(asio::io_context& ioc) socket(ioc){};
            HandleConnection(){
                // read command from client
                asio::async_read(
                    socket_,
                    asio::buffer(recv_cmd_),
                    [this](const asio::error& error, std::size_t byte_transferred){
                       RespProtocol::ParserError ec {RespProtocol::ParserError::kOk};
                       auto commands = RespProtocol::CommandParser::Parse(this->recv_cmd_, ec);
                       if (ec != RespProtocol::ParserError::kOk) {
                            logger::error("Failed to parse command from client {}", recv_cmd_);
                            return;
                       }
                       if (commands.empty()){
                            logger::info("Received empty command list ! Do nothing here");
                            return;
                       }
                       
                       bool handleAllComands = false;
                       size_t currIdx = 0;
                       while(!handleAllComands) { 
                           if(gStringCmdToEnumCmd.find(commands[currIdx]) == gStringCmdToEnumCmd.end()){
                              logger::error("Unsupported command {}", commands[currIdx]);
                              // TODO: Should we continue to next command 
                              return;
                           }
                           switch(commands[currIdx]){
                              case C_PING:
                                HandlePingCommand();
                                break;
                              case C_ECHO:
                                currIdx++;
                                if (commands.size() == currIdx){
                                    logger::error("Missing message for echo command!");
                                    return;
                                }
                                HandleEchoCommand(commands[currIdx++]);
                              default:
                                throw std::runtime_error("Unepxected command ", commands[currIdx]);                               

                           } 
                           if (currIdx == handleAllComands.size()){
                               handleAllComands = true;
                               logger::info("All command are handled properly !");
                           }
                       }

                    }
                );
            };
            GetSocket() {
                return socket_;
            }
        private:
            TCPSocket socket_;
            std::string recv_cmd_;
            
            const std::string PONG_MSG {"PONG"};
            // ----- ALL command Handlers are here for now ----
            void HandlePingCommand(void){
                logger::info("Handle Ping Command by sending back Pong message to client ");
                asio::async_write(socket_, 
                    asio::buffer(PONG_MSG),
                    [this](const asio::error& ec, size_t byte_transferred){
                        if (ec){
                            logger::error("Failed to send pong msg to client {}", ec);
                        }
                        assert(byte_transferred == PONG_MSG.size());
                        logger::info("Successfully send PONG message to client");
                    }
                );
            }
            void HandleEchoCommand(std::string msg){
                logger::info("Hnalde Echo Command with recv message {}", msg);
                asio::async_write(socket_, 
                    asio::buffer(msg),
                    [this](const asio::error& ec, size_t byte_transferred){
                        if (ec){
                            logger::error("Failed to send echo msg \"{}\" to client {}", msg, ec);
                        }
                        assert(byte_transferred == msg.size());
                        logger::info("Successfully send echo message \"{}\" to client", msg);
                    }
                );
 
            }


    };

    /*! \brief       Implement a asynchronous TCP server accepting client commands via
     *
    */
    template<typename TCPAcceptor, typename TCPSocket>
    class Server {
        public:

            Server(uint32_t port, uint32_t num_thread_workers = 10): port_(port),
                                                                     num_thread_workers_(num_thread_workers), 
                                                                     ioc_(),
                                                                     acceptor_(ioc_, tcp::endpoint(tcp::v4, port),
                                                                     pool_(num_thread_workers_)
                                                                     ){};

            /*! \brief  Start a TCP server accepting client connectiosn at specified port number port_
            */
            void StartAccepting() {
                // initialize async server and wait for client commands
                logger::info("Accepting new connection !");
                std::make_shared<Connection<TCPSocket>> conn {ioc_};
                acceptor_.async_accept(
                   conn->GetSocket();
                   [conn, this](const asio::error& error){
                        asio::post(
                            pool_,  
                            [](shared_ptr<Connection<TCPSocket>> conn){
                                logger::info("Handle Client Connection Async");
                                conn->HandleConnection();
                            },
                            conn
                        );
                        // back to accepting 
                        this->StartAccepting();
                   }
                );
                
            };

            /*! \brief Start server accepting client connection at initialized port port_ with threadpool workers
             *  
            */
            void Run(){
                logger::info("Start Server Listening at port {} with {} workers", port_, num_thread_workers_);
                ioc_.run(); 
                this->StartAccepting();
            }

        private:
            uint32_t port_;
            uint32_t num_thread_workers_;
            asio::io_context ioc_;
            TCPAcceptor acceptor_;
            asio::thread_pool pool_;
            void StartAccepting();

    };

}

#endif // __SERVER_H__