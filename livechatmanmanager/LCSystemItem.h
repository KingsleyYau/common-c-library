/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCSystemItem.h
 *   desc: LiveChat系统消息item
 */

#pragma once

#include <string>
using namespace std;

class LCSystemItem
{
public:
	LCSystemItem();
	virtual ~LCSystemItem();

public:
	bool Init(const string& message);

public:
	string	m_message;		// 消息内容
};
