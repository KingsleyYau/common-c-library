/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCTextItem.h
 *   desc: LiveChat文本消息item
 */

#pragma once

#include <string>
using namespace std;

class LCTextItem
{
public:
	LCTextItem();
	virtual ~LCTextItem();

public:
	// 初始化
	bool Init(const string& message);

public:
	string	m_message;		// 消息内容
	bool 	m_illegal;		// 内容是否非法
};
