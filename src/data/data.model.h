#include "data.h"

#ifndef DATA_MODEL_H
#define DATA_MODEL_H

namespace mason {
namespace data {

	//template<typename F, typename... Ts>

	//
	template <typename M>
	class model
	{
		public:
			model(): _model(new M()), _is_ref(false) {
				//
				_model->sync( *this );
			}

			model(data::object values): model() {
				//
				//for (auto i = v.as<data::object>().begin(); i != v.as<data::object>().end(); ++i) {
				for (auto& i: values) {
					//
					if (_fields.count(i.first) > 0) {
						_fields[ i.first ] = i.second;
					}
				}
			}

			// model(M* m): _model(&m), _is_ref(true) {
			// 	//
			// 	_model->sync( *this );
			// }

			model(M m): model() {
				//
				*_model = m;
			}

			~model() {
				if (!_is_ref)
					delete _model;
			}

			M* operator->() const { 
				return _model; 
			}

			operator M() {
				//
				return *_model;
			}

			operator object() {
				//
				return _fields;
				//return std::map<std::string, data::var>(_fields.begin(), _fields.end());

				// var result = object();

				// for (auto& i: _fields) {
				// 	//
				// 	result[i.first] = _fields[i.first];
				// }

				// return result;
			}

			var& get(const std::string& key) {
				//
				return _fields.at(key);
			}

			auto primary() { 
				//
				return _primary; 
			}

			auto fields() {
				//
				std::vector<std::string> result;

				for(auto i: _fields) {
					//
					result.push_back(i.first);
				}

				return result;
			}

			template <typename T>
			void field(const std::string& name, T& value) {
				//
				_fields[name] = data::ref(value); 
			}

			template <typename T>
			void primary(const std::string &name, T& value) {
				//
				field(name, value);
				_primary = name;
			}

			template <typename F, typename T>
			void has_one(const std::string &name, T& value) {
				//
			}

			template <typename F, typename T>
			void has_many(const std::string &name, T& value) {
				//
			}

		private:
			M* _model;
			bool _is_ref;
			data::object _fields;
			std::string _primary;
	};

}
}

#endif