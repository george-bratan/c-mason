#include "includes.h"

//
TEST_CASE( "VAR", "[data][var]") {
	//
	data::var v;

	SECTION( "null" ) {
		//
		v = nullptr;
		REQUIRE( v.is_null() );
		REQUIRE( v == nullptr );
	}

	SECTION( "int" ) {
		//
		v = 1;
		REQUIRE( v.is_int() );
		REQUIRE( v == 1 );
	}

	SECTION( "float" ) {
		//
		v = 1.2;
		REQUIRE( v.is_float() );
		REQUIRE( v == 1.2f );
	}

	SECTION( "bool" ) {
		//
		v = true;
		REQUIRE( v.is_bool() );
		REQUIRE( v == true );
	}

	SECTION( "string" ) {
		//
		v = "string";
		REQUIRE( v.is_string() );
		REQUIRE( v == "string" );
	}
	
	SECTION( "array" ) {
		//
		v = data::var({1, 2});
		REQUIRE( v.is_vector() );
		REQUIRE( v.size() == 2 );
		REQUIRE( v[0] == 1 );
		REQUIRE( v[1] == 2 );

		v.push_back(3);
		REQUIRE( v[2] == 3 );
		REQUIRE( v.size() == 3 );
	}
	
	SECTION( "object" ) {
		//
		v = data::var({{"a", 1}, {"b", 2}});
		REQUIRE( v.is_object() );
		REQUIRE( v.size() == 2 );
		REQUIRE( v["a"] == 1 );
		REQUIRE( v["b"] == 2 );

		v.push_back({"c", 3});
		REQUIRE( v["c"] == 3 );
		REQUIRE( v.size() == 3 );
	}
}
