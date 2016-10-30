#include "data.h"

#ifndef DATA_query_H
#define DATA_query_H

namespace mason {
namespace data {

	static sql::Connection * connection();

	//
	class prepare
	{
		public:
			prepare(const std::string& statement) 
			{
				_statement = data::connection()->prepareStatement( statement );
			}

			~prepare()
			{
				delete _statement;
			}

			prepare& bind(data::map& params)
			{
				for (auto& param: params) {
					_index++;

					if (param.second.is_int()) 
						_statement->setInt(param.first, param.second.as<int>());

					if (param.second.is_bool())
						_statement->setBoolean(param.first, param.second.as<bool>());

					if (param.second.is_float()) 
						_statement->setDouble(param.first, param.second.as<double>());

					if (param.second.is_string()) 
						_statement->setString(param.first, param.second.as<std::string>());
				}

				return *this;
			}

			prepare& bind(data::vector& params)
			{
				for (auto& param: params) {
					_index++;

					if (param.is_int()) 
						_statement->setInt(_index, param.as<int>());

					if (param.is_bool())
						_statement->setBoolean(_index, param.as<bool>());

					if (param.is_float()) 
						_statement->setDouble(_index, param.as<double>());

					if (param.is_string()) 
						_statement->setString(_index, param.as<std::string>());
				}

				return *this;
			}

			record first()
			{
				auto record = _statement->executeQuery();
				record->first();

				return record;
			}

			std::vector<record> all()
			{
				auto record = _statement->executeQuery();
				record->beforeFirst();
				
				std::vector<data::record> result;
				while(record->next()) {
					//
					result.push_back( record );
				}

				return result;
			}

			int execute()
			{
				return _statement->executeUpdate();
			}

		private:
			sql::PreparedStatement *_statement = nullptr;
			int _index = 0;
	};

	class query 
	{
		public:
			query(const std::string& table): _table(table)
			{
				//
			}

			int count() 
			{
				return query("(" + _statement() + ") AS T").with(_params).select("COUNT(*) AS count").first().get("count");
			}

			int insert_id() 
			{
				return query(_table).select("LAST_INSERT_ID() AS last").first().get("last");
			}

			int execute()
			{
				int rows = prepare().bind(_params).execute();

				return _type == type::INSERT ? insert_id() : rows;
			}

			record first() 
			{
				return prepare().bind(_params).first();
			}

			std::vector<record> all() 
			{
				return prepare().bind(_params).all();
			};

			prepare prepare() 
			{
				return _statement();
			};

			//
			query& select(std::string fields) 
			{
				std::swap(_fields, fields);
				return *this;
			}

			query& from(std::string table) 
			{
				std::swap(_table, table);
				return *this;
			}

			query& with(data::vector& params)
			{
				_params.insert(_params.end(), params.begin(), params.end());
				return *this;
			}

			query& set(std::string values, data::vector params = {}) 
			{
				std::swap(_values, values);
				return with(params);
			}

			query& where(std::string condition, data::vector params = {}) 
			{
				std::swap(_condition, condition);
				return with(params);
			}

			query& update(const std::string& values, data::vector params = {})
			{
				_type = type::UPDATE;
				return set(values, params);
			}

			query& insert(const std::string& values, data::vector params = {})
			{
				_type = type::INSERT;
				return set(values, params);
			}

			query& remove(const std::string& condition, data::vector params = {})
			{
				_type = type::DELETE;
				return where(condition, params);
			}

			template <typename T>
			auto remove(data::model<T>& m)
			{
				return remove(m.primary() + " = ?", {m.get(m.primary())}).execute();
			}

			template <typename T>
			auto remove(T& model)
			{
				data::model<T> m = model;
				return remove(m);
			}

			template <typename T>
			auto insert(data::model<T>& m)
			{
				std::string fields = "";
				data::vector params = {};

				//
				for (auto i: m.fields()) {
					//
					if (i == m.primary())
						continue;

					if (fields.size() > 0)
						fields += ", ";

					fields += i + " = ?";
					params.push_back( m.get(i) );
				}

				return insert(fields, params).execute();
			}

			template <typename T>
			auto insert(T& model)
			{
				data::model<T> m = model;
				return insert(m);
			}

			template <typename T>
			auto update(data::model<T>& m)
			{
				std::string fields = "";
				data::vector params = {};

				//
				for (auto i: m.fields()) {
					//
					if (i == m.primary())
						continue;

					if (fields.size() > 0)
						fields += ", ";

					fields += i + " = ?";
					params.push_back( m.get(i) );
				}

				return update(fields, params).where(m.primary() + " = ?", {m.get(m.primary())}).execute();
			}

			template <typename T>
			auto update(T& model)
			{
				data::model<T> m = model;
				return update(m);
			}


		private:
			std::string _statement()
			{
				switch (_type) {
					case type::SELECT:
						return "SELECT " + _fields + " FROM " + _table + " WHERE " + _condition;

					case type::UPDATE:
						return "UPDATE " + _table + " SET " + _values + " WHERE " + _condition;

					case type::INSERT:
						return "INSERT INTO " + _table + " SET " + _values;

					case type::DELETE:
						return "DELETE FROM " + _table + " WHERE " + _condition;
				}

				throw std::domain_error("Cannot build statement!");
			}

		private:
			enum class type
			{
				SELECT,
				INSERT,
				UPDATE,
				DELETE
			};

			query::type _type = type::SELECT;
			std::string _table;
			std::string _fields = "*";
			std::string _values = "*";
			std::string _condition = "(1)";
			data::vector _params;
	};

}
}

#endif