#include "json.h"

#ifndef JSON_OBJECT_H
#define JSON_OBJECT_H

namespace json {

	class Object;

	enum class Type 
	{
		Null = 0,
		Boolean,
		Integer,
		Float,
		String,
		Array,
		Object
	};

	class Value 
	{
		friend class Object;
		friend class Parser;

		bool _bool;
		long _int;
		double _float;
		std::string _string;
		std::vector<Object> _array;
		std::map<std::string, Object> _object;
	};

	class Object
	{
		friend class Parser;

		public:
			//
			Object() 
			{
				_type = Type::Null;
			};

			Object(bool i)
			{
				_type = Type::Boolean;
				_value._bool = i;
			};

			Object(int i)
			{
				_type = Type::Integer;
				_value._int = i;
			};

			Object(long i)
			{
				_type = Type::Integer;
				_value._int = i;
			};

			Object(double i)
			{
				_type = Type::Float;
				_value._float = i;
			};

			Object(std::string i)
			{
				_type = Type::String;
				_value._string = i;
			};

			Object(const Object &o)
			{
				_type = o._type;
				_value = o._value;
			};

			template <typename T>
			Object(std::vector<T> v) 
			{
				_type = Type::Array;
				for (auto i: v) {
					_value._array.push_back( new Object(*i) );
				}
			};

			template <typename T>
			Object(std::map<std::string, T> v) 
			{
				_type = Type::Object;
				for (auto i: v) {
					_value._object[ (*i).first ] = new Object( (*i).second );
				}
			};

			~Object() = default;

			//
			std::string serialize();

			//
    		Object & operator [](const int i) { return _value._array[i]; }
    		Object & operator [](const char* i) { return _value._object[i]; }

    		void operator =(const bool &i ) { _type = Type::Boolean; _value._bool = i; }
    		void operator =(const int &i ) { _type = Type::Integer; _value._int = i; }
    		void operator =(const long &i ) { _type = Type::Integer; _value._int = i; }
    		void operator =(const double &i ) { _type = Type::Float; _value._float = i; }
    		void operator =(const std::string &i ) { _type = Type::String; _value._string = i; }

    		// implicit conversion
    		operator bool() { _cast(Type::Boolean); return _value._bool; } 
    		operator int() { _cast(Type::Integer); return _value._int; } 
    		operator long() { _cast(Type::Integer); return _value._int; } 
    		operator float() { _cast(Type::Float); return _value._float; } 
    		operator std::string() { _cast(Type::String); return _value._string; } 
    		operator std::vector<Object>() { _cast(Type::Array); return _value._array; } 
    		operator std::map<std::string, Object>() { _cast(Type::Object); return _value._object; } 

    		//
		protected:
			json::Type _type;
			json::Value _value;

			void _cast(Type type) { if (_type != type) throw std::runtime_error("Bad type: " + std::to_string(static_cast<int>(_type))); }
			std::string _escape(const std::string& source);
	};

	//
	std::string Object::_escape(const std::string& source) 
	{
		std::stringstream ss;

		//
		for (size_t i = 0; i < source.length(); ++i) {
			if (unsigned(source[i]) < '\x20' || source[i] == '\\' || source[i] == '"') {
				ss << "\\u" << std::setfill('0') << std::setw(4) << std::hex << unsigned(source[i]);
			} else {
				ss << source[i];
			}
		} 

		//
		return ss.str();
	}

	//
	std::string Object::serialize()
	{
		std::string result = "";

		if (_type == Type::Null) {
			result = "null";
		}

		if (_type == Type::Boolean) {
			result = _value._bool ? "true" : "false";
		}

		if (_type == Type::Integer) {
			result = std::to_string(_value._int);
		}

		if (_type == Type::Float) {
			result = std::to_string(_value._float);
		}

		if (_type == Type::String) {
			result = "\"" + _escape(_value._string) + "\"";
		}

		if (_type == Type::Array) {
			result += "[";

			for (auto i = _value._array.begin(); i != _value._array.end(); ++i) {
				//
				result += (*i).serialize();

				if (i+1 != _value._array.end()) 
					result += ", ";
			}

			result += "]";
		}

		if (_type == Type::Object) {
			result += "{";

			for (auto i = _value._object.begin(); i != _value._object.end(); ++i) {
				//
				result += (*i).first + ':' + (*i).second.serialize();

				if (std::next(i) != _value._object.end()) 
					result += ", ";
			}

			result += "}";
		}

		return result;
	}
	
	std::ostream& operator << (std::ostream &os, Object &o) 
	{ 
		os << o.serialize(); 
		return os; 
	};
}

#endif