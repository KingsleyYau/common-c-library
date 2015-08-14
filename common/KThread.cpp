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

KThread::KThread() {
	m_pthread_t = -1;
	m_pthread_t_id = 0;
	m_isRunning = false;
	m_pKRunnable = NULL;
}
KThread::KThread(KRunnable *runnable) {
	m_pthread_t = -1;
	m_pthread_t_id = -1;
	m_isRunning = false;
	this->m_pKRunnable = runnable;
}
KThread::~KThread() {
//	stop();
}
void* KThread::thread_proc_func(void *args){
	pthread_t threadId = 0;
	KThread *pKThread = (KThread*)args;
	if(pKThread){
		DLog("KThread", "thread_proc_func ( (%lu)->onRun ) \n ", pKThread->getThreadId());
		threadId = pKThread->getThreadId();
		pKThread->onRun();
	}
	DLog("KThread", "thread_proc_func ( (%lu) exit ) \n", threadId);
	return (void*)0;
}
pthread_t KThread::start(KRunnable *runnable){
	if( runnable != NULL ) {
		this->m_pKRunnable = runnable;
	}

	pthread_attr_t attrs;
	pthread_attr_init(&attrs);
	pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);

	int ret = pthread_create(&m_pthread_t, &attrs, &thread_proc_func, (void*)this);
	if(0 != ret) {
		ELog("KThread", "start ( create thread fail reson : (%s) ) \n", strerror(ret));
		m_pthread_t = -1;
		m_pthread_t_id = -1;
	}
	else {
		DLog("KThread", "start ( create thread : (%lu) succeed ) \n", m_pthread_t);
		m_pthread_t_id = m_pthread_t;
	}
	return m_pthread_t;
}
void KThread::stop() {
	if(isRunning()) {
		DLog("KThread", "stop( wait for thread :(%lu) exit... ) \n ", m_pthread_t);
		if(0 != pthread_join(m_pthread_t, NULL)){
			DLog("KThread", "stop( wait for thread :(%lu) exit error fail ) \n", m_pthread_t);
		}
		else{
			DLog("KThread", "thread : (%lu) exit succeed \n", m_pthread_t);
		}
	}
	else if(m_pthread_t_id != -1){
		DLog("KThread", "stop( thread : (%lu) already exit ) \n", m_pthread_t_id);
	}
	m_pthread_t = -1;
	m_pthread_t_id = -1;
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
	return m_pthread_t;
}
bool KThread::isRunning() const{
	bool bFlag = false;
	if( m_pthread_t != -1 ) {
		bFlag = true;
	}
	return bFlag;
}
void KThread::onRun() {
	if( NULL != m_pKRunnable ) {
		m_pKRunnable->onRun();
	}
}
