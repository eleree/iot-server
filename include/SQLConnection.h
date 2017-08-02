#pragma once

#ifndef __IOT_SQL_CONNECTION_H__
#define __IOT_SQL_CONNECTION_H__

#include <stdio.h>
#include <stdint.h>
#include <string>

#include <mysql\mysql.h>
#include <mysql\my_global.h>

using namespace std;
	
class SQLResult
{
public:
	SQLResult() {}
	~SQLResult() {}


	int32_t capacity(void);

	bool next(void);
	bool previous(void);

	bool getInt(string columnName, int32_t * data, int32_t count);
	bool getInt64(string columnName, int64_t * data, int32_t count);
	bool getString(string columnName, string * data, int32_t count);

private:
	int32_t _result;
};

class SQLConnection
{
public:
	SQLConnection() {}
	~SQLConnection() {}

	bool setScheme(string scheme);
	bool isConnected(void);
	bool isClosed(void);

	int32_t execute(void);

	SQLResult * query(string stat);

	int32_t update(string stat);

	SQLConnection& statement(string stat);

	/* prepare statement bind */
	void setBigInt(unsigned int parameterIndex, const string value);

	void setDouble(unsigned int parameterIndex, double value);

	void setInt(unsigned int parameterIndex, int32_t value);

	void setInt64(unsigned int parameterIndex, int64_t value);

	void setNull(unsigned int parameterIndex, int sqlType);



private:
	std::ostringstream _stream;
	string _server;
	string _user;
	string _password;
	string _scheme;
	uint16_t _port;
	MYSQL * _connection;
};
#endif