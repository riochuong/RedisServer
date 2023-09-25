#include "db_handler.h"

using namespace RedisServer;


    void DBHandler::ProcessCmd(std::vector<std::string>& cmd, CmdHandlerErr& ec, std::string& out){
        
        CommandType type = CommandType::UNKNOWN;
        if (gToCommandType.find(cmd[0]) != gToCommandType.end()){
            type = gToCommandType.at(cmd[0]);
        }
        
        switch(type){
            case CommandType::GET:
                break;
            case CommandType::SET:
                break;
            case CommandType::DELETE:
                break;
            case CommandType::EXIST:
                break;
            case CommandType::LPUSH:
                break;
            case CommandType::RPUSH:
                break;
            default:
                ec = CmdHandlerErr::UnsupportedCommand;
                break;   
        }
    }