#include "rest.h"

#ifndef REST_RESPONSE_H
#define REST_RESPONSE_H

namespace mason {
namespace rest {

	class response
	{
		friend class request;

		public:
			response() = default;
			~response() = default;

			const std::string content() { return _content; }
			const data::object headers() { return _headers; }
			const int status() { return _status; }
			data::var data() { return json::parse(_content); }

			// implicit conversion
			operator bool() { return _status == 200; } 

		protected:
			int _status = 0;
			std::string _content;
			data::object _headers;
	};
}
}

#endif