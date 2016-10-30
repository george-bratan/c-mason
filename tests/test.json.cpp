#include "includes.h"

//
TEST_CASE( "JSON", "[json][var]" ) {
	//
	auto obj = json::parse("{a:null, b:true, c:1, d:1.2, e:\"string\", f:[1, 2], g:{a:3, b:\"nested\", c:[], d:{}}}");

	SECTION( "types" ) {
		//
		REQUIRE( obj["a"].is_null() );
		REQUIRE( obj["b"].is_bool() );
		REQUIRE( obj["c"].is_int() );
		REQUIRE( obj["d"].is_float() );
		REQUIRE( obj["e"].is_string() );
		REQUIRE( obj["f"].is_vector() );
		REQUIRE( obj["g"].is_object() );
	}

	SECTION( "values" ) {
		//
		REQUIRE( obj["a"] == nullptr );
		REQUIRE( obj["b"] == true );
		REQUIRE( obj["c"] == 1 );
		REQUIRE( obj["d"] == 1.2f );
		REQUIRE( obj["e"] == "string" );
		REQUIRE( obj["f"].size() == 2 );
		REQUIRE( obj["g"].size() == 4 );

		REQUIRE( obj["f"][0] == 1 );
		REQUIRE( obj["f"][1] == 2 );

		REQUIRE( obj["g"]["a"] == 3 );
		REQUIRE( obj["g"]["b"] == "nested" );
		REQUIRE( obj["g"]["c"].size() == 0 );
		REQUIRE( obj["g"]["d"].size() == 0 );
	}

	SECTION( "serialization" ) {
		//
		REQUIRE( json::serialize(obj) == "{a:null, b:true, c:1, d:1.200000, e:\"string\", f:[1, 2], g:{a:3, b:\"nested\", c:[], d:{}}}" );
	}

	SECTION( "assignments" ) {
		//
		obj["a"] = 10;
		REQUIRE( json::serialize(obj) == "{a:10, b:true, c:1, d:1.200000, e:\"string\", f:[1, 2], g:{a:3, b:\"nested\", c:[], d:{}}}" );

		obj["c"] = 20;
		REQUIRE( json::serialize(obj) == "{a:10, b:true, c:20, d:1.200000, e:\"string\", f:[1, 2], g:{a:3, b:\"nested\", c:[], d:{}}}" );

		obj["f"].push_back(3);
		REQUIRE( json::serialize(obj) == "{a:10, b:true, c:20, d:1.200000, e:\"string\", f:[1, 2, 3], g:{a:3, b:\"nested\", c:[], d:{}}}" );

		obj["g"]["e"] = false;
		REQUIRE( json::serialize(obj) == "{a:10, b:true, c:20, d:1.200000, e:\"string\", f:[1, 2, 3], g:{a:3, b:\"nested\", c:[], d:{}, e:false}}" );

		obj["g"].push_back({"f", nullptr});
		REQUIRE( json::serialize(obj) == "{a:10, b:true, c:20, d:1.200000, e:\"string\", f:[1, 2, 3], g:{a:3, b:\"nested\", c:[], d:{}, e:false, f:null}}" );
	}

	SECTION( "corner cases, control characters" ) {
		//
		obj = json::parse("{a:1, b:\"string {\", c:[]}");
		REQUIRE( obj.size() == 3 );
		REQUIRE( obj["b"] == "string {" );
		REQUIRE( json::serialize(obj) == "{a:1, b:\"string {\", c:[]}" );

		obj = json::parse("{a:1, b:\"string }\", c:[]}");
		REQUIRE( obj.size() == 3 );
		REQUIRE( obj["b"] == "string }" );
		REQUIRE( json::serialize(obj) == "{a:1, b:\"string }\", c:[]}" );

		obj = json::parse("{a:1, b:\"string {\\\"}\", c:[]}");
		REQUIRE( obj.size() == 3 );
		REQUIRE( obj["b"] == "string {\"}" );
		REQUIRE( json::serialize(obj) == "{a:1, b:\"string {\\\"}\", c:[]}" );

		obj = json::parse("{a:1, b:\"string {\\\":[]{{,1,}}}\", c:[]}");
		REQUIRE( obj.size() == 3 );
		REQUIRE( obj["b"] == "string {\":[]{{,1,}}}" );
		REQUIRE( json::serialize(obj) == "{a:1, b:\"string {\\\":[]{{,1,}}}\", c:[]}" );

		obj = json::parse("{a:1, b:\"string {\\\":[]{{,1\\n\\t\\r\\f:,}}}\", c:[]}");
		REQUIRE( obj.size() == 3 );
		REQUIRE( obj["b"] == "string {\":[]{{,1\n\t\r\f:,}}}" );
		REQUIRE( json::serialize(obj) == "{a:1, b:\"string {\\\":[]{{,1\\n\\t\\r\\f:,}}}\", c:[]}" );
	}
}
