#include "Thread.h"

void * Thread::runnableEntry(void * pThread)
{

	((Thread *)pThread)->_pRunnableTarget->run();

	return 0;
 }


void Thread::start(Runnable& target)
{
	_pRunnableTarget = &target;
	pthread_create(&_thread, NULL, runnableEntry, this);
}
