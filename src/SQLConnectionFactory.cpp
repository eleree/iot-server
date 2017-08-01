#include "SQLConnectionFactory.h"
#if 0
static void SQLConnectorTest(void)
{
	MYSQL *con = mysql_init(NULL);
	if (con == NULL)
	{
		fprintf(stderr, "mysql_init() failed\n");
		exit(1);
	}
	if (mysql_real_connect(con, "localhost", "root", "123456",
		"system", 0, NULL, 0) == NULL)
	{
		exit(1);
	}
	if (mysql_query(con, "SELECT * FROM device LIMIT 3"))
	{
		exit(1);
	}
	MYSQL_RES *result = mysql_store_result(con);
	int num_fields = mysql_num_fields(result);
	MYSQL_ROW row;
	MYSQL_FIELD *field;
	while ((row = mysql_fetch_row(result)))
	{
		for (int i = 0; i < num_fields; i++)
		{
			if (i == 0)
			{
				while (field = mysql_fetch_field(result))
				{
					printf("%s ", field->name);
				}
				printf("\n");
			}
			printf("%s  ", row[i] ? row[i] : "NULL");
		}
	}
	printf("\n");
	mysql_free_result(result);
	mysql_close(con);
}
#endif


SQLConnectionFactory::SQLConnectionFactory()
{

}

SQLConnectionFactory::SQLConnectionFactory(const char * server, uint16_t port, const char * username, const char * password) :
	_server(server), _port(port)
{
	_connection = mysql_init(NULL);

}

SQLConnectionFactory::~SQLConnectionFactory()
{
	mysql_close(_connection);
}