#include "data.h"

#ifndef DATA_RECORD_H
#define DATA_RECORD_H

namespace mason {
namespace data {

	class record
	{
		public:
			record(sql::ResultSet *result)
			{
				if (result->isBeforeFirst() || result->isAfterLast())
					return;

				// get meta data
				auto meta = result->getMetaData();

				//getColumnTypeName

				// pull values
				for (auto i = 1; i < meta->getColumnCount() + 1; i++) {
					//
					if (result->isNull(i)) {
						//
						_values[ meta->getColumnLabel(i) ] = data::null;
					}
					else {
						//
						switch (meta->getColumnType(i)) {
							case sql::DataType::INTEGER:
							case sql::DataType::DECIMAL:
							case sql::DataType::TINYINT:
							case sql::DataType::SMALLINT:
							case sql::DataType::MEDIUMINT:
							case sql::DataType::BIGINT:
								_values[ meta->getColumnLabel(i) ] = (long) result->getInt(i);
								break;

							case sql::DataType::REAL:
							case sql::DataType::DOUBLE:
							case sql::DataType::NUMERIC:
								_values[ meta->getColumnLabel(i) ] = (double) result->getDouble(i);
								break;

							case sql::DataType::CHAR:
							case sql::DataType::VARCHAR:
							case sql::DataType::LONGVARCHAR:
								_values[ meta->getColumnLabel(i) ] = (std::string) result->getString(i);
								break;

							case sql::DataType::BIT:
							case sql::DataType::SET:
							case sql::DataType::ENUM:
							case sql::DataType::TIME:
							case sql::DataType::DATE:
							case sql::DataType::TIMESTAMP:
							case sql::DataType::BINARY:
							case sql::DataType::VARBINARY:
							case sql::DataType::LONGVARBINARY:
							default:
								_values[ meta->getColumnLabel(i) ] = (std::string) result->getString(i);
								break;
						}
					}
				}
			}

			//
			data::var& get(const std::string& index) { return _values[index]; }

			// implicit conversion
    		operator bool() { return !_values.empty(); } 
    		operator var() { return _values; }

    		template <typename T>
    		operator model<T>() { return data::model<T>(_values); }

    		template <typename T>
    		operator T() { return (T) data::model<T>(_values); }

		protected:
			data::object _values;
	};
	
}
}

#endif