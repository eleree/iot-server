#pragma once

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <string>

using namespace std;

class TCPClient
{
public:
	TCPClient(string serverAddr, uint16_t port, uint32_t timeout)
	{
		_port = port;
		_serverAddr = serverAddr;
		memset(&_address, 0, sizeof(_address));
		_address.sin_family = AF_INET;
		inet_pton(AF_INET, _serverAddr.c_str(), &_address.sin_addr);
		_address.sin_port = htons(_port);	

		_base = event_base_new();
		_fd = socket(AF_INET, SOCK_STREAM, 0);
		_conn = bufferevent_socket_new(_base, _fd, BEV_OPT_CLOSE_ON_FREE);

		setTimeout(&_writeTimeout, timeout);
		setTimeout(&_readTimeout, timeout);
		bufferevent_set_timeouts(_conn, &_readTimeout, &_writeTimeout);
		bufferevent_setcb(_conn, conn_readcb, conn_writecb, conn_eventcb, this);

	}

	~TCPClient() {
		bufferevent_free(_conn);
		event_base_free(_base);
	}

	int32_t connect(string serverAddr, uint16_t port, uint32_t timeout)
	{

		int enable = 1;
		if (setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, (char*)&enable, sizeof(enable)) < 0)
			printf("ERROR: TCP_NODELAY SETTING ERROR!\n");
		//bufferevent_setcb(conn, NULL, NULL, NULL, NULL); // For client, we don't need callback function
		if (bufferevent_socket_connect(_conn, (struct sockaddr*)&_address, sizeof(_address)) == 0)
		{
			printf("connect success\n");
			return 0;
		}

		/* connect failed */
		return -1;
	}

	int32_t write(uint8_t * data, uint32_t length, uint32_t timeout)
	{
		bufferevent_enable(_conn, EV_WRITE);
		setTimeout(&_writeTimeout, timeout);
		bufferevent_set_timeouts(_conn, &_readTimeout, &_writeTimeout);
		bufferevent_write(_conn, data, length);		
		event_base_dispatch(_base);
		return 0;
	}

	int32_t read(uint8_t * data, uint32_t targetLen, uint32_t timeout)
	{
		_buffer = data;
		_readLength = targetLen;
		bufferevent_enable(_conn, EV_READ);
		setTimeout(&_readTimeout, timeout);
		bufferevent_set_timeouts(_conn, &_readTimeout, &_writeTimeout);
		event_base_loopexit(_base, &_readTimeout);
		event_base_dispatch(_base);
		return targetLen - _readLength;
	}

	int32_t close(void) 
	{
		return 0;
	}

	int32_t isConnected(void);

	static void conn_readcb(struct bufferevent *bev, void *user_data)
	{
		int32_t readLen = 0;
		TCPClient * client = (TCPClient *)user_data;
		printf("read_cb\r\n");

		while (client->_readLength > 0)
		{
			readLen = bufferevent_read(client->_conn, client->_buffer, client->_readLength);
			if (readLen < 0)
			{
				event_base_loopexit(client->_base, NULL);
				return;
			}
			client->_readLength -= readLen;
		}

		printf("readlen %d\r\n", client->_readLength);
		event_base_loopexit(client->_base, NULL);
	}

	static void conn_writecb(struct bufferevent *bev, void *user_data)
	{
		printf("write_cb\r\n");
	}

	static void conn_eventcb(struct bufferevent *bev, short events, void *user_data)
	{
		printf("libevent callback %d\n", events);
	}

private:
	bool _isConnected;
	evutil_socket_t _fd;
	struct event_base* _base;
	struct bufferevent * _conn;
	struct sockaddr_in _address;
	struct timeval _readTimeout;
	struct timeval _writeTimeout;
	string _serverAddr;
	uint16_t _port;
	uint8_t * _buffer;
	int32_t _writeLength;
	int32_t _readLength;

	struct timeval * setTimeout(struct timeval * timeVal, uint32_t time)
	{
		timeVal->tv_sec = time/1000;
		timeVal->tv_usec = (time % 1000) * 1000;
		return timeVal;
	}
};