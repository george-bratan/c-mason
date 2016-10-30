#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <initializer_list>

#include <mysql_connection.h>
#include <mysql/driver.h>
#include <mysql/statement.h>
#include <mysql/prepared_statement.h>
#include <mysql/resultset.h>

#ifndef DATA_H
#define DATA_H

#include "data.var.h"
#include "data.model.h"
#include "data.record.h"
#include "data.query.h"

namespace mason {
namespace data {

	//
	static sql::Driver *_driver;
	static sql::Connection *_connection;

	//
	static void connect(const std::string& host, const std::string& user, const std::string& pass)
	{
		_driver = get_driver_instance();
		_connection = _driver->connect(host, user, pass);
	}

	static void schema(const std::string& name) 
	{
		_connection->setSchema(name);
	}

	static void disconnect()
	{
		delete _connection;
	}

	static sql::Connection * connection()
	{
		return _connection;
	}

}
}

#endif