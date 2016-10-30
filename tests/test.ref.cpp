#include "includes.h"

//
TEST_CASE( "REF", "[data][ref]") {
	//
	SECTION( "int" ) {
		//
		int a = 10;
		data::var v = data::ref(a);
		REQUIRE( v == 10 );

		v = 20;
		REQUIRE( a == 20 );

		a = 30;
		REQUIRE( v == 30 );

		long b = 100;
		data::var w = data::ref(b);
		REQUIRE( w == 100 );

		w = 200;
		REQUIRE( b == 200 );

		b = 300;
		REQUIRE( w == 300 );

		//
		b = 100;
		v = b;
		REQUIRE( v == 100 );

		a = 10;
		w = a;
		REQUIRE( w == 10 );
	}

	SECTION( "float" ) {
		//
		float a = 10.1f;
		data::var v = data::ref(a);
		REQUIRE( v == 10.1f );

		v = 20.1f;
		REQUIRE( a == 20.1f );

		a = 30.1f;
		REQUIRE( v == 30.1f );

		double b = 100.1f;
		data::var w = data::ref(b);
		REQUIRE( w == 100.1f );

		w = 200.1f;
		REQUIRE( b == 200.1f );

		b = 300.1f;
		REQUIRE( w == 300.1f );

		//
		b = 100.1f;
		v = b;
		REQUIRE( v == 100.1f );

		a = 10.1f;
		w = a;
		REQUIRE( w == 10.1f );
	}

	SECTION( "string" ) {
		//
		std::string a = "10";
		data::var v = data::ref(a);
		REQUIRE( v == "10" );

		v = "20";
		REQUIRE( a == "20" );

		a = "30";
		REQUIRE( v == "30" );

		char * b = (char *) "100";
		data::var w = data::ref(b);
		REQUIRE( w == "100" );

		w = (char*) "200";
		REQUIRE( b == "200" );

		b = (char*) "300";
		REQUIRE( w == "300" );

		//
		b = (char*) "100";
		v = b;
		REQUIRE( a == "100" );
		REQUIRE( v == "100" );

		a = "10";
		w = a;
		std::string ww = w.as<std::string>();
		REQUIRE( ww == "10" );
	}
}
