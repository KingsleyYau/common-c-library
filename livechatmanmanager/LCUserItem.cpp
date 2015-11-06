/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCUserItem.cpp
 *   desc: LiveChat用户item
 */

#include "LCUserItem.h"
#include "IAutoLock.h"

LCUserItem::LCUserItem()
{
	m_userId = "";
	m_userName = "";
	m_sexType = USER_SEX_FEMALE;
	m_clientType = CLIENT_ANDROID;
	m_statusType = USTATUS_OFFLINE_OR_HIDDEN;
	m_chatType = LC_CHATTYPE_OTHER;
	m_inviteId = "";
	m_tryingSend = false;
	m_order = 0;

	m_msgListLock = IAutoLock::CreateAutoLock();
	m_sendMsgListLock = IAutoLock::CreateAutoLock();
}

LCUserItem::~LCUserItem()
{
	ClearMsgList();
	IAutoLock::ReleaseAutoLock(m_msgListLock);
	IAutoLock::ReleaseAutoLock(m_sendMsgListLock);
}

// 聊天消息列表加锁
void LCUserItem::LockMsgList()
{
	if (NULL != m_msgListLock) {
		m_msgListLock->Lock();
	}
}

// 聊天消息列表解徜
void LCUserItem::UnlockMsgList()
{
	if (NULL != m_msgListLock) {
		m_msgListLock->Unlock();
	}
}

// 待发送消息列表加锁
void LCUserItem::LockSendMsgList()
{
	if (NULL != m_sendMsgListLock) {
		m_sendMsgListLock->Lock();
	}
}

// 待发送消息列表解锁
void LCUserItem::UnlockMsgList()
{
	if (NULL != m_sendMsgListLock) {
		m_sendMsgListLock->Unlock();
	}
}

// 获取聊天记录列表（已按时间排序）
LCMessageList LCUserItem::GetMsgList()
{
	LockMsgList();
	LCMessageList tempList = m_msgList;
	UnlockMsgList();
	return tempList;
}

// 排序插入聊天记录
bool LCUserItem::InsertSortMsgList(LCMessageItem* item)
{
	LockMsgList();

	// 插入消息
	m_msgList.push_back(item);
	// 排序
	m_msgList.sort(LCMessageItem::Sort());
	// 改变消息的用户
	item->SetUserItem(this);

	UnlockMsgList();

	return true;
}

// 删除聊天记录
bool LCUserItem::RemoveSortMsgList(LCMessageItem* item)
{
	bool result = false;
	if (NULL != item) {
		LockMsgList();
		m_msgList.remove(item);
		UnlockMsgList();
	}
	return result;
}

// 清除所有聊天记录
void LCUserItem::ClearMsgList()
{
	LockMsgList();
	for (LCMessageList::iterator iter = m_msgList.begin();
			iter != m_msgList.end();
			iter ++)
	{
		(*iter)->Clear();
		delete (*iter);
	}
	m_msgList.clear();
	UnlockMsgList();
}

// 清除所有已完成的聊天记录
void LCUserItem::ClearFinishedMsgList()
{
	LockMsgList();

	// 找到所有已完成的聊天记录
	LCMessageList tempList;
	for (LCMessageList::iterator iter = m_msgList.begin();
			iter != m_msgList.end();
			iter++)
	{
		if ((*iter)->statusType == LCMessageItem::StatusType_Finish)
		{
			tempList.push_back(*iter);
		}
	}

	// 移除已完成的聊天记录
	for (LCMessageList::iterator tempIter = tempList.begin();
			tempIter != tempList.end();
			tempIter++)
	{
		m_msgList.remove(*tempIter);
		(*tempIter)->Clear();
		delete (*tempIter);
	}

	UnlockMsgList();
}

// 根据消息ID获取LCMessageItem
LCMessageItem* LCUserItem::GetMsgItemWithId(int msgId)
{
	LCMessageItem* item = NULL;

	LockMsgList();

	for (LCMessageList::iterator iter = m_msgList.begin();
			iter != m_msgList.end();
			iter++)
	{
		if ((*iter)->m_msgId == msgId) {
			item = (*iter);
		}
	}

	UnlockMsgList();

	return item;
}

// 设置聊天状态
void LCUserItem::SetChatTypeWithEventType(TALK_EVENT_TYPE eventType)
{
	switch (eventType) {
	case TET_ENDTALK:
		this.chatType = LC_CHATTYPE_OTHER;
		break;
	case TET_STARTCHARGE:
		if (m_chatType != LC_CHATTYPE_IN_CHAT_CHARGE
			&& m_chatType != LC_CHATTYPE_IN_CHAT_USE_TRY_TICKET)
		{
			this.chatType = LC_CHATTYPE_IN_CHAT_CHARGE;
		}
		break;
	case TET_STOPCHARGE:
		this.chatType = LC_CHATTYPE_OTHER;
		break;
	case TET_NOMONEY:
	case TET_VIDEONOMONEY:
		this.chatType = LC_CHATTYPE_OTHER;
		break;
	case TET_TARGETNOTFOUND:
		this.chatType = LC_CHATTYPE_OTHER;
		break;
	default:
		break;
	}
}

// 设置聊天状态
void LCUserItem::SetChatTypeWithTalkMsgType(bool charge, TALK_MSG_TYPE msgType)
{
	m_chatType = GetChatTypeWithTalkMsgType(charge, msgType);
}

// 根据 TalkMsgType 获取聊天状态
static LCUserItem::ChatType LCUserItem::GetChatTypeWithTalkMsgType(bool charge, TALK_MSG_TYPE msgType)
{
	ChatType chatType = LC_CHATTYPE_OTHER;
	switch(msgType) {
	case TMT_FREE:
		if (!charge) {
			// TMT_FREE 及 charge=false，则为邀请
			m_chatType = LC_CHATTYPE_INVITE;
		}
		else {
			// charge=true，则为InChatCharge
			m_chatType = LC_CHATTYPE_IN_CHAT_CHARGE;
		}
		break;
	case TMT_CHARGE:
		m_chatType = LC_CHATTYPE_IN_CHAT_CHARGE;
		break;
	case TMT_CHARGE_FREE:
		m_chatType = LC_CHATTYPE_IN_CHAT_USE_TRY_TICKET;
		break;
	default:
		m_chatType = LC_CHATTYPE_OTHER;
		break;
	}
	return m_chatType;
}

// 获取对方发出的最后一条聊天消息
LCMessageItem* LCUserItem::GetTheOtherLastMessage()
{
	LCMessageItem* item = NULL;

	LockMsgList();
	if (!m_msgList.empty())
	{
		for (LCMessageList::iterator iter = m_msgList.begin();
			iter != m_msgList.end();
			iter++)
		{
			if ((*iter)->m_sendType == LCMessageItem::SendType_Recv)
			{
				item = (*iter);
			}
		}
	}
	UnlockMsgList();

	return item;
}

// 结束聊天处理
void LCUserItem::EndTalk()
{
	m_chatType = LC_CHATTYPE_OTHER;
	ClearMsgList();
}

/**
 * 获取比较器
 * @return
 */

// 比较函数
bool LCUserItem::Sort(const LCUserItem* item1, const LCUserItem* item2)
{
	// true在前，false在后
	bool result = false;

	item1->LockMsgList();
	item2->LockMsgList();

	if (item1->m_msgList.empty() && item2->m_msgList.empty())
	{
		// 默认按名字排序，a优先，z排最后
		result = item1->m_userName <= item2->m_userName;
	}
	else if (!item1->m_msgList.empty() && !item2->m_msgList.empty())
	{
		// 都有消息
		LCMessageList::iterator iter1 = (item1->m_msgList.end()--);
		LCMessageList::iterator iter2 = (item2->m_msgList.end()--);
		// 按最后一条待发送消息生成时间排序
		result = (*iter1)->m_createTime >= (iter2)->m_createTime;
	}
	else {
		// 有消息的排前面，已经排序成功
		result = !item1->m_msgList.empty();
	}

	item2->UnlockMsgList();
	item1->UnlockMsgList();

	return result;
}
