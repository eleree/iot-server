#pragma once
#ifndef __IOT_AUTH_SERVER_H__
#define __IOT_AUTH_SERVER_H__

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#ifndef _WIN32
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#endif

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

class AuthServer
{
public:
	AuthServer();
	~AuthServer();
	int32_t start(void);
	int32_t stop(void);
	static void listener_cb(struct evconnlistener *, evutil_socket_t,struct sockaddr *, int socklen, void *);
	static void conn_writecb(struct bufferevent *, void *);
	static void conn_eventcb(struct bufferevent *, short, void *);
	static void signal_cb(evutil_socket_t, short, void *);
private:
	struct event_base * _base;
	struct evconnlistener * _listener;
	struct event * _signal_event;

	struct sockaddr_in _sin;
};

#endif