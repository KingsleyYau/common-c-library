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
	// 消息类型
	typedef enum _CodeType {
		MESSAGE,	// 使用m_message（默认）
		NOMONEY,	// 余额不足
	} CodeType;

public:
	LCWarningItem();
	virtual ~LCWarningItem();

public:
	// 初始化
	bool Init(const string& message);
	bool Init(const string& message, LCWarningLinkItem* linkItem);

public:
	CodeType			m_codeType;		// 消息类型
	string				m_message;		// 消息内容
	LCWarningLinkItem*	m_linkItem;		// 链接item
};
