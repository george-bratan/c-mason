#include "json.h"

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

namespace json {

	class Parser
	{
		public:
			//
			Parser() = default;
			~Parser() = default;

			//
			static Object parse(std::string source);

		protected:
			//
			static std::string _trim(const std::string& source, char c);
			static std::string _unescape(const std::string& source);
			static std::vector<std::string> _split(const std::string& source, char d);
	};

	std::string Parser::_trim(const std::string& source, char c = ' ')
	{
		size_t first = source.find_first_not_of(c);
		size_t last = source.find_last_not_of(c);
		return source.substr(first, last - first + 1);
	}

	std::string Parser::_unescape(const std::string& source) 
	{
		bool escaped = false;
		std::stringstream ss;

		//
		for (size_t i = 0; i < source.length(); ++i) {
			if (source[i] == '\\' && !escaped) {
				escaped = true;
			} else {
				ss << source[i];
				escaped = false;
			}
		} 
		
		//
		return ss.str();
	} 

	std::vector<std::string> Parser::_split(const std::string& source, char d) 
	{
		std::vector<std::string> pieces;
		std::string tmp = "";

		int a = 0, b = 0;
		bool escaped = false;

		//
		for (auto ch: source) {
			//
			if (ch == '"') 
				escaped = !escaped;

			if (escaped) {
				//
				tmp += ch;
				continue;
			}

			if (ch == '{') a++;
			if (ch == '}') a--;
			if (ch == '[') b++;
			if (ch == ']') b--;

			if (a || b) {
				//
				tmp += ch;
				continue;
			}

			//
			if (ch == d) {
				//
				pieces.push_back(tmp);
				tmp = "";

				continue;
			}

			tmp += ch;
		}

		//
		pieces.push_back(tmp);

		// done
		return pieces;
	}

	Object Parser::parse(std::string source) 
	{
		//std::cout << source << std::endl;
		source = _trim(source);
		json::Object result = {};

		switch (source[0]) {
			case '{': {
				// object
				result._type = Type::Object;

				// push children
				std::vector<std::string> pieces = _split(source.substr(1, source.size()-2), ',');
				for (std::string& piece: pieces) {
					//
					std::string n = piece.substr(0, piece.find(':'));
					std::string v = piece.substr(piece.find(':')+1);
					//std::vector<std::string> pair = split(piece, ':');
					result._value._object[ _trim(n) ] = parse(_trim(v));
				}
				break;
			}
			case '[': {
				// array
				result._type = Type::Array;

				// push children
				std::vector<std::string> pieces = _split(source.substr(1, source.size()-2), ',');
				for (std::string& piece: pieces) {
					//
					result._value._array.push_back( parse(_trim(piece)) );
				}
				break;
			}
			case '"': {
				result._type = Type::String;
				result._value._string = _unescape( source.substr(1, source.size()-2) );
				break;
			}
			default: {
				// values
				if (source == "null") {
					// null
					result._type = Type::Null;
				}
				else if (source == "true") {
					// null
					result._type = Type::Boolean;
					result._value._bool = true;
				}
				else if (source == "false") {
					// null
					result._type = Type::Boolean;
					result._value._bool = false;
				}
				else if (source.find('.') != std::string::npos) {
					//
					result._type = Type::Float;
					result._value._float = std::stod(source);
				}
				else {
					//
					result._type = Type::Integer;
					result._value._int = std::stoi(source);
				}
				break;
			}
		}

		//
		return result;
	}
	
}

#endif