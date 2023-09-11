#include <kv.h>
#include <memory.h>
#include <list>
#include "logging.h"

using namespace RedisServer;



void KeyValueDatabase::Get(const std::string& key, std::string &value, DatabseErrCode& ec) {
    auto value_it = this->db_.find(key);
    if (value_it == this->db_.end()){
        ec = DatabseErrCode::KeyNotFound;
        logger::error("Key Not Found {}", key);
        return;
    }
    if (value_it->second->type != ValueType::STRING){
        ec = DatabseErrCode::InvalidValueType;
        logger::error("Invalid value type for key {} Must be STRING but received {}", key, value_it->second->type);
        return;
    }
    ec = DatabseErrCode::Ok;
    value = *static_cast<std::string*>(value_it->second->val);
}


void KeyValueDatabase::Set(const std::string& key, const std::string& value, std::time_t expired_at,  DatabseErrCode& ec){

    auto value_it = this->db_.find(key);
    if (value_it != this->db_.end()){
        logger::info("Overwritten value for key {}", key);
        if (value_it->second->type == ValueType::LIST) {
            std::list<std::string>* list_ptr = static_cast<std::list<std::string>*>(value_it->second->val);
            delete(list_ptr); 
        } else {
            std::string* str_ptr = static_cast<std::string*>(value_it->second->val);
            delete(str_ptr); 
        }
        value_it->second->type = ValueType::STRING;
        value_it->second->expired_at = expired_at;
        value_it->second->val = (void*)(new std::string(value));
    } else{
        this->db_[key] = std::make_shared<Value>();
        this->db_[key]->expired_at = expired_at;
        this->db_[key]->val = (void*)(new std::string(value));
    }
}


uint32_t KeyValueDatabase::LPush(const std::string& key, const std::vector<std::string>& values, DatabseErrCode& ec){
    return 0;
}


uint32_t KeyValueDatabase::RPush(const std::string& key, const std::vector<std::string>& values, DatabseErrCode& ec) {
    return 0;
}


std::vector<std::string> KeyValueDatabase::RRange(const std::string& key, const std::vector<std::string>& values, DatabseErrCode& ec){
    return std::vector<std::string>();
}


bool KeyValueDatabase::Exists(const std::string& key, DatabseErrCode& ec){
    ec = DatabseErrCode::Ok;
    return db_.find(key) != db_.end();
}