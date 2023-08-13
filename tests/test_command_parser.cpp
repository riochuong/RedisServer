#include <catch2/catch_test_macros.hpp>
#include <cstdlib>

#include "resp_protocol/command_parser.h"


using namespace std::string_literals;
using namespace RedisServer::RespProtocol;




static std::string GenerateTestString(size_t num_commands, 
                                      size_t max_command_size, 
                                      std::vector<std::string>& expected_subcmds) {
    std::string cmd {"*" + std::to_string(num_commands)+"\r\n"};
    for (int i = 0; i < num_commands; i++){
        size_t cmd_size = std::rand() % max_command_size;
        logger::debug("Generate command of size {}", cmd_size);
        std::string sub_cmd {"$" + std::to_string(cmd_size) + "\r\n"};
        size_t start_cmd_str = sub_cmd.size();
        for (int j = 0; j < cmd_size; j++){
            char new_char = 'a' + rand() % 26; 
            sub_cmd.push_back(new_char);
        }
        expected_subcmds.push_back(sub_cmd.substr(start_cmd_str));
        cmd += sub_cmd;
        cmd += "\r\n";
    }
    logger::info("Total number of subcommand genreated {}. Commands \r\n {}", num_commands, cmd);
    return cmd;
} 

TEST_CASE("Test Command Parser Basic", "Test Resp Protocol"){

    SECTION("Basic Sanity Test")
    {

        ParserError ec = ParserError::kOk;
        auto commands = CommandParser::Parse(
            "*2\r\n$4\r\nLLEN\r\n$6\r\nmylist\r\n"s,
            ec);

        REQUIRE(ec == ParserError::kOk);
        REQUIRE(commands.size() == 2);
        REQUIRE(commands[0].size() == 4);
        REQUIRE(commands[0] == "LLEN");
        REQUIRE(commands[1] == "mylist");
        REQUIRE(commands[1].size() == 6);
    }

    SECTION("Empty Command Test")
    {

        ParserError ec = ParserError::kOk;
        auto commands = CommandParser::Parse(
            "*0\r\n"s,
            ec);

        REQUIRE(commands.empty());
        REQUIRE(ec == ParserError::kOk);
    }

    SECTION("Number of Commands Mismatch")
    {

        ParserError ec = ParserError::kOk;
        auto commands = CommandParser::Parse(
            "*3\r\n$4\r\nLLEN\r\n$6\r\nmylist\r\n"s,
            ec);

        REQUIRE(ec == ParserError::kNumberOfCommandsMismatch);
        REQUIRE(commands.empty());
    }

    SECTION("Junks after command")
    {

        ParserError ec = ParserError::kOk;
        auto commands = CommandParser::Parse(
            "*4\r\n$4\r\nLLEN\r\n$6\r\nmylist\r\n$2\r\naa\r\n$5\r\n1234b\r\n sss"s,
            ec);

        REQUIRE(ec == ParserError::kExtraJunkAfterAllCommandsParsed);
        REQUIRE(commands.empty());
    }


    SECTION("Command Size Mismatch")
    {

        ParserError ec = ParserError::kOk;
        auto commands = CommandParser::Parse(
            "*4\r\n$4\r\nLLEN\r\n$6\r\nmylistaaa\r\n$2\r\naa\r\n$5\r\n1234b\r\n"s,
            ec);

        REQUIRE(ec == ParserError::kCommandSizeMismatch);
        REQUIRE(commands.empty());
    }

    
    SECTION("Invalid command size that is not a number")
    {

        ParserError ec = ParserError::kOk;
        auto commands = CommandParser::Parse(
            "*4\r\n$4\r\nLLEN\r\n$6\r\nmylist\r\n$lslslslsls\r\naa\r\n$5\r\n1234b\r\n"s,
            ec);

        REQUIRE(ec == ParserError::kInvalidCommandSize);
        REQUIRE(commands.empty());
    }

    SECTION("Empty Command")
    {

        ParserError ec = ParserError::kOk;
        auto commands = CommandParser::Parse(
            "*4\r\n$4\r\nLLEN\r\n$6\r\nmylist\r\n$0\r\n\r\n$5\r\n1234b\r\n"s,
            ec);

        REQUIRE(ec == ParserError::kOk);
        REQUIRE(commands.size() == 4);
        REQUIRE(commands[2].empty());
    }
}

TEST_CASE("Stress Test Command Parser", "Test Resp Protocol"){
       
    uint32_t seed = time(NULL);
    logger::info("Stress Test Command Parser Seed: {}", seed);
    uint32_t num_cycles = 20;
    uint32_t num_commands = 100;
    uint32_t max_cmd_size = 30;
    ParserError ec = ParserError::kOk;
    std::vector<std::string> expeced_sub_cmds {};

    for (int i = 1; i <= num_cycles; i++){
        ec = ParserError::kOk;
        expeced_sub_cmds.clear();
        logger::info("Stress Test Command Parser Cycle {}", i);
        std::string command = GenerateTestString(num_commands, 
                                                 max_cmd_size,
                                                 expeced_sub_cmds
                                                 );
        REQUIRE(num_commands == expeced_sub_cmds.size());
        auto commands = CommandParser::Parse(command, ec);
        REQUIRE(commands.size() == num_commands);
        REQUIRE(commands.size() == expeced_sub_cmds.size());
        for (int i = 0; i < commands.size(); i++){
            REQUIRE(commands[i] == expeced_sub_cmds[i]);
        }
    }
}




