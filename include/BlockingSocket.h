#pragma once

#include "EventBase.h"
#include <event2/event.h>  
#include <event2/event_struct.h>  
#include <pthread/pthread.h>
#include <string>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

using namespace std;

class BlockingSocket
{
public:
	enum {
		SOCKET_STATUS_ERROR = -3,
		SOCKET_STATUS_TIMEOUT = -2,
		SOCKET_STATUS_DISCONNECTED = -1,
		SOCKET_STATUS_OK = 0,
		SOCKET_STATUS_CONNECTED = 1
	};

	BlockingSocket(EventBase * eventBase,  string serverAddr, uint16_t port)
	{
		pthread_mutex_init(&_mutex, NULL);
		pthread_cond_init(&_rcond, NULL);
		pthread_cond_init(&_wcond, NULL);
		_port = port;
		_serverAddr = serverAddr;
		_eventBase = eventBase;
		memset(&_address, 0, sizeof(_address));
		_address.sin_family = AF_INET;
		inet_pton(AF_INET, _serverAddr.c_str(), &_address.sin_addr);
		_address.sin_port = htons(_port);
		_fd = socket(AF_INET, SOCK_STREAM, 0);
		_conn = bufferevent_socket_new(_eventBase->getEventBase(), _fd, BEV_OPT_CLOSE_ON_FREE);
		bufferevent_setcb(_conn, internelReadCallback, NULL, internelEventCallback, this);
	}

	~BlockingSocket() {}

	int32_t connect(void)
	{
		int enable = 1;
		if (setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, (char*)&enable, sizeof(enable)) < 0)
			printf("ERROR: TCP_NODELAY SETTING ERROR!\n");

#if 1
		int rc = ::connect(_fd, (struct sockaddr*)&_address, sizeof(_address));
		if (rc == SOCKET_ERROR)
		{
			printf("connect socket error\r\n");
			return SOCKET_STATUS_ERROR;
		}

		_connected = SOCKET_STATUS_CONNECTED;

		return SOCKET_STATUS_OK;
#else
		if (bufferevent_socket_connect(_conn, (struct sockaddr*)&_address, sizeof(_address)) == 0)
		{
			return 0;
		}
#endif
		return -1;
	}

	int32_t write(uint8_t const* buf, size_t len) 
	{
		if (_connected != SOCKET_STATUS_CONNECTED)
			return SOCKET_STATUS_DISCONNECTED;

		if (bufferevent_write(_conn, buf, len) == 0)
			return len;
		else
			return SOCKET_STATUS_ERROR;
	}

	int32_t read(uint8_t* buf, size_t len, uint32_t timeout)
	{
		size_t targetLen = len;
		struct timespec ts;
		ts.tv_sec = timeout / 1000 + time(NULL);
		ts.tv_nsec = (timeout % 1000) * 1000 * 1000;

		pthread_mutex_lock(&_mutex);

		struct evbuffer *buffer = bufferevent_get_input(_conn);
		size_t length = evbuffer_get_length(buffer);

		if (length >= len)
		{
			bufferevent_read(_conn, buf, len);
			pthread_mutex_unlock(&_mutex);
			return len;
		}
		else
		{
			bufferevent_read(_conn, buf, length);
			buf += length;
			len -= length;
		}

		/* blocking here, we may not be signaled after timeout, so we read buffer once before exit loop */
		bufferevent_enable(_conn, EV_READ | EV_WRITE);
		while (len != 0)
		{
			int ret = pthread_cond_timedwait(&_rcond, &_mutex, &ts);
			length = evbuffer_get_length(buffer);
			bufferevent_read(_conn, buf, MIN(length, len));
			buf += MIN(length, len);
			len -= MIN(length, len);
			if (ret == ETIMEDOUT)
			{
				break;
			}						
			printf("len %d\r\n", len);
		}
		bufferevent_enable(_conn, EV_WRITE);
		pthread_mutex_unlock(&_mutex);

		if (_connected != SOCKET_STATUS_CONNECTED)
			return SOCKET_STATUS_DISCONNECTED;

		return targetLen - len;
	}

	int32_t write(uint8_t const* buf, size_t len, uint32_t timeout)
	{
		return bufferevent_write(_conn, buf, len);
	}

	void close(void)
	{
#ifdef _WIN32
		::closesocket(_fd);
#else
		close(fd);
#endif
		bufferevent_free(_conn);
		pthread_mutex_destroy(&_mutex);
		pthread_cond_destroy(&_rcond);
		pthread_cond_destroy(&_wcond);
	}

private:
	int32_t _connected;
	EventBase * _eventBase;
	evutil_socket_t _fd;
	bufferevent * _conn;
	string _serverAddr;
	uint16_t _port;
	struct sockaddr_in _address;

	pthread_mutex_t _mutex;
	pthread_cond_t _rcond;
	pthread_cond_t _wcond;

	static void internelReadCallback(struct bufferevent* bev, void* ctx)
	{
		BlockingSocket * socket = (BlockingSocket *)ctx;
		pthread_cond_broadcast(&socket->_rcond);
	}

	static void internelWriteCallback(struct bufferevent* bev, void* ctx)
	{
		BlockingSocket * socket = (BlockingSocket *)ctx;
		pthread_cond_broadcast(&socket->_wcond);
	}

	static void  internelEventCallback(struct bufferevent* bev, int16_t events, void* ctx)
	{
		BlockingSocket * socket = (BlockingSocket*)ctx;
		//printf("got callback %d\n", events);
		if (events & BEV_EVENT_CONNECTED) {
			printf("CONNECTED\n");
		}
		if (events & BEV_EVENT_EOF) {
			printf("EOF\n");
			socket->_connected = 0;
		}
		if (events & BEV_EVENT_TIMEOUT) {
			printf("TIMEOUT\n");
		}
		if (events & BEV_EVENT_ERROR) {
			socket->_connected = 0;
			printf("ERROR %s\n", evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
		}
		if (events & BEV_EVENT_READING) {
			printf("READING\n");
		}
		if (events & BEV_EVENT_WRITING) {
			printf("WRITING\n");
		}

	}
};