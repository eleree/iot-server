#pragma once

#include <stdint.h>
#include <pthread\pthread.h>
#include <queue>

using namespace std;

template <class T>
class MessageQueue
{
public:
	enum {
		MESSAGE_QUEUE_TIMEOUT = -3 ,
		MESSAGE_QUEUE_FULL,
		MESSAGE_QUEUE_ERROR,
		MESSAGE_QUEUE_OK = 0,
		MESSAGE_QUEUE_MAX,
	};

	MessageQueue(uint32_t len)
	{		
		pthread_mutex_init(_mutex, NULL);
		pthread_cond_init(_readCond, NULL);
		pthread_cond_init(_writeCond, NULL);
	}
	
	int32_t enqueue(T message, uint32_t timeout)
	{
		pthread_mutex_lock(&_mutex);
		pthread_mutex_unlock(&_mutex);
		return MESSAGE_QUEUE_OK;
	}

	T dequeue(void)
	{
		T message;
		pthread_mutex_lock(&_mutex);
		if (_queue.empty())
			pthread_cond_wait(&_readCond, _mutex);
		message = _queue.front();
		_queue.pop();

		if (_maxLen - 1 == _queue.size())
			pthread_cond_broadcast(_writeCond);

		pthread_mutex_unlock(&_mutex);
		return message;
	}

	int32_t dequeue(T& message, uint32_t timeout)
	{
		T message;
		struct timespec ts;
		ts.tv_sec = timeout / 1000;
		ts.tv_nsec = (timeout % 1000) * 1000 * 1000;
		pthread_mutex_lock(&_mutex);
		if (_queue.empty())
		{
			int ret = pthread_cond_timedwait(&_readCond, &_mutex, &ts);
			if (ret == ETIMEDOUT)
			{
				pthread_mutex_unlock(&_mutex);
				return MESSAGE_QUEUE_TIMEOUT;
			}			
		}

		message = _queue.front();
		_queue.pop();

		if (_maxLen - 1 == _queue.size())
			pthread_cond_broadcast(_writeCond);

		pthread_mutex_unlock(&_mutex);
		return message;
	}

private:
	std::queue<T> _queue;
	uint32_t _maxLen;
	pthread_mutex_t _mutex;
	pthread_cond_t _readCond;
	pthread_cond_t _writeCond;

};