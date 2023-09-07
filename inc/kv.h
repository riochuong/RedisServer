#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

namespace RedisServer{
    enum ValueType
    {
        STRING = 1,
        LIST = 2
    };

    typedef struct _val
    {
        ValueType type;
        void *val;
    } Value;

    enum DatabseErrCode {
        Ok,
        InvalidKeyType,
        MissingKeyWord
    };

    class KeyValueDatabase
    {
    public:
        KeyValueDatabase(void): db_{} {};

        void Set(const std::string& key, const std::string& value, uint64_t expiration, DatabseErrCode& ec);

        /* GET only handle String values */
        void Get(const std::string& key, std::string &ret_value, DatabseErrCode& ec);

        uint32_t Pushl(const std::string& key, const std::vector<std::string>& values, DatabseErrCode& ec);

        uint64_t GetNumberOfKeys(void);

    private:
        std::unordered_map<std::string, std::shared_ptr<Value>> db_;
    };
}
