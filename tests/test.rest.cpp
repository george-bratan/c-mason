#include "includes.h"

//
TEST_CASE( "REST", "[rest][model]" ) {
	//
	SECTION("get") {
		//
		auto response = rest::request("http://local.forex/oanda/rest.php").get({{"a", 1}, {"b", 2}});
		auto data = response.data();

		REQUIRE( response.status() == 200 );
		REQUIRE( data.is_object() );
		REQUIRE( data.size() == 2 );
		
		REQUIRE( data["response"]["age"] == 55 );
		REQUIRE( data["response"]["name"] == "Author 5" );
	}

	SECTION("post") {
		//
		auto response = rest::request("http://local.forex/oanda/rest.php").post({{"a", 1}, {"b", 2}});
		auto data = response.data();

		REQUIRE( response.status() == 200 );
		REQUIRE( data.is_object() );
		REQUIRE( data.size() == 2 );
		
		REQUIRE( data["response"]["age"] == 55 );
		REQUIRE( data["response"]["name"] == "Author 5" );
	}
}
