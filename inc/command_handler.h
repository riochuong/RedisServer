#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace RedisServer {

    enum class CmdHandlerErr{
        Ok,
        InvalidFormat
    };

    enum class CommandType {
        SET,
        GET,
        LPUSH,
        RPUSH,
        EXIST,
        DELETE,
        UNKNOWN
    };


    static const std::unordered_map<CommandType, std::string> gToCommandStr {
        {CommandType::SET, "SET"},
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