/*
 * author: Samson.Fan
 *   date: 2015-03-30
 *   file: Counter.h
 *   desc: 计数器（带锁）
 */

#pragma once

class IAutoLock;
class Counter
{
public:
	Counter(void);
	virtual ~Counter(void);

public:
	// 初始化
	bool Init();
	// 重置计数
	void Reset();
	// 获取当前计数
	int GetCount();
	// 加1
	void AddOne();
	// 减1
	void SubOne();
	// 获取计数并加1
	int GetCountAndAddOne();

private:
	int			m_count;	// 计数
	IAutoLock*	m_lock;		// 锁
	bool		m_bInit;	// 初始化标志
};
