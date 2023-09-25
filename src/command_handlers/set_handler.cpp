#include <chrono>
#include <ctime>
#include "logging.h"
#include "utils.h"
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
    if (cmd.size() >= 5)
    {
       std::string &expiry_key = cmd[3];
       std::string &expiry_val = cmd[4];
       auto expired_time = std::chrono::system_clock::now();
       uint64_t timestamp = 0;
       if (!ToUint64tFromStr(expiry_val, &timestamp)){
           logger::error("Invalid Time Format for EX/PX");
           ec = CmdHandlerErr::InvalidTimeStampFormat;
           return;
       }

       if (expiry_key == "EX"){
            expired_time += std::chrono::seconds(timestamp);
       }
       else if (expiry_key == "PX"){
            expired_time += std::chrono::milliseconds(timestamp);
       }
       else if (expiry_key == "EXAT"){
            expired_time = std::chrono::system_clock::time_point(std::chrono::seconds(timestamp));            
       } else if (expiry_key == "PXAT"){
            expired_time = std::chrono::system_clock::time_point(std::chrono::milliseconds(timestamp));
       }

       if (expired_time < std::chrono::system_clock::now()){
           logger::error("Expiry Time is in the past");
           ec = CmdHandlerErr::ExpiryTimeIsInThePast;
           return;
       }
    }
    
}
