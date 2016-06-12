/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <string>

//
#include "json/json.h"

//
int main(int argc, char const *argv[])
{
	// parse
	json::Object obj = json::parse("{a:1, b:\"root, {some:characters\", c:[1,2], d:{a:1, b:\"nested, [\\:{),ff,a:abc:abcd/\"}}");
	std::cout << obj["b"] << std::endl;
	
	// assign
	std::string i = obj["b"];
	std::cout << i << std::endl;

	// assign
	obj["a"] = 9;
	int x = obj["a"];
	std::cout << x << std::endl;	

	// done
	return 0;
}