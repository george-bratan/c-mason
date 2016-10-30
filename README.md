# C++ Mason
C++11 helper library.

Details
--
Provides tools to consume JSON, REST and database content. See tests for further examples.

Variant 
--
Polymorphic container class. Accepts any type or class and provides conversion methods for fundamental types.

~~~~
// variant
var a = nullptr;
var b = true;
var c = 10;
var d = 10.2;
var e = "string";
var f = {1, 2, 3};
var g = {{"a", 1}, {"b", true}, {"c", "str"}};

// broad type test
if (v.is_int()) ... // true for all integral types
if (v.is_string()) ... // true or all string constructible types

// exact type test
if (v.is<int>()) ... // true for int only
if (v.is<std::string>()) .. // true for std::string only

// explicit conversion
var v = 10;
auto x = v.as<int>(); // 10
auto y = v.as<long>(); // 10

// reference variant
int i = 1;
var a = ref(i); // holds reference
std::cout << a << std::endl; // a = 1
std::cout << i << std::endl; // i = 1

a = 2;
std::cout << a << std::endl; // a = 2
std::cout << i << std::endl; // i = 2
~~~~

Model
--
Adds introspection for objects implementing a `sync()` method.

Example:
~~~
class author {
	public:
		int id;
		int age;
		std::string name;

		club club;
		std::vector<book> books;

		author() {}

		void sync(auto &m) {
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

author a = {1, 11, "Author 1"};
model<author> b = a;
std::cout << b->id << std::endl; // 1
std::cout << b->age << std::endl; // 11
std::cout << b->name << std::endl; // Author 1

auto fields = b.fields(); // {"id", "age", "name"}
std::string name = b.get("name") // Author 1

author c = b;
std::cout << c.id << std::endl; // 1
std::cout << c.age << std::endl; // 11
std::cout << c.name << std::endl; // Author 1
~~~

JSON
--
Static methods for consuming and producing JSON content. No validation.

Example:
~~~
auto obj = json::parse("{a:null, b:true, c:1, d:1.2, e:\"string\", f:[1, 2], g:{a:3, b:\"nested\", c:[], d:{}}}");

std::cout << obj["a"] << std::endl; // null
std::cout << obj["b"] << std::endl; // true
std::cout << obj["c"] << std::endl; // 1
std::cout << obj["d"] << std::endl; // 1.2
std::cout << obj["e"] << std::endl; // string
std::cout << obj["f"][0] << std::endl; // 1
std::cout << obj["g"]["a"] << std::endl; // 3
~~~

REST
--
Wrapper over CURL. Provides shorthand chained methods to consume web services. Incomplete.

Example:
~~~
auto response = rest::request("http://example/api/authors").get( {{"id", 1}} );
auto data = response.data();

std::cout << response.status() << std::endl; // 200
std::cout << data["age"] << std::endl; // 11
std::cout << data["name"] << std::endl; // Author 1

data["age"] = 22;
rest::request("http://example/api/authors").post( data );

// implicit conversion to model specialisation if exists
model<author> a = rest::request("http://example/api/authors").get( {{"id", 1}} ).data(); 
std::cout << a->age << std::endl; // 11
std::cout << a->name << std::endl; // Author 1

// implicit conversion to c++ class if a model specialisation exists
author a = rest::request("http://example/api/authors").get( {{"id", 1}} ).data();
std::cout << a.age << std::endl; // 11
std::cout << a.name << std::endl; // Author 1

~~~

SQL Query Builder
--
Provides shorthand chained methods to consume database objects. Uses prepared statements and parameter binding. 
Wrapper over MySQL. Incomplete.

Example:
~~~
data::connect("localhost", "root", "");
data::schema("test");

// variant
auto first = data::query("authors").select("age, name").where("id = ?", {1}).first();
std::cout << first["age"] << std::endl; // 11
std::cout << first["name"] << std::endl; // Author 1

// std::vector<variant>
auto all = data::query("authors").select("age, name").all();
std::cout << all[0]["age"] << std::endl; // 11 
std::cout << all[1]["age"] << std::endl; // 22

// implicit conversion to model specialisation if exists
model<author> a = data::query("authors").first(); 
std::cout << a->age << std::endl; // 11
std::cout << a->name << std::endl; // Author 1

// implicit conversion to c++ class if a model specialisation exists
author a = data::query("authors").first();
std::cout << a.age << std::endl; // 11
std::cout << a.name << std::endl; // Author 1

data::disconnect();
~~~

Revisions
--
Version 0.2:
- stable variant class
- basic JSON parser
- basic ORM, incomplete
- basic REST client, incomplete

Version 0.1:
- Naive JSON parser

