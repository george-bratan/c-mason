#include "rest.h"

#ifndef REST_REQUEST_H
#define REST_REQUEST_H

namespace mason {
namespace rest {

	class request
	{
		public:
			request(const std::string& source)
			{
				_source = source;

				//
				_curl = curl_easy_init();

				// set the URL we want
				curl_easy_setopt(_curl, CURLOPT_URL, _source.c_str());

				// callbacks
				curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, _write_content);
				curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, _write_headers);
			}

			~request()
			{
				curl_easy_cleanup(_curl);
			}

			response get(data::object query = {})
			{
				response response;

				this->_prepare(response);

				// update url
				//auto _query = std::map<std::string, std::string>(query.begin(), query.end());
				std::map<std::string, std::string> _query;
				for (auto& i: query) {
					//
					_query[ i.first ] = i.second.as<std::string>();
				}

				curl_easy_setopt(_curl, CURLOPT_URL, (_source + "?" + util::string::url_params(_query)).c_str());

				this->_execute();

				return response;
			}

			response post(data::object payload = {})
			{
				response response;

				this->_prepare(response);

				//auto _payload = std::map<std::string, std::string>(payload.begin(), payload.end());
				std::map<std::string, std::string> _payload;
				for (auto& i: payload) {
					//
					_payload[ i.first ] = i.second.as<std::string>();
				}

				//curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, util::string::url_params(payload).c_str());
				curl_easy_setopt(_curl, CURLOPT_COPYPOSTFIELDS, util::string::url_params(_payload).c_str());

				this->_execute();

				return response;
			}

		protected:
			CURL * _curl;
			std::string _source;
			std::string _headers;

			void _prepare(response &response)
			{
				curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &response);
				curl_easy_setopt(_curl, CURLOPT_WRITEHEADER, &response);
			}

			bool _execute()
			{
				return (curl_easy_perform(_curl) == CURLE_OK);
			}

			static size_t _write_content(char *buffer, size_t size, size_t nmemb, response *response)
			{
				size_t buffer_len = size * nmemb;
				size_t content_len = response->_content.size();

				try {
					// resize
					response->_content.resize(content_len + buffer_len);
				}
				catch(std::bad_alloc &e) {
					// memory
					return 0;
				}

				std::copy((char *)buffer, (char *)buffer + buffer_len, response->_content.begin() + content_len);

				//
				return size * nmemb;
			}

			static size_t _write_headers(char *buffer, size_t size, size_t nmemb, response *response)
			{
				std::string header = buffer;
				util::string::trim(header);

				if (header.find_first_of(':') != std::string::npos) {
					//
					std::vector<std::string> parts = util::string::split(header, ":");
					response->_headers[ parts[0] ] = util::string::trim(parts[1]);
				}
				else if (header.find_first_of("HTTP") != std::string::npos) {
					//
					std::vector<std::string> parts = util::string::split(header, " ");
					response->_status = std::stoi( parts[1] );
				}

				//
				return size * nmemb;
			}

	};

	// request::request(const std::string& source)
	

	// request::~request()
	

	// response request::get(data::object query)
	

	// response request::post(data::object payload)
	

	// void request::_prepare(response &response)
	


	// size_t request::_write_content(char *buffer, size_t size, size_t nmemb, response *response)
	

	// size_t request::_write_headers(char *buffer, size_t size, size_t nmemb, response *response)
	
}
}

#endif