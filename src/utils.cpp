
#include "utils.h"
#include "logging.h"

bool ToSizeTFromStr(const std::string& str, size_t* val){
 try{
        if (!val){
            throw std::runtime_error("val must be a valid pointer that store convert value");
        }        
        *val = std::stol(str);
    } catch(std::invalid_argument const& ex){
        logger::error("Failed to convert to size_t. raw str {}. Ex {}", str, ex.what());
        return false;
    } catch (std::out_of_range const& ex) {
        logger::error("Input out of range size_t. raw str {}. Ex {}", str, ex.what());
        return false; 
    }
    return true;
}

bool ToUint64tFromStr(const std::string& str, uint64_t* val){
 try{
        if (!val){
            throw std::runtime_error("val must be a valid pointer that store convert value");
        }        
        *val = std::stoull(str);
    } catch(std::invalid_argument const& ex){
        logger::error("Failed to convert to size_t. raw str {}. Ex {}", str, ex.what());
        return false;
    } catch (std::out_of_range const& ex) {
        logger::error("Input out of range size_t. raw str {}. Ex {}", str, ex.what());
        return false; 
    }
    return true;
}



bool ExecCmdWithOutput(std::string &cmd, std::string &output){
    const size_t BUFF_SIZE = 1024;
    char buff [BUFF_SIZE] = {0};
    output = "";
    logger::info("Executed cmd: {}", cmd);
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe){
        logger::error("Failed to execute command {}", cmd);
        return false;
    }
    while(fgets(buff, BUFF_SIZE, pipe)){
        output += buff;
    }
    logger::info("Command raw output {}", output);
    auto ret = pclose(pipe);
    if (ret){
        logger::error("Command might have failed to execute. output {} ret {}", output, ret);
    }
    return ret == 0;
}