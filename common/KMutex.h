/*
 * KMutex.h
 *
 *  Created on: 2014/10/27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef _INC_KMutex_
#define _INC_KMutex_

#ifndef _WIN32
#include <pthread.h>

#include "KLog.h"

class KMutex
{
public:
	KMutex(){
		initlock();
	}
	~KMutex(){
		desrtoylock();
	}
	int trylock(){
		return pthread_mutex_trylock(&m_Mutex);
	}
	int lock(){
		return pthread_mutex_lock(&m_Mutex);
	}
	int unlock(){
		return pthread_mutex_unlock(&m_Mutex);
	}
protected:
	int initlock(){
		return pthread_mutex_init(&m_Mutex, NULL);
	}
	int desrtoylock(){
		return pthread_mutex_destroy(&m_Mutex);
	}
private:
	pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_Mutexattr;
};

#else

#include "IAutoLock.h"

class KMutex
{
public:
	KMutex(){
		initlock();
	}
	~KMutex(){
		destroylock();
	}
	int trylock(){
		return m_lock->TryLock() ? 0 : -1;
	}
	int lock(){
		return m_lock->Lock() ? 0 : -1;
	}
	int unlock(){
		return m_lock->Unlock() ? 0 : -1;
	}
protected:
	int initlock(){
		bool result = false;
		m_lock = IAutoLock::CreateAutoLock();
		if (NULL != m_lock) {
			result = m_lock->Init();
		}
		return result ? 0 : -1;
	}
	int destroylock(){
		bool result = false;
		if (NULL != m_lock) {
			delete m_lock;
			result = true;
		}
		return result ? 0 : -1;
	}
private:
	IAutoLock*	m_lock;
};

#endif
#endif
