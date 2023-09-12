
#include <catch2/catch_test_macros.hpp>
#include "kv.h"

using namespace RedisServer;

TEST_CASE("KV Test DB", "Set/Get Basic") {

    KeyValueDatabase kv_db {};

    SECTION("Verify Empty KV DB"){
        REQUIRE(kv_db.GetNumberOfKeys() == 0);
    }

    SECTION("Set and Get same key should return proper value"){
        DatabseErrCode ec = DatabseErrCode::Ok;
        std::string key {"1234abc"};
        std::string value {"abc@2"};
        kv_db.Set(key, value, 0,  ec);
        REQUIRE(ec == DatabseErrCode::Ok);
        std::string ret_val {};
        kv_db.Get(key, ret_val, ec);
        REQUIRE(ec == DatabseErrCode::Ok);
        REQUIRE(ret_val == value);
        REQUIRE(kv_db.Exists(key));
        // delete nonexistence key -- should return True
        REQUIRE(kv_db.DeleteKey("notexistencebefore"));
        REQUIRE(kv_db.DeleteKey(key));
        REQUIRE(!kv_db.Exists(key));
    } 
}

