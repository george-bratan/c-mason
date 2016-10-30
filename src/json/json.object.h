#include "json.h"

#ifndef JSON_OBJECT_H
#define JSON_OBJECT_H

namespace mason {
namespace json {

	class object
	{
		public:
			object(): _var() {}
			object(data::var var): _var(var) {}

			operator data::var() { return _var; }

		private:
			data::var _var;
	};
}
}

#endif