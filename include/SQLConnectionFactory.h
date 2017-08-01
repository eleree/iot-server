#pragma once

#ifndef __IOT_SQL_CONNECTOR_H__
#define __IOT_SQL_CONNECTOR_H__

#include <stdio.h>
#include <string>
#include <mysql\mysql.h>
#include <mysql\my_global.h>

using namespace std;

class SQLConnectionFactory
{
public:
	SQLConnectionFactory();
	SQLConnectionFactory::SQLConnectionFactory(const char * server, uint16_t port, const char * username, const char * password);
	~SQLConnectionFactory();
private:
	string _server;
	string _user;
	string _password;
	uint16_t _port;
	MYSQL * _connection;

};

#endif