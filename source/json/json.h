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

namespace json {

	//
	json::Object parse(const std::string& source)
	{
		return json::Parser::parse(source);
	}

}

#endif