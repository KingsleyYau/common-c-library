/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCUserItem.h
 *   desc: LiveChat用户item
 */

#pragma once

#include "LCMessageItem.h"
#include "ILiveChatClientDef.h"
#include <string>
#include <list>
using namespace std;

class IAutoLock;
class LCUserItem
{
public:
	// 聊天状态
	typedef enum {
		LC_CHATTYPE_IN_CHAT_CHARGE, 			// 收费
		LC_CHATTYPE_IN_CHAT_USE_TRY_TICKET, 	// 试聊券
		LC_CHATTYPE_INVITE,				 		// 邀请
		LC_CHATTYPE_MANINVITE,					// 男士已经发出邀请
		LC_CHATTYPE_OTHER,			 			// 其它
	} ChatType;

public:
	LCUserItem();
	virtual ~LCUserItem();

public:
	// 获取聊天记录列表（已按时间排序）
	LCMessageList GetMsgList();
	// 排序插入聊天记录
	bool InsertSortMsgList(LCMessageItem* item);
	// 删除聊天记录
	bool RemoveSortMsgList(LCMessageItem* item);
	// 清除所有聊天记录
	void ClearMsgList();
	// 清除所有已完成的聊天记录
	void ClearFinishedMsgList();
	// 根据消息ID获取LCMessageItem
	LCMessageItem* GetMsgItemWithId(int msgId);
	// 设置聊天状态
	void SetChatTypeWithEventType(TALK_EVENT_TYPE eventType);
	// 设置聊天状态
	void SetChatTypeWithTalkMsgType(bool charge, TALK_MSG_TYPE msgType);
	// 根据 TalkMsgType 获取聊天状态
	static ChatType GetChatTypeWithTalkMsgType(bool charge, TALK_MSG_TYPE msgType);
	// 获取对方发出的最后一条聊天消息
	LCMessageItem* GetTheOtherLastMessage();
	// 结束聊天处理
	void EndTalk();

	// 比较函数
	static bool Sort(const LCUserItem* item1, const LCUserItem* item2);

public:
	// 聊天消息列表加锁
	void LockMsgList();
	// 聊天消息列表解徜
	void UnlockMsgList();
	// 待发送消息列表加锁
	void LockSendMsgList();
	// 待发送消息列表解锁
	void UnlockMsgList();


public:
	string			m_userId;			// 用户ID
	string			m_userName;			// 用户名
	USER_SEX_TYPE	m_sexType;			// 用户性别
	CLIENT_TYPE		m_clientType;		// 客户端类型
	USER_STATUS_TYPE	m_statusType;	// 在线状态
	ChatType		m_chatType;			// 聊天状态
	string			m_inviteId;			// 邀请ID
	bool			m_tryingSend;		// 是否正在尝试发送（可能正在检测是否可聊天，包括检测是否有试聊券，是否可使用，是否有足够点数等)
	int				m_order;			// 排序分值（用于邀请排序）
	LCMessageList	m_msgList;			// 聊天消息列表
	IAutoLock*		m_msgListLock;		// 聊天消息列表锁
	LCMessageList	m_sendMsgList;		// 待发消息列表
	IAutoLock*		m_sendMsgListLock;	// 待发消息列表锁
};
typedef list<LCUserItem*>		LCUserList;
