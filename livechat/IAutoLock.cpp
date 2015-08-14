/*
 * author: Samson.Fan
 *   date: 2015-03-30
 *   file: IAutoLock.cpp
 *   desc: 跨平台锁
 */


#include "IAutoLock.h"

#ifdef WIN32

#include <windows.h>

class WinAutoLock : public IAutoLock
{
public:
	WinAutoLock() {
		m_bInit = false;
	}
	virtual ~WinAutoLock() {
		if (m_bInit) {
			DeleteCriticalSection(&m_section);
			m_bInit = false;
		}
	}

public:
	bool Init() {
		if (!m_bInit) {
			m_bInit = InitializeCriticalSectionAndSpinCount(&m_section, 1) ? true : false;
		}
		return m_bInit;
	}

	void Lock() {
		if (m_bInit) {
			TryEnterCriticalSection(&m_section);
		}
	}

	void Unlock() {
		if (m_bInit) {
			LeaveCriticalSection(&m_section);
		}
	}

private:
	CRITICAL_SECTION	m_section;
	bool				m_bInit;
};

#else

#include <pthread.h>

class LinuxAutoLock : public IAutoLock
{
public:
    LinuxAutoLock() {
        m_bInit = false;
    };

    virtual ~LinuxAutoLock() {
		if (m_bInit) {
			pthread_mutex_destroy(&m_lock);
			m_bInit = false;
		}
    };

public:
	bool Init() {
		if (!m_bInit) {
			m_bInit = pthread_mutex_init(&m_lock, NULL) == 0;
		}
		return m_bInit;
	}

	void Lock() {
		if (m_bInit) {
			pthread_mutex_lock(&m_lock);
		}
	}

	void Unlock() {
		if (m_bInit) {
			pthread_mutex_unlock(&m_lock);
		}
	}

private:
	pthread_mutex_t	m_lock;
	bool			m_bInit;
};

#endif

IAutoLock* IAutoLock::CreateAutoLock()
{
	IAutoLock* lock = NULL;

#ifdef WIN32
	lock = new WinAutoLock();
#else
	lock = new LinuxAutoLock();
#endif

	return lock;
}

void IAutoLock::ReleaseAutoLock(IAutoLock* lock)
{
	delete lock;
}
