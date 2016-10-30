#include "includes.h"

//
TEST_CASE( "DATA", "[data][record][model]" ) {
	// connect
	data::connect("localhost", "root", "");
	data::schema("test");

	SECTION("record") {
		//
		auto first = data::query("authors").select("age, name").where("id = ?", {1}).first();
		REQUIRE( first.get("age") == 11 );
		REQUIRE( first.get("name") == "Author 1" );

		auto all = data::query("authors").all();
		REQUIRE( all.size() == 2 );
		REQUIRE( all[0].get("age") == 11 );
		REQUIRE( all[0].get("name") == "Author 1" );
	}

	SECTION("model") {
		//
		data::model<author> a = data::query("authors").select("name, age").where("id = ?", {1}).first();
		REQUIRE( a->age == 11 );
		REQUIRE( a->name == "Author 1" );

		author b = data::query("authors").select("age, name").where("id = ?", {2}).first();
		REQUIRE( b.age == 22 );
		REQUIRE( b.name == "Author 2" );
	}

	SECTION("query") {
		//
		int in = data::query("authors").insert("age = ?, name = ?", {11, "INSERT"}).execute();
		author a = data::query("authors").where("id = ?", {in}).first();
		REQUIRE( a.age == 11 );
		REQUIRE( a.name == "INSERT" );

		int up = data::query("authors").update("age = ?, name = ?", {22, "UPDATE"}).where("id = ?", {in}).execute();
		author b = data::query("authors").where("id = ?", {in}).first();
		REQUIRE( up == 1 );
		REQUIRE( b.age == 22 );
		REQUIRE( b.name == "UPDATE" );

		int rm = data::query("authors").remove("id = ?", {in}).execute();
		author c = data::query("authors").where("id = ?", {in}).first();
		REQUIRE( rm == 1 );
		// REQUIRE( c.empty() );

		author m;
		m.age = 111;
		m.name = "INSERT MODEL";
		m.id = data::query("authors").insert(m);

		author d = data::query("authors").where("id = ?", {m.id}).first();
		REQUIRE( d.age == 111 );
		REQUIRE( d.name == "INSERT MODEL" );

		m.age = 222;
		m.name = "UPDATE MODEL";
		int rows = data::query("authors").update(m);

		author e = data::query("authors").where("id = ?", {m.id}).first();
		REQUIRE( rows == 1 );
		REQUIRE( e.age == 222 );
		REQUIRE( e.name == "UPDATE MODEL" );
		
		int rm_rows = data::query("authors").remove(m);
		author f = data::query("authors").where("id = ?", {m.id}).first();
		REQUIRE( rm_rows == 1 );
		// REQUIRE( f.empty()  );
	}

	//
	data::disconnect();
}
