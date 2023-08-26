#pragma once

#include "logging.h"
#include <asio.hpp>

class TestClient {
    public:
        TestClient(uint16_t port, std::string host): host_(host), port_(port), sock_ {ioc_} {}

        bool Connect(){
             if (closed_){
                logger::warn("Socket is already closed !!! Please create new client");
                return false;
             }
             if (connected_){
                logger::warn("Socket is already connected !!!!");
                return false;
             }
             asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string(host_), port_);
             sock_.connect(endpoint);
             connected_ = true;
             logger::info("Client connected to host successfully");
             return  true;
        }

        bool Send(const std::string msg){
            if(!connected_){
                logger::warn("Socket is not connected yet. Please call Connect !");
                return false;
            }
            if (closed_){
                logger::warn("Socket is already closed ! Please create new client and call connect");
                return false;
            }
            asio::error_code ec;
            asio::write(sock_, asio::buffer(msg), ec);
            if(ec){
                logger::error("Failed to write msg {} due to error {}", msg, ec.message());
                return false;
            }
            logger::info("Successfully send msg {}", msg);
            return true;
        }

        bool Recv(std::string &recv_msg){
            asio::streambuf sb;
            asio::error_code ec;
            logger::info("Client started waiting for receiving msg");
            std::size_t bytes_read = asio::read(sock_, sb, asio::transfer_all(), ec);
            if (ec && ec.message() != "End of file"){
                logger::error("Failed to read message due to error {} ", ec.message());
                return false;
            }

            recv_msg = {
                    asio::buffers_begin(sb.data()),
                    asio::buffers_begin(sb.data()) + bytes_read
            };
            logger::info("Successfully recv msg {}", recv_msg);
            return true;
        }

        void Close() {
            if (connected_ && !closed_){
                logger::info("Close socket ");
                sock_.close();
                closed_ = true;
            }
        }

    private:
        asio::io_context ioc_ {};
        asio::ip::tcp::socket sock_ {ioc_};
        std::string host_;
        uint16_t port_;
        bool connected_ = false;
        bool closed_ = false;

};