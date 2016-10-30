#include "includes.h"

//
TEST_CASE( "MODEL", "[model]" ) {
	//
	author a;

	a.age = 99;
	a.name = "Author A";

	data::model<author> b = a;
	REQUIRE( b->age == 99 );
	REQUIRE( b->name == "Author A" );

	b->age = 999;
	b->name = "Author B";
	// REQUIRE( a.age == 999 );
	// REQUIRE( a.name == "Author B" );
	REQUIRE( b->age == 999 );
	REQUIRE( b->name == "Author B" );

	author c = b;
	REQUIRE( c.age == 999 );
	REQUIRE( c.name == "Author B" );

	data::object d = b;
	REQUIRE( d.size() == 3 );
	REQUIRE( d["age"] == 999 );
	REQUIRE( d["name"] == "Author B" );
}
