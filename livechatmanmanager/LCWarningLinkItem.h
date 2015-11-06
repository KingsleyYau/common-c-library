/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCWarningLinkItem.h
 *   desc: LiveChat警告消息的link item
 */

#pragma once

#include <string>
using namespace std;

class LCWarningLinkItem
{
public:
	// 链接操作类型
	typedef enum {
		Unknow,			// 默认/未知
		Rechange,		// 充值
	} LinkOptType;

public:
	LCWarningLinkItem();
	LCWarningLinkItem(const LCWarningLinkItem& item);
	virtual ~LCWarningLinkItem();

public:
	// 初始化
	bool Init(const string& linkMsg, LinkOptType linkOptType);

public:
	string		m_linkMsg;		// 链接文字内容
	LinkOptType	m_linkOptType;	// 链接操作类型
};
