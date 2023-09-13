#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace RedisServer {

    enum class CmdHandlerErr{
        Ok,
        InvalidFormat,
        InvalidCommandSize
    };

    enum class CommandType {
        SET,
        GET,
        PING,
        ECHO_CMD,
        LPUSH,
        RPUSH,
        EXIST,
        DELETE,
        UNKNOWN
    };


    static const std::unordered_map<CommandType, std::string> gToCommandStr {
        {CommandType::SET, "SET"},
        {CommandType::PING, "PING"},
        {CommandType::ECHO_CMD, "ECHO"},
        {CommandType::GET, "GET"},
        {CommandType::LPUSH, "LPUSH"},
        {CommandType::RPUSH, "RPUSH"},
        {CommandType::EXIST, "EXIST"},
        {CommandType::DELETE, "DELETE"},
        {CommandType::UNKNOWN, "UNKNOWN"},
    };

    class CommandHandler {
         public: 
            virtual void ProcessCmd(std::vector<std::string>& cmd, CmdHandlerErr& ec, std::string& out);
            
            CommandHandler(CommandType t): type_(t){}; 
            
            CommandType getType(void){return type_;}
            
            std::string getStrType(){
                if (gToCommandStr.find(type_) != gToCommandStr.end()){
                    return gToCommandStr.at(type_);
                }
                return gToCommandStr.at(CommandType::UNKNOWN);
            }
         private:
            CommandType type_;

    };

   
}