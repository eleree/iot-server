#include "AuthServer.h"

#pragma warning( disable : 4996)

AuthServer::AuthServer():_stopped(true)
{
	pthread_mutex_init(&_mutex, NULL);

}

AuthServer::~AuthServer()
{
	pthread_mutex_destroy(&_mutex);
}

int32_t AuthServer::start(void)
{
	_thread.start(*this);
	_stopped = false;
	return 0;
}

int32_t AuthServer::stop(void)
{	
	return event_base_loopbreak(_base);	
}

#define MESSAGE "hello world\n"
void AuthServer::listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
	struct sockaddr *sa, int socklen, void *user_data)
{
	struct event_base *base = (struct event_base *)user_data;
	struct bufferevent *bev;

	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(base);
		return;
	}
	bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, NULL);
	bufferevent_enable(bev, EV_WRITE|EV_READ);

	bufferevent_write(bev, MESSAGE, strlen(MESSAGE));
}

void AuthServer::conn_readcb(struct bufferevent *bev, void *user_data)
{
	char buf[1024];
	int n;
	struct evbuffer *input = bufferevent_get_input(bev);
	while ((n = evbuffer_remove(input, buf, sizeof(buf))) > 0) {
		//fwrite(buf, 1, n, stdout);
		bufferevent_write(bev, buf, n);
	}
}

void AuthServer::conn_writecb(struct bufferevent *bev, void *user_data)
{
	struct evbuffer *output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0) {
		printf("flushed answer\n");
		//bufferevent_free(bev);
	}
}

void AuthServer::conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	}
	else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n",
			strerror(errno));/*XXX win32*/
	}
	/* None of the other events can happen here, since we haven't enabled
	* timeouts */
	bufferevent_free(bev);
}

void AuthServer::signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = (struct event_base *)user_data;
	struct timeval delay = { 2, 0 };

	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

	event_base_loopexit(base, &delay);
}


void AuthServer::run(void)
{
	_base = event_base_new();
	if (!_base) {
		fprintf(stderr, "Could not initialize libevent!\n");
	}
	memset(&_sin, 0, sizeof(_sin));
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(9995);

	_listener = evconnlistener_new_bind(_base, listener_cb, (void *)_base,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
		(struct sockaddr*)&_sin,
		sizeof(_sin));

	if (!_listener) {
		fprintf(stderr, "Could not create a listener!\n");
		return ;
	}

	_signal_event = evsignal_new(_base, SIGINT, signal_cb, (void *)_base);

	if (!_signal_event || event_add(_signal_event, NULL)<0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return;
	}

	event_base_dispatch(_base);
	
	evconnlistener_free(_listener);
	event_free(_signal_event);
	event_base_free(_base);

	_stopped = true;

	return;
}