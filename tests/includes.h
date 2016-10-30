#include <stdlib.h>
#include <iostream>
#include <string>
#include <typeinfo>
#include <map>
#include <unordered_map>
#include <type_traits>
#include <cxxabi.h>
#include <memory>
#include <cstdlib>
#include <cstring>

//
#ifndef INCLUDES_H
#define INCLUDES_H

//
#include "lib/catch.hpp"

//
#include "../src/util/util.h"
#include "../src/data/data.h"
#include "../src/json/json.h"
#include "../src/rest/rest.h"

//
using namespace mason;

//
class club {};
class book {};

//
class author {
	public:
		int id;
		int age;
		std::string name;

		club club;
		std::vector<book> books;
		std::vector<book> get_books();

		author() {}

		void sync(data::model<author> &m) {
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

#endif