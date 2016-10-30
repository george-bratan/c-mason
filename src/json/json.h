#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <iomanip>

#ifndef JSON_H
#define JSON_H

#include "json.object.h"
#include "json.parser.h"

namespace mason {
namespace json {

	//
	static data::var parse(std::string source)
	{
		return parser::parse(source);
	}

	static std::string serialize(data::var& object)
	{
		return parser::serialize(object);
	}

}
}

#endif