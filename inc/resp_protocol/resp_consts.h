#ifndef __RESP_H__
#define __RESP_H__

#include <unordered_map>

namespace RedisServer {

    namespace RespProtocol{

        enum RespDataType {
            SimpleStrings = 1,
            Errors,
            Integers,
            BulkStrings,
            Arrays
        };

        static const std::unordered_map<RespDataType, char> gRespDataTypePrefixMap = {
            {SimpleStrings, '+'},
            {Errors, '-'},
            {Integers, ':'},
            {BulkStrings, '$'},
            {Arrays, '*'},
        };

    }

}

#endif // __RESP_H__