#include "resp_protocol/command_parser.h"
#include "resp_protocol/resp_consts.h"
#include "utils.h"

using namespace RedisServer;


static bool NextTokenStart(const std::string& str, size_t start, size_t* token_start) {
    size_t r_size_break_idx = str.find('\r', start);
    size_t n_size_break_idx = str.find('\n', start);

    if (! (start < str.size())){
        logger::error("CommandParser Error: index token out of range for start {} str size {}", start, str.size());
        return false;
    }

    if (r_size_break_idx == std::string::npos or n_size_break_idx == std::string::npos)
    {
        logger::error("NextTokenStart Error: Cannot find \r\n after Bulk Arr Size");
        return false;
    }

    if (n_size_break_idx != (r_size_break_idx + 1))
    {
        logger::error("NextTokenStart Error: \\r\\n is not in the right position after Bulk Arr Size");
        return false;
    }
    if (!token_start){
        throw std::runtime_error("token_start must be a valid pointer to store value");
    }
    *token_start = n_size_break_idx + 1;
    return true;
}


std::vector<std::string> RespProtocol::CommandParser::Parse(const std::string& raw_commands, ParserError& err){
     
    static const char simple_string_start = gRespDataTypePrefixMap.at(RespDataType::Arrays);
    static const size_t min_command_size {4};

    size_t bulk_arr_size = 0;

    std::vector<std::string> commands {};

    logger::info("Parsing commands: {}", raw_commands);
    
    if (raw_commands.size() < min_command_size){
        err = ParserError::kInvalidFormat;
        logger::error("CommandParser Error: {}. Command size is less than min required {} Raw Commands {} Size {}", 
                RespProtocol::gParseErrorStringMap.at(err), 
                min_command_size, 
                raw_commands, 
                raw_commands.size());
        return commands;
    }

    size_t array_start_idx = raw_commands.find(simple_string_start);
    size_t bulk_array_size_start_idx = array_start_idx + 1;
    size_t bulk_array_size_end_idx = 0;
    size_t start_command_size_idx = 0;
    size_t start_command_str_idx = 0;
    size_t command_len = 0;

    if (array_start_idx){
        err = ParserError::kMissingArrayStartChar;
        logger::error("CommandParser Error: {} Starting char was {}", 
                    RespProtocol::gParseErrorStringMap.at(err), raw_commands.at(0));
        return commands;
    }

    if (!NextTokenStart(raw_commands, bulk_array_size_start_idx, &bulk_array_size_end_idx)){
        err = ParserError::kMissingBulkArraySize;
        logger::error("CommandParser Error: {} Bulk Array Size cannot be found {}", 
                    RespProtocol::gParseErrorStringMap.at(err), raw_commands);
        return commands;        
    }

    start_command_size_idx = bulk_array_size_end_idx;

    if (!ToSizeTFromStr(raw_commands.substr(bulk_array_size_start_idx, bulk_array_size_end_idx - 2), 
                &bulk_arr_size)){
        err = ParserError::kMissingBulkArraySize;
        logger::error("CommandParserError: {} Bulk Array Size cannot be found from idx {} Commands {}",
              bulk_array_size_start_idx, raw_commands);
        return commands;
    }
    logger::info("Bulk Array Size is valid {}", bulk_arr_size);
    if (!bulk_arr_size){
        logger::warn("Bulk Array Size is 0. Return empty list !");
        return commands;
    }

    while(start_command_size_idx < raw_commands.size()) {

        if (raw_commands.at(start_command_size_idx) != gRespDataTypePrefixMap.at(RespDataType::BulkStrings)){
            err = ParserError::kUnexpectedDataTypeThatShouldBeBulkStrings;
            logger::error("CommandParseError: {} at {} found {} expected {}", 
                    gParseErrorStringMap.at(err), 
                    start_command_size_idx, 
                    raw_commands.at(start_command_str_idx),
                    gRespDataTypePrefixMap.at(RespDataType::BulkStrings));
            commands.clear();
            return commands;
        }

        start_command_size_idx++;
        if (!NextTokenStart(raw_commands, start_command_size_idx, &start_command_str_idx)){
            err = ParserError::kInvalidFormat;
            logger::error("CommandParseError: Missing \r\n while parsing command size at {} cmds {}", 
                start_command_size_idx, raw_commands);
            commands.clear();
            return commands;
        }

        if (!ToSizeTFromStr(raw_commands.substr(start_command_size_idx, start_command_str_idx - 2), &command_len)){
            err = ParserError::kInvalidCommandSize;
            logger::error("CommandParseError:  {} from index {} to index {}", 
                gParseErrorStringMap.at(err), 
                start_command_size_idx, 
                start_command_str_idx - 2
            );
            commands.clear();
            return commands; 
        }

        // check for ending of command string here 
        if (!NextTokenStart(raw_commands, start_command_str_idx, &start_command_size_idx)){
            err = ParserError::kInvalidFormat;
            logger::error("CommandParseError: Missing \r\n while parsing command size at {} cmds {}", 
                start_command_size_idx, raw_commands);
            commands.clear();
            return commands;
        }

        commands.push_back(raw_commands.substr(start_command_str_idx, start_command_size_idx - start_command_str_idx - 2));
        logger::info("CommandParser: new command added {}", commands.at(commands.size() -1));
        if (commands.at(commands.size() - 1).size() != command_len){
            err = ParserError::kCommandSizeMismatch;
            logger::error("CommandParseError: {}. new command size {} new command {} expected command len {} command {}",
                gParseErrorStringMap.at(err),
                commands.at(commands.size() - 1).size(), 
                commands.at(commands.size() - 1), 
                command_len, 
                raw_commands
            );
            commands.clear();
            return commands;
        }

        if (commands.size() == bulk_arr_size && start_command_size_idx < raw_commands.size()){
            err = ParserError::kExtraJunkAfterAllCommandsParsed;
            logger::error("CommandParseError: {}. All {} commands are added but still not at the end of raw_commands value",
                gParseErrorStringMap.at(err), bulk_arr_size
            );
            commands.clear();
            return commands;
        }

    }

    if (commands.size() != bulk_arr_size){
        err = ParserError::kNumberOfCommandsMismatch;
        logger::error("CommandParseError: {} expected {} but received {}",
            gParseErrorStringMap.at(err),
            bulk_arr_size,
            commands.size()
        );
        commands.clear();
        return commands;
    }

    return commands;
}