#ifndef __COMMAND_PARSER_H__
#define __COMMAND_PARSER_H__
#include <vector>
#include <string>

#include "resp_protocol/resp_consts.h"
#include "logging.h"

namespace RedisServer
{
    namespace RespProtocol
    {
        enum ParserError
        {
            kOk = 1,
            kMissingArrayStartChar,
            kInvalidFormat,
            kCommandSizeMismatch,
            kExtraJunkAfterAllCommandsParsed,
            kInvalidCommandSize,
            kMissingBulkArraySize,
            KUnsupportedCommands,
            kUnexpectedDataTypeThatShouldBeBulkStrings,
            kNumberOfCommandsMismatch
        };


        static const std::unordered_map<ParserError, std::string> gParseErrorStringMap = {
            {ParserError::kOk, "Ok"},
            {ParserError::kMissingArrayStartChar, "Missing * as start char of command"},
            {ParserError::kInvalidFormat, "Invalid Format"},
            {ParserError::kInvalidCommandSize, "Invalid Command Size. Must be a number"},
            {ParserError::KUnsupportedCommands, "Unuspported Commands"},
            {ParserError::kMissingBulkArraySize, "Missing Bulk Array Size"},
            {ParserError::kUnexpectedDataTypeThatShouldBeBulkStrings, "Unexpected DataType Symbol That Should be $ for bulk strings"},
            {ParserError::kCommandSizeMismatch, "Command Size Mismatch"},
            {ParserError::kExtraJunkAfterAllCommandsParsed, "Extra chars found after all commands are addded"},
            {ParserError::kNumberOfCommandsMismatch, "Number of Commands are "}

        };

        /*! \brief Command Parser implements parsing features for client commands send in as Array of Bulk Strings
         *  \param raw_commands          Command to be parsed sent from Redis client 
         *  \param error                 Upon parsing success this should return ParserError::kOK or else specific error
         * 
         * 
         * \returns vector of all commands that is parsed properly from raw_commadns input
         */
        class CommandParser
        {
        public:
            /*! \brief Parse input strings from Redis client that should be formatted as Aarray of Bulk Strings
             *
             *
            */
            static std::vector<std::string> Parse(const std::string& raw_commands, ParserError& err); 
            

};
    }

}
#endif // __COMMAND_PARSER_H__