/*
 * KThread.cpp
 *
 *  Created on: 2014/10/27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#include "KThread.h"
#include <time.h>
#include "KLog.h"

#ifndef _WIN32

#ifdef IOS
#define INVALID_PTHREAD NULL
#else
#define INVALID_PTHREAD -1
#endif

KThread::KThread() {
	m_pthread_t = INVALID_PTHREAD;
	m_isRunning = false;
	m_pKRunnable = NULL;
}
KThread::KThread(KRunnable *runnable) {
	m_pthread_t = INVALID_PTHREAD;
	m_isRunning = false;
	this->m_pKRunnable = runnable;
}
KThread::~KThread() {
//	stop();
}
void* KThread::thread_proc_func(void *args){
	KThread *pKThread = (KThread*)args;
	if(pKThread){
//		DLog("KThread", "thread_proc_func ( (%ld)->onRun ) \n ", pKThread->getThreadId());
		pKThread->onRun();
	}
//	DLog("KThread", "thread_proc_func ( (%ld) exit ) \n", pKThread->getThreadId());
	return (void*)0;
}
long KThread::start(KRunnable *runnable){
	if( runnable != NULL ) {
		this->m_pKRunnable = runnable;
	}

	pthread_attr_t attrs;
	pthread_attr_init(&attrs);
	pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);

	int ret = pthread_create(&m_pthread_t, &attrs, &thread_proc_func, (void*)this);
	if(0 != ret) {
		ELog("KThread", "start ( create thread fail reson : (%s) ) \n", strerror(ret));
		m_pthread_t = INVALID_PTHREAD;
	}
	else {
		DLog("KThread", "start ( create thread : (%ld) succeed ) \n", m_pthread_t);
	}
    
    long long startRet = -1;
#ifdef IOS
    startRet = (long long)m_pthread_t;
#else
    startRet = m_pthread_t;
#endif
    
	return startRet;
}
void KThread::stop() {
	if(isRunning()) {
		DLog("KThread", "stop( wait for thread :(%ld) exit... ) \n ", m_pthread_t);
		if(0 != pthread_join(m_pthread_t, NULL)){
			DLog("KThread", "stop( wait for thread :(%ld) exit error fail ) \n", m_pthread_t);
		}
		else{
			DLog("KThread", "thread : (%ld) exit succeed \n", m_pthread_t);
		}
	}
	m_pthread_t = INVALID_PTHREAD;
}
void KThread::sleep(uint32_t msec){
	//usleep(100);
//	struct timespect st;
//	st.tv_sec = msec / 1000;
//	st.tv_nsec = (msec % 1000) * 1000 * 1000;
//	if(-1 == nanosleep(&st, NULL)){
//		showLog("Jni.KThread.sleep", "thread sleep error!");
//	}
}
long KThread::getThreadId() const {
    long ret = -1;
    
#ifdef IOS
    ret = (long long)m_pthread_t;
#else
    ret = m_pthread_t;
#endif
    
	return ret;
}
bool KThread::isRunning() const{
	bool bFlag = false;
	if( m_pthread_t != NULL ) {
		bFlag = true;
	}
	return bFlag;
}
void KThread::onRun() {
	if( NULL != m_pKRunnable ) {
		m_pKRunnable->onRun();
	}
}

#else

KThread::KThread() {
	m_pthread_t = NULL;
	m_threadId = 0;
	m_isRunning = false;
	m_pKRunnable = NULL;
}
KThread::KThread(KRunnable *runnable) {
	m_pthread_t = NULL;
	m_threadId = 0;
	m_isRunning = false;
	this->m_pKRunnable = runnable;
}
KThread::~KThread() {
	CloseHandle(m_pthread_t);
	m_pthread_t = NULL;
}
#ifndef _WIN32
void* KThread::thread_proc_func(void *args)
#else
DWORD WINAPI KThread::thread_proc_func(void *args)
#endif
{
	KThread *pKThread = (KThread*)args;
	if(pKThread){
		DLog("KThread", "thread_proc_func ( (%ld)->onRun ) \n ", pKThread->getThreadId());
		pKThread->onRun();
	}
	//DLog("KThread", "thread_proc_func ( (%ld) exit ) \n", pKThread->getThreadId());
	return NULL;
}
pthread_t KThread::start(KRunnable *runnable){
	if( runnable != NULL ) {
		this->m_pKRunnable = runnable;
	}

	m_pthread_t = CreateThread(NULL, 0, KThread::thread_proc_func, this, 0, &m_threadId);
	if(NULL == m_pthread_t) {
		ELog("KThread", "start ( create thread fail reson : (%x) ) \n", GetLastError());
	}
	else {
		DLog("KThread", "start ( create thread : (%ld) succeed ) \n", m_pthread_t);
	}
	return m_threadId;
}
void KThread::stop() {
	if(isRunning()) {
		DLog("KThread", "stop( wait for thread :(%ld) exit... ) \n ", m_pthread_t);
		if(0 != WaitForSingleObject(m_pthread_t, INFINITE)){
			DLog("KThread", "stop( wait for thread :(%ld) exit error fail ) \n", m_pthread_t);
		}
		else{
			DLog("KThread", "thread : (%ld) exit succeed \n", m_pthread_t);
		}
	}

	CloseHandle(m_pthread_t);
	m_pthread_t = NULL;
}
void KThread::sleep(uint32_t msec){
	//usleep(100);
//	struct timespect st;
//	st.tv_sec = msec / 1000;
//	st.tv_nsec = (msec % 1000) * 1000 * 1000;
//	if(-1 == nanosleep(&st, NULL)){
//		showLog("Jni.KThread.sleep", "thread sleep error!");
//	}
}
pthread_t KThread::getThreadId() const {
	//return pthread_self();
	return m_threadId;
}
bool KThread::isRunning() const{
	bool bFlag = false;
	if( NULL != m_pthread_t ) {
		bFlag = true;
	}
	return bFlag;
}
void KThread::onRun() {
	if( NULL != m_pKRunnable ) {
		m_pKRunnable->onRun();
	}
}

#endif