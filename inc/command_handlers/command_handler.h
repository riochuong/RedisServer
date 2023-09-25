#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace RedisServer {

    enum class CmdHandlerErr{
        Ok,
        InvalidFormat,
        InvalidCommandSize,
        InvalidTimeStampFormat,
        ExpiryTimeIsInThePast,
        UnsupportedCommand
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

    static const std::unordered_map<std::string, CommandType> gToCommandType {
        {"SET", CommandType::SET},
        {"PING", CommandType::PING},
        {"ECHO", CommandType::ECHO_CMD},
        {"GET", CommandType::GET},
        {"LPUSH", CommandType::LPUSH},
        {"RPUSH", CommandType::RPUSH},
        {"EXIST", CommandType::EXIST},
        {"DELETE", CommandType::DELETE},
        {"UNKNOWN", CommandType::UNKNOWN},
    };

    class CommandHandler {
        public: 
            virtual void ProcessCmd(std::vector<std::string>& cmd, CmdHandlerErr& ec, std::string& out);
    };

   
}