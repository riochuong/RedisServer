#pragma once

#include <unordered_map>
#include <string>
#include <ctime>
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
        std::time_t expired_at;
    } Value;

    enum DatabseErrCode {
        Ok,
        InvalidKeyType,
        InvalidValueType,
        KeyNotFound,
        MissingKeyWord
    };

    class KeyValueDatabase
    {
    public:
        KeyValueDatabase(void): db_{} {};

        /* Set key to hold the string value. If key already holds a value, it is overwritten, regardless of its type. 
           Any previous time to live associated with the key is discarded on successful SET operation.*/
        void Set(const std::string& key, const std::string& value, std::time_t expired_at, DatabseErrCode& ec);

        /* Get the value of key. If the key does not exist the special value nil is returned. 
          An error is returned if the value stored at key is not a string, because GET only handles string values.
        */
        void Get(const std::string& key, std::string &ret_value, DatabseErrCode& ec);

        /*Insert all the specified values at the head of the list stored at key. 
          If key does not exist, it is created as empty list before performing the push operations. 
          When key holds a value that is not a list, an error is returned.
          It is possible to push multiple elements using a single command call 
          just specifying multiple arguments at the end of the command. 
          Elements are inserted one after the other to the head of the list, from the leftmost element to the rightmost element. 
          So for instance the command LPUSH mylist a b c will result into a list containing c as first element, b as second element and a as third element.*/
        uint32_t LPush(const std::string& key, const std::vector<std::string>& values, DatabseErrCode& ec);

        /*
          Similar to LPush but start to insert elements from the end of the list.
        */        
        uint32_t RPush(const std::string& key, const std::vector<std::string>& values, DatabseErrCode& ec);
        
        /*
          Returns the specified elements of the list stored at key.
          The offsets start and stop are zero-based indexes, with 0 being the first element of the list (the head of the list), 
          1 being the next element and so on.
          These offsets can also be negative numbers indicating offsets starting at the end of the list. 
          For example, -1 is the last element of the list, -2 the penultimate, and so on.
        */
        std::vector<std::string> RRange(const std::string& key, const std::vector<std::string>& values, DatabseErrCode& ec);

        bool Exists(const std::string& key, DatabseErrCode& ec);

        uint64_t GetNumberOfKeys(void);

    private:
        std::unordered_map<std::string, std::shared_ptr<Value>> db_;
    };
}
