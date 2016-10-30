//
#define CATCH_CONFIG_MAIN  
#include "tests/lib/catch.hpp"

//
#include "src/mason.h"

//
using namespace mason;

//
class club {};
class book {};

class author {
	public:
		int id;
		int age;
		std::string name;

		club club;
		std::vector<book> books;
		std::vector<book> get_books();

		author() {}

		void sync(data::model<author> &m) {
			//
			m.primary("id", id);

			m.field("age", age);
			m.field("name", name);

			m.has_one<class club>("clubs", club);
			m.has_many<class book>("books", books);
		}

	private:
		//
};

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
