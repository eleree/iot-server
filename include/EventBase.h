#pragma once

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>

#include "Runnable.h"
#include "Thread.h"

class EventBase: Runnable
{
public:
	EventBase() 
	{
		evthread_use_windows_threads();
		_base = event_base_new();
	}

	~EventBase() 
	{
		event_base_free(_base);
	}

	struct event_base * getEventBase(void)
	{
		return _base;
	}

	void stop(void)
	{
		_stop = true;
	}

	int32_t loop(void)
	{
		event_base_loop(_base, EVLOOP_ONCE);
		return 0;
	}

	void run(void)
	{
		printf("start event base thread\r\n");
		_stop = false;
		while(!_stop)
			event_base_loop(_base, EVLOOP_ONCE);
	}

	int32_t startLoopThread()
	{
		_thread.start(*this);
		return 0;
	}

private:
	struct event_base* _base;
	Thread _thread;
	bool _stop;
};