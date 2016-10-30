#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <iomanip>

#ifndef UTIL_H
#define UTIL_H

#include "util.string.h"

namespace mason {
namespace util {

	//
	static void print() {
		std::cout << std::endl;
	}

	template<typename T, typename ... Args>
	static void print(T arg, const Args&... rest) {
		std::cout << arg << " ";
		print(rest...);
	}

	template <class T>
	static std::string name()
	{
		typedef typename std::remove_reference<T>::type TR;
		std::unique_ptr<char, void(*)(void*)> own(abi::__cxa_demangle(typeid(TR).name(), nullptr, nullptr, nullptr), std::free);
		std::string r = (own != nullptr) ? own.get() : typeid(TR).name();
		
		if (std::is_const<TR>::value)
			r += " const";
		if (std::is_volatile<TR>::value)
			r += " volatile";
		if (std::is_lvalue_reference<T>::value)
			r += "&";
		else if (std::is_rvalue_reference<T>::value)
			r += "&&";

		return r;
	}

}
}

#endif