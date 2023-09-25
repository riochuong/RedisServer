#include <memory>
#include "asio.hpp"
#include "kv.h"
#include "command_handler.h"

using StrandExec = asio::strand<asio::thread_pool::executor_type>;



namespace RedisServer{

    class DBHandler: public CommandHandler {
        public:
            DBHandler(std::shared_ptr<KeyValueDatabase> ptr_) : CommandHandler(){
                this->db_ptr_ = ptr_;
            };
            void ProcessCmd(std::vector<std::string>& cmd, CmdHandlerErr& ec, std::string& out);
        private:
            std::shared_ptr<KeyValueDatabase> db_ptr_;

    };

}