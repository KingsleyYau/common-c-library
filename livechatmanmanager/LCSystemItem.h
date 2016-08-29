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
	// 消息类型
	typedef enum _CodeType
	{
		MESSAGE,				// 使用m_message（默认）
		TRY_CHAT_END,			// 试聊结束
		NOT_SUPPORT_TEXT,		// 不支持文本消息
		NOT_SUPPORT_EMOTION,	// 不支持高级表情消息
		NOT_SUPPORT_VOICE,		// 不支持语音消息
		NOT_SUPPORT_PHOTO,		// 不支持私密照消息
		NOT_SUPPORT_MAGICICON,	// 不支持小高表消息
	} CodeType;

public:
	LCSystemItem();
	virtual ~LCSystemItem();

public:
	bool Init(const string& message);

public:
	CodeType	m_codeType;	// 消息类型
	string	m_message;		// 消息内容
};
