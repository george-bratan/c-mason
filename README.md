# mason
C++11 web library built for demonstration purposes.

Details
--
Provides tools to consume JSON, MySQL and REST content.

Example
--
~~~~
	// parsing
	json::Object obj = json::parse("{a:1, b:\"string\", c:[1,2], d:{a:2, b:\"nested string\"}}");
	std::cout << obj["b"] << std::endl;
	
	// conversion
	std::string i = obj["b"];
	std::cout << i << std::endl;

	// assignment
	obj["a"] = 9;
	int x = obj["a"];
	std::cout << x << std::endl;
~~~~

Version 0.1:
- Naive JSON parser

