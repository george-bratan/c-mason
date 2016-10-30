#include "util.h"

#ifndef UTIL_STRING_H
#define UTIL_STRING_H

namespace mason {
namespace util {
namespace string {

	//
	static const char* WHITESPACE = " \t\n\r\f\v";

	// 
	inline std::string& trim(std::string& source, const char* c = WHITESPACE)
	{
		if (source.empty())
			return source;

		source.erase(source.find_last_not_of(c) + 1);
		source.erase(0, source.find_first_not_of(c));

		return source;
	}

	//
	inline std::string& tolower(std::string &source) 
	{
	    std::transform(source.begin(), source.end(), source.begin(), [](unsigned char c) { return std::tolower(c); });
	    
	    return source;
	}

	//
	inline std::string& toupper(std::string &source) 
	{
	    std::transform(source.begin(), source.end(), source.begin(), [](unsigned char c) { return std::toupper(c); });
	    
	    return source;
	}

	//
	static std::vector<std::string> split(const std::string &source, const char* d = WHITESPACE)
	{
		std::vector<std::string> result;

		// skip delimiter
		size_t first = source.find_first_not_of(d);
		size_t next = first;

		while (first != std::string::npos){
			// 
			next = source.find_first_of(d, first);
			
			// 
			result.push_back(source.substr(first, next - first));
			
			// skip delimiter
			first = source.find_first_not_of(d, next);
		}

		return result;
	}

	static std::string url_encode(const std::string &source) 
	{
		std::ostringstream stream;
		stream.fill('0');
		stream << std::hex;

		for (auto c: source) {
			// Keep alphanumeric and other accepted characters intact
			if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
			    stream << c;
			    continue;
			}

			// Any other characters are percent-encoded
			stream << std::uppercase;
			stream << '%' << std::setw(2) << int(c);
			stream << std::nouppercase;
		}

		return stream.str();
	}

	static std::string url_params(std::map<std::string, std::string> params)
	{
		std::string result;

		for (auto i: params) {
			//
			result += i.first + "=" + url_encode(i.second) + "&";
		}

		return trim(result, "&");
	}

	static std::string escape(const std::string& source) 
	{
		std::string result;

		//
		for (auto c: source) {
			//
			switch (c) {
				//
				case '"': 
				case '\\': 
					result += '\\';
					result += c;
					break;
				
				case '\b':
					result += '\\';
					result += 'b';
					break;

				case '\f':
					result += '\\';
					result += 'f';
					break;

				case '\n':
					result += '\\';
					result += 'n';
					break;

				case '\r':
					result += '\\';
					result += 'r';
					break;

				case '\t':
					result += '\\';
					result += 't';
					break;

				default: 
					result += c;
			}
		}

		//
		return result;
	}

	static std::string unescape(const std::string& source) 
	{
		std::string result;
		bool escaping = false;

		for (auto c: source) {
			//
			if (escaping) {
				//
				switch (c) {
					case '"': 
					case '\\':
						result += c;
						break;

					case 'b':
						result += '\b';
						break;

					case 'f':
						result += '\f';
						break;

					case 'n':
						result += '\n';
						break;

					case 'r':
						result += '\r';
						break;

					case 't':
						result += '\t';
						break;

					default:
						result += c;
				}

				escaping = false;
				continue;
			}
			
			escaping = (c == '\\');

			if (escaping) {
				continue;
			}

			//
			result += c;
		} 
		
		return result;
	} 

}
}
}

#endif