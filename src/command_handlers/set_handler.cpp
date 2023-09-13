#include <chrono>
#include <ctime>
#include "logging.h"
#include "command_handlers/set_handler.h"

using namespace RedisServer;

void SetHandler::ProcessCmd(std::vector<std::string>& cmd, CmdHandlerErr& ec, std::string& out){

   if (cmd.empty()){
      logger::error("cmd list is empty for Set Handler");
      ec = CmdHandlerErr::InvalidFormat;
      return;
   } 
   
   if (cmd[0] != "GET"){
      logger::error("Command starting word is not correct !");
      ec = CmdHandlerErr::InvalidFormat;
      return;
   }
    if (cmd.size() < 3){
       logger::error("Invalid Command Size. Need to be at least 3 but received {}", cmd.size());
       ec = CmdHandlerErr::InvalidCommandSize;
       return;

    }
    std::string_view key {cmd[1]};
    std::string_view value {cmd[2]};

    // check for timestamp 
    if (cmd.size() >= 5){
        std::string_view expiry_key = cmd[3];
        std::string_view expiry_val = cmd[4];
        if (expiry_key == "EX"){

        }
    }
   

}
