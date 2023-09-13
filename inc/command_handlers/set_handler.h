#include <memory>
#include "asio.hpp"
#include "kv.h"
#include "command_handler.h"

using StrandExec = asio::strand<asio::thread_pool::executor_type>;



namespace RedisServer{

    class SetHandler: public CommandHandler {
        public:
            SetHandler(std::shared_ptr<KeyValueDatabase> kv_, 
                       std::shared_ptr<StrandExec> strand_) : CommandHandler(CommandType::SET), executor_(strand_){
            };
            void ProcessCmd(std::vector<std::string>& cmd, CmdHandlerErr& ec, std::string& out);
        private:
            std::shared_ptr<StrandExec> executor_;


    };

}