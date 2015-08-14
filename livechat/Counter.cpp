/*
 * author: Samson.Fan
 *   date: 2015-03-30
 *   file: Counter.cpp
 *   desc: 计数器（带锁）
 */

#include <stdio.h>
#include "Counter.h"
#include "IAutoLock.h"
#include "CommonDef.h"

Counter::Counter(void)
{
	m_count = 0;
	m_lock = NULL;
	m_bInit = false;
}

Counter::~Counter(void)
{
	if (m_bInit) {
		IAutoLock::ReleaseAutoLock(m_lock);
		m_lock = NULL;
	}
}

// 初始化
bool Counter::Init()
{
	if (!m_bInit) {
		m_lock = IAutoLock::CreateAutoLock();
		if (NULL != m_lock) {
			m_bInit = m_lock->Init();
		}
	}
	return m_bInit;
}

// 重置计数
void Counter::Reset()
{
	if (m_bInit) {
		m_lock->Lock();
		m_count = 0;
		m_lock->Unlock();
	}
}
	
// 获取当前计数
int Counter::GetCount()
{
	int count = 0;
	if (m_bInit) {
		m_lock->Lock();
		count = m_count;
		m_lock->Unlock();
	}
	return count;
}
	
// 加1
void Counter::AddOne()
{
	if (m_bInit) {
		m_lock->Lock();
		m_count++;
		m_lock->Unlock();
	}
}
	
// 减1
void Counter::SubOne()
{
	if (m_bInit) {
		m_lock->Lock();
		m_count--;
		m_lock->Unlock();
	}
}

// 获取计数并加1
int Counter::GetCountAndAddOne()
{
	int count = 0;
	if (m_bInit) {
		m_lock->Lock();
		count = m_count++;
		m_lock->Unlock();
	}
	return count;
}
