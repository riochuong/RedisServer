#pragma once
#include <vector>

#include "logging.h"
#include <asio.hpp>

class MockSocket {
    public:
        MockSocket(asio::thread_pool &exec): exec_(exec.get_executor()) {
            // ignore buff_size for MockSocket
        }

        asio::thread_pool::executor_type get_executor(){
            return exec_;
        }

        void set_option(asio::socket_base::receive_buffer_size option){
            return;
        }

        void open(asio::ip::tcp sock){
            return;
        }

    private:
        asio::thread_pool::executor_type exec_;
};

class MockAcceptor {
    public:
        static asio::error_code errorAcceptor;
        MockAcceptor(asio::io_context& ioc, asio::ip::tcp::endpoint ep): ep_(ep), exec_(ioc.get_executor()) {}

        template <typename AcceptHandler>
        void async_accept(MockSocket& socket, AcceptHandler&& handler){
            asio::async_initiate<AcceptHandler, void(const asio::error_code&)>(
                [](AcceptHandler &&handler, auto& exec){
                       logger::info("Mock Acceptor Handle Client !"); 
                       asio::post(
                            exec,
                            asio::detail::bind_handler(
                                std::move(handler),
                                errorAcceptor
                            )
                       );
                       logger::info("Handle Client complete from async_initiate ");
                },
                handler,
                exec_
            );

        }

    private:
        asio::ip::tcp::endpoint ep_;
        asio::io_context::executor_type exec_;
};

class MockAsyncReadWrite
{
    public:

        static std::string messageToRead;
        static std::string messageToWrite;
        
        static asio::error_code errorCodeForRead;
        static asio::error_code errorCodeForWrite;

        static std::vector<std::string> msgVector; 
        static size_t currMsgIdx;

        static void setMessageToRead(std::string msg) {
            msgVector.clear();
            messageToRead = msg;
            logger::info("Message To Read: {}", messageToRead);
            // break message to read into chunks to simulate socket read
            std::string delim = "\r\n";
            size_t pos = msg.find(delim);
            while(pos != std::string::npos){
                msgVector.push_back(msg.substr(0, pos + 2));
                msg.erase(0, pos + 2);
                pos = msg.find(delim);
            }           
            if (pos == std::string::npos && !msg.empty()){
                msgVector.push_back(msg);
            } 
            currMsgIdx = 0;
        }

        template <
            typename AsyncReadStream,
            typename DynamicBuffer_v2>
        static std::size_t read(
            AsyncReadStream &s,
            DynamicBuffer_v2 &b,
            asio::error_code &ec
        ) 
        {
            ec = errorCodeForRead;
            if (currMsgIdx >= msgVector.size()){
                throw std::runtime_error("Message read complete ! Need to call setMessageRead with new message to reset");
            }
            size_t numbyte_copied = asio::buffer_copy(
                b.prepare(msgVector[currMsgIdx].size()),
                asio::buffer(msgVector[currMsgIdx])
            );
            b.commit(numbyte_copied);
            currMsgIdx++;
            logger::info("Number of bytes copied read {} ", numbyte_copied);
            return numbyte_copied; 
        }

        template <
            typename AsyncWriteStream,
            typename DynamicBuffer_v2>
        static std::size_t write(
            AsyncWriteStream &s,
            const DynamicBuffer_v2 &buffers,
            asio::error_code &ec)
        {
            ec = errorCodeForWrite;
            messageToWrite = {asio::buffers_begin(buffers), asio::buffers_end(buffers)};
            logger::info("Received message to write {}", messageToWrite);
            return messageToWrite.size();
        }
};


inline std::string MockAsyncReadWrite::messageToRead = {};
inline asio::error_code MockAsyncReadWrite::errorCodeForRead = {};
inline asio::error_code MockAsyncReadWrite::errorCodeForWrite = {};
inline asio::error_code MockAcceptor::errorAcceptor = {};
inline std::string MockAsyncReadWrite::messageToWrite = {};
inline std::vector<std::string> MockAsyncReadWrite::msgVector = {};
inline size_t MockAsyncReadWrite::currMsgIdx = 0;