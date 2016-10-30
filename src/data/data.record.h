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

				// pull values
				for (auto i = 1; i < meta->getColumnCount() + 1; i++) {
					//
					if (result->isNull(i)) {
						//
						_values[ meta->getColumnLabel(i) ] = data::null;
					}
					else if (meta->isNumeric(i)) {
						//
						_values[ meta->getColumnLabel(i) ] = (long) result->getInt(i);
					}
					else {
						//
						_values[ meta->getColumnLabel(i) ] = (std::string) result->getString(i);
					}
				}
			}

			//
			//data::var & operator [](const char* i) { return _values[i]; }
			//data::var & operator [](const std::string &i) { return _values[i]; }
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