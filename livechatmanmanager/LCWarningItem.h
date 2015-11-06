/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCWarningItem.h
 *   desc: LiveChat警告消息item
 */

#pragma once

#include "LCWarningLinkItem.h"
#include <string>
using namespace std;

class LCWarningItem
{
public:
	LCWarningItem();
	virtual ~LCWarningItem();

public:
	// 初始化
	bool Init(const string& message);
	bool Init(const string& message, const LCWarningLinkItem& linkItem);

public:
	string				m_message;		// 消息内容
	LCWarningLinkItem	m_linkItem;		// 链接item
};
