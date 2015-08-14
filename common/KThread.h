/*
 * KThread.h
 *
 *  Created on: 2014/10/27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef _INC_KThread_
#define _INC_KThread_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "KMutex.h"
#include "KCond.h"
#include "KRunnable.h"

class KThread
{
public:
	KThread();
	KThread(KRunnable *runnable);
	virtual ~KThread();
	pthread_t start(KRunnable *runnable = NULL);
	void stop();
	void sleep(uint32_t msec);
	bool isRunning() const;
	pthread_t getThreadId() const;
protected:
	virtual void onRun();
private:
	KRunnable *m_pKRunnable;
	bool m_isRunning;
	pthread_t m_pthread_t;
	pthread_t m_pthread_t_id; // 只在退出线程时候输出log用
	static void *thread_proc_func(void *args);
};
#endif
