#pragma once
#ifndef __IOT_RUNNABLE_H__
#define __IOT_RUNNABLE_H__

class  Runnable
	/// The Runnable interface with the run() method
	/// must be implemented by classes that provide
	/// an entry point for a thread.
{
public:
	Runnable();
	virtual ~Runnable();

	virtual void run() = 0;
	/// Do whatever the thread needs to do. Must
	/// be overridden by subclasses.
};

#endif