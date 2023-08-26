
#ifndef __SERVER_H__
#define __SERVER_H__
#include <cstdint>
#include <string>
#include <memory>
#include <asio.hpp>
#include <unordered_map>

#include "logging.h"
#include "resp_protocol/command_parser.h"
#include "parsing_utils.h"

using asio::ip::tcp;
using namespace std::string_literals;
namespace RedisServer{

    class AsioAsyncReadWriteWrapper
    {
    public:

        template <
            typename AsyncReadStream,
            typename DynamicBuffer_v2>
        static std::size_t read(
            AsyncReadStream &s,
            DynamicBuffer_v2 &b,
            asio::error_code &ec)
        {
            return asio::read_until(s, 
                                    b,
                                    "\r\n", 
                                    ec);
        }

        template <
            typename AsyncWriteStream,
            typename DynamicBuffer_v2>
        static std::size_t write(
            AsyncWriteStream &s,
            const DynamicBuffer_v2 &buffers,
            asio::error_code &ec)
        {
            return asio::write(s, buffers, ec);
        }
    };

    enum ClientCommandType
    {
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
    template<typename TCPSocket, typename AsyncLib>
    class Connection {
        public:
            Connection(asio::thread_pool& exec): socket_(exec){}
           void HandleConnection(){
                // read command from client
                asio::error_code ec;
                
                if (ec){
                    logger::error("Hitting error during command read {}", ec.message());
                    return;
                }

                RespProtocol::ParserError pec{RespProtocol::ParserError::kOk};
                std::string raw_command {};

                if (!ReadRawCommandFromSocket(raw_command)){
                    logger::info("Failed to read raw command from socket !!!");
                    return;
                }

                logger::info("Raw command from client: {}", raw_command);
                auto commands = RespProtocol::CommandParser::Parse(raw_command, pec);
                if (pec != RespProtocol::ParserError::kOk)
                {
                    logger::error("Failed to parse command from client {}", raw_command);
                    return;
                }
                if (commands.empty())
                {
                    logger::info("Received empty command list ! Do nothing here");
                    return;
                }

                bool handleAllComands = false;
                size_t currIdx = 0;
                while (!handleAllComands)
                {
                    if (gStringCmdToEnumCmd.find(commands[currIdx]) == gStringCmdToEnumCmd.end())
                    {
                        logger::error("Unsupported command {}", commands[currIdx]);
                        // TODO: Should we continue to next command
                        return;
                    }
                    switch (gStringCmdToEnumCmd.at(commands[currIdx]))
                    {
                    case C_PING:
                        HandlePingCommand();
                        currIdx++;
                        break;
                    case C_ECHO:
                        currIdx++;
                        if (commands.size() == currIdx)
                        {
                            logger::error("Missing message for echo command!");
                            return;
                        }
                        HandleEchoCommand(commands[currIdx++]);
                        break;
                    default:
                        throw std::runtime_error("Unepxected command " + commands[currIdx]);
                    }
                    if (currIdx == commands.size())
                    {
                        handleAllComands = true;
                        logger::info("All commands are handled properly !");
                    }
                }
            };
            TCPSocket &GetSocket() {
                return socket_;
            }
        private:
            TCPSocket socket_;
            asio::streambuf recv_;
            const std::string PONG_MSG {"*1\r\n$4\r\nPONG\r\n"s};
            asio::const_buffers_1 PONG_MSG_BUFFER = asio::buffer(PONG_MSG);
            // ----- ALL command Handlers are here for now ----
            void HandlePingCommand(void){
                logger::info("Handle Ping Command by sending back Pong message to client ");
                asio::error_code ec {};
                std::size_t written_byte = AsyncLib::write(socket_, PONG_MSG_BUFFER, ec);
                logger::info("Num written by: {}", written_byte);
                assert(written_byte == PONG_MSG.size());
                logger::info("Successfully send PONG message to client");
            }
            void HandleEchoCommand(std::string msg){
                logger::info("Handle Echo Command with recv message {}", msg);
                asio::error_code ec {};      
                std::string reply {
                    "*1\r\n$"s + std::to_string(msg.size()) + "\r\n"s + msg + "\r\n"s
                };
                asio::mutable_buffers_1 msg_buffer = asio::buffer(reply);
                std::size_t byte_transferred = AsyncLib::write(socket_, msg_buffer, ec);
                assert(byte_transferred == reply.size());
                logger::info("Successfully send echo message \"{}\" to client", msg); 
            }

            bool ReadRawCommandFromSocket(std::string &raw_cmd) {
                asio::error_code ec;
                std::size_t bytes_read = AsyncLib::read(socket_, recv_, ec);

                if (ec){
                    logger::error("Failed to read command {}", ec.message());
                    return false;
                }

                size_t num_item  = 0;
                std::string num_items_str (
                    asio::buffers_begin(recv_.data()),
                    asio::buffers_begin(recv_.data()) + bytes_read
                );
                
                if (num_items_str.size() < 2){
                    logger::error("Invalid nunmber of item from command. Expected size to be >= 2. But get {}", num_items_str.size());
                    return false;
                }

                if (num_items_str.at(0) != '*'){
                    logger::error("Invalid command format. {}", num_items_str);
                    return false;
                }

                if (!ToSizeTFromStr(num_items_str.substr(1), &num_item)){
                   logger::error("No Number of items found from first token of message !");
                   return false;
                }
    
                for (int i = 0; i < num_item; i++){
                    bytes_read = AsyncLib::read(socket_, recv_, ec) - bytes_read;
                    if (ec){
                        logger::error("Failed to read the byte size {}th portion of message ", i + 1);
                        return false;
                    }
                    logger::info("Successfully read size portion of message {}", bytes_read); 
                    bytes_read = AsyncLib::read(socket_, recv_, ec) - bytes_read;
                    if (ec){
                        logger::error("Failed to read the content  {}th portion of message ", i + 1);
                        return false;
                    }
                }

                raw_cmd = std::string(                    
                    asio::buffers_begin(recv_.data()),
                    asio::buffers_begin(recv_.data()) + recv_.size()
                );
                return true;
            }

 

    };

    /*! \brief   Implement a asynchronous TCP server accepting client commands via
     *
    */
    template<typename TCPAcceptor, typename TCPSocket, typename AsyncLib>
    class Server {
        public:

            Server(uint16_t port, std::size_t num_thread_workers = 10, bool cont = true): port_(port),
                                                                     num_thread_workers_(num_thread_workers), 
                                                                     ioc_(),
                                                                     acceptor_(ioc_, tcp::endpoint(tcp::v4(), port_)),
                                                                     pool_(num_thread_workers),
                                                                     continuous_mode_(cont)
                                                                     {};

            /*! \brief Start server accepting client connection at initialized port port_ with threadpool workers
             *  
            */
            void Run(){
                logger::info("Start Server Listening at port {} with {} workers", port_, num_thread_workers_);
                this->StartAccepting();
                if (continuous_mode_){
                    logger::info("Start Keep Server Running !!!");
                    //KeepServerRunning();
                }
                running_ = true;
                ioc_.restart();
                size_t exec_handler_count = ioc_.run();
                logger::info("Server exited Run()  {} !", exec_handler_count);
            }

            void Stop() {
                continuous_mode_ = false;
                ioc_.stop();
            }

            bool IsRunning(){
                logger::info("Handler Executed {} Server Stopped {}", ioc_.poll(), ioc_.stopped());
                return !ioc_.stopped();
            }

        private:
            uint16_t port_;
            std::size_t num_thread_workers_;
            asio::io_context ioc_;
            TCPAcceptor acceptor_;
            asio::thread_pool pool_;
            // support testing
            bool running_ {false};
            bool continuous_mode_;

            void KeepServerRunning() {
                asio::post(
                    ioc_,
                    [this](){
                        logger::info("Keep Server Running task !");
                        sleep(5);
                        if(this->continuous_mode_){
                            this->KeepServerRunning();
                        }
                    }
                );
            }

            /*! \brief  Start a TCP server accepting client connectiosn at specified port number port_
             */
            void StartAccepting()
            {
                // initialize async server and wait for client commands
                logger::info("Accepting new connection !");
                auto conn = std::make_shared<Connection<TCPSocket, AsyncLib> >(this->pool_);
                acceptor_.async_accept(
                    conn->GetSocket(),
                    [conn, this](const asio::error_code &error)
                    {   
                        logger::info("Start Handle Socket !"); 
                        if (!error){
                            logger::info("Post Handle Client to Thread Pool !");
                            asio::post(
                                this->pool_,
                                [conn]()
                                {
                                    logger::info("Handle Client Connection Async");
                                    conn->HandleConnection();
                                    logger::info("Finished Handle Connection");
                                }
                            );
                        } else {
                            logger::error("Error Accepting Connection: {}", error.message());
                        }
                        // back to accepting
                        if (this->continuous_mode_){
                            this->StartAccepting();
                        }
                    });
                logger::info("Complete Async Accepting !");
            };
    };

}



#endif // __SERVER_H__