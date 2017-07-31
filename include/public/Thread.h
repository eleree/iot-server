#pragma once
#ifndef __IOT_THREAD_H__
#define __IOT_THREAD_H__

#include <stdint.h>
#include "pthread.h"
#include "Runnable.h"


class Thread
{
public:
	void start(Runnable& target);

	void join();

	bool isRunning() const;

	static void yield();

	static void * runnableEntry(void * pThread);

	Runnable * _pRunnableTarget;

private:
	pthread_t _thread;
};

#endif