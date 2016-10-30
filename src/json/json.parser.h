#include "json.h"

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

namespace mason {
namespace json {

	class parser
	{
		public:
			// no validation performed
			// undefined behaviour on invalid json strings
			static data::var parse(std::string& source)
			{
				data::var result;

				//
				util::string::trim(source);
				//util::string::tolower(source);

				//
				switch (source[0]) {
					case '{': {
						// object
						result = data::object();

						// children
						auto pieces = _split(source.substr(1, source.size()-2), ',');

						// push
						for (auto piece: pieces) {
							//
							auto pair = _split(piece, ':');

							util::string::trim(pair[0], " \"");
							util::string::tolower(pair[0]);

							result.as<data::object>()[ pair[0] ] = parse(pair[1]);
						}
						break;
					}
					case '[': {
						// array
						result = data::vector();

						// children
						auto pieces = _split(source.substr(1, source.size()-2), ',');

						// push
						for (auto piece: pieces) {
							//
							result.as<data::vector>().push_back( parse(piece) );
						}
						break;
					}
					case '"': {
						result = util::string::unescape( source.substr(1, source.size()-2) );
						break;
					}
					default: {
						if (source == "null") {
							// 
							result = nullptr;
						}
						else if (source == "true") {
							// 
							result = true;
						}
						else if (source == "false") {
							// 
							result = false;
						}
						else if (source.find('.') != std::string::npos) {
							//
							result = std::stod(source);
						}
						else {
							//
							result = std::stoi(source);
						}
						break;
					}
				}

				//
				return result;
			}

			static std::string serialize(data::var& object)
			{
				std::string result = "";

				if (object.is_null()) {
					result = "null"; //"null";
				}

				if (object.is_bool()) {
					result = object.as<bool>() ? "true" : "false";
				}

				if (object.is_int()) {
					result = std::to_string( object.as<long>() ); 
				}

				if (object.is_float()) {
					result = std::to_string( object.as<double>() ); 
				}

				if (object.is_string()) {
					result = "\"" + util::string::escape( object.as<std::string>() ) + "\"";
				}

				if (object.is_vector()) {
					result += "[";

					for (auto& i: object.as<data::vector>()) {
						//
						if (result.size() > 1)
							result += ", ";

						result += serialize(i);
					}

					result += "]";
				}

				if (object.is_object()) {
					result += "{";

					for (auto& i: object.as<data::object>()) {
						//
						if (result.size() > 1)
							result += ", ";

						result += i.first + ":" + serialize(i.second);
					}

					result += "}";
				}

				return result;
			}

		protected:
			// splits string by character delimiter, skipping json structures
			static std::vector<std::string> _split(const std::string& source, char delimiter)
			{
				std::vector<std::string> pieces;
				std::string tmp = "";

				int in_array = 0;
				int in_object = 0;
				bool in_string = false;
				bool escaped = false;

				//
				for (auto c: source) {
					// backslash inside string
					if (in_string && c == '\\' && !escaped) {
						// next character will not affect string start/end
						tmp += c;
						escaped = true;
						continue;
					}

					// detect string
					if (c == '"' && !escaped) {
						// string start/end
						in_string = !in_string;
					}

					// detect arrays and objects
					if (!in_string) {
						// count nested structures
						if (c == '{') in_object++;
						if (c == '}') in_object--;
						if (c == '[') in_array++;
						if (c == ']') in_array--;
					}

					// skip structures
					if (in_string || in_array || in_object) {
						//
						tmp += c;
						escaped = false;
						continue;
					}

					// split chunk
					if (c == delimiter) {
						//
						pieces.push_back(tmp);
						tmp = "";

						continue;
					}

					tmp += c;
				}

				// final chunk
				if (!tmp.empty()) {
					// might be empty
					pieces.push_back(tmp);
				}

				// done
				return pieces;
			}
	};

	
	//std::vector<std::string> parser::_split(const std::string& source, char delimiter) 
	

	
	//data::var parser::parse(std::string& source) 
	

	//std::string parser::serialize(data::var& object)
	
	
}
}

#endif