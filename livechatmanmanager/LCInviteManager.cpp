/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCInviteManager.h
 *   desc: LiveChat高级表情管理类
 */

#include "LCInviteManager.h"
#include "LCBlockManager.h"
#include "LCContactManager.h"
#include "LCUserManager.h"
#include "LCUserItem.h"
#include "LCMessageItem.h"
#include <livechat/ILiveChatClient.h>
#include <common/IAutoLock.h>
#include <livechat/Counter.h>
#include <common/CommonFunc.h>
#include <common/CheckMemoryLeak.h>

using namespace std;

static long long g_handleTimeInterval = 25 * 1000;	// 处理邀请时间间隔(25s)
static int g_maxNoRandomCount = 10;					// 最大非随机处理数（最多多少次就要有一次随机）

LCInviteManager::LCInviteManager()
{
	m_isInit = false;
	m_userMgr = NULL;
	m_blockMgr = NULL;
	m_contactMgr = NULL;
	m_inviteUserListLock = IAutoLock::CreateAutoLock();
	if (NULL != m_inviteUserListLock) {
		m_inviteUserListLock->Init();
	}

	m_preHandleTime = 0;
	m_handleCount = 0;
	m_randomHandle = 0;
}

LCInviteManager::~LCInviteManager()
{
	IAutoLock::ReleaseAutoLock(m_inviteUserListLock);
}

// 初始化
bool LCInviteManager::Init(
		LCUserManager* userMgr
		, LCBlockManager* blockMgr
		, LCContactManager* contactMgr
		, ILiveChatClient* liveChatClient)
{
	bool result = false;

	if (NULL != userMgr
		&& NULL != liveChatClient
		&& NULL != blockMgr
		&& NULL != contactMgr)
	{
		m_userMgr = userMgr;
		m_blockMgr = blockMgr;
		m_contactMgr = contactMgr;
		m_liveChatClient = liveChatClient;

		result = true;
	}

	m_isInit = result;

	return result;
}

// 判断是否需要处理的邀请消息
LCInviteManager::HandleInviteMsgType LCInviteManager::IsToHandleInviteMsg(
															const string& userId
															, bool charge
															, TALK_MSG_TYPE type)
{
	HandleInviteMsgType result = PASS;
	if (m_isInit) {
		// 已经完成初始化，可以进行过滤
		if (LCUserItem::LC_CHATTYPE_INVITE == LCUserItem::GetChatTypeWithTalkMsgType(charge, type))
		{
			bool handle = false;
			// 是否黑名单用户
			if (m_blockMgr->IsExist(userId)) {
				result = LOST;
				handle = true;
			}

			// 是否联系人
			if (!handle) {
				if (m_contactMgr->IsExist(userId)) {
					result = PASS;
					handle = true;
				}
			}

			// 是否最近联系人
//			if (!handle) {
//				ContactBean contact = ContactManager.getInstance().getContactById(userId);
//				if (null == contact) {
//					result = HandleInviteMsgType.HANDLE;
//					handle = true;
//				}
//			}
		}
	}

	return result;
}

// 获取用户item（不存在不创建）
LCUserItem* LCInviteManager::GetUserNotCreate(const string& userId)
{
	LCUserItem* userItem = NULL;
	for (LCUserList::iterator iter = m_inviteUserList.begin();
			iter != m_inviteUserList.end();
			iter++)
	{
		if (userId == (*iter)->m_userId)
		{
			userItem = (*iter);
		}
	}
	return userItem;
}

// 获取用户item（不存在则创建）
LCUserItem* LCInviteManager::GetUser(const string& userId)
{
	LCUserItem* userItem = GetUserNotCreate(userId);
	if (NULL == userItem)
	{
		userItem = new LCUserItem;
		userItem->m_userId = userId;
	}
	return userItem;
}

// 获取并移除指定邀请用户
LCUserItem* LCInviteManager::GetAndRemoveUserWithPos(int pos)
{
	LCUserItem* userItem = NULL;

	if (m_inviteUserList.size() > pos && pos >= 0)
	{
		// 找元素
		LCUserList::iterator userIter = m_inviteUserList.begin();
		for (int j = 0;	j < pos; userIter++, j++);
		userItem = (*userIter);

		// 移除
		m_inviteUserList.erase(userIter);
	}

	return userItem;
}

// 移除超时邀请
void LCInviteManager::RemoveOverTimeInvite()
{
	int i = 0;
	while (i < m_inviteUserList.size())
	{
		bool removeFlag = true;

		// 找第i个元素
		LCUserList::iterator userIter = m_inviteUserList.begin();
		for (int j = 0;	j < i; userIter++, j++);

		if (m_inviteUserList.end() != userIter && NULL != (*userIter))
		{
			LCUserItem* userItem = (*userIter);
			if (!userItem->m_msgList.empty())
			{
				LCMessageList::iterator msgIter = userItem->m_msgList.begin();
				LCMessageItem* item = (*msgIter);
				long long currentTime = getCurrentTime();
				if (item->m_createTime + g_handleTimeInterval >= currentTime) {
					removeFlag = false;
				}
			}

			if (removeFlag) {
				m_inviteUserList.erase(userIter);
				continue;
			}
		}

		i++;
	}
}

// 插入邀请用户
bool LCInviteManager::InsertInviteUser(LCUserItem* item)
{
	bool result = false;
	if (NULL != item)
	{
		// 插入用户
		m_inviteUserList.push_back(item);
		// 排序
		SortInviteList();
	}
	return result;
}

// 对邀请列表排序
void LCInviteManager::SortInviteList()
{
	m_inviteUserList.sort(LCInviteManager::Sort);
}

// 处理邀请消息
LCMessageItem* LCInviteManager::HandleInviteMessage(
									Counter& msgIdIndex
									, const string& toId
									, const string& fromId
									, const string& fromName
									, const string& inviteId
									, bool charge
									, int ticket
									, TALK_MSG_TYPE msgType
									, const string& message)
{
	LCMessageItem* item = NULL;

	LockInviteUsersList();

	// 移除超时邀请
	RemoveOverTimeInvite();

	// 插入到列表
	{
		// 获取/生成UserItem
		LCUserItem* userItem = GetUser(fromId);
		userItem->m_sexType = USER_SEX_FEMALE;
		userItem->m_inviteId = inviteId;
		userItem->m_userName = fromName;
		userItem->SetChatTypeWithTalkMsgType(charge, msgType);
		userItem->m_statusType = USTATUS_ONLINE;
		// 生成MessageItem
		item = new LCMessageItem;
		item->Init(msgIdIndex.GetAndIncrement()
				, LCMessageItem::SendType_Recv
				, fromId
				, toId
				, inviteId
				, LCMessageItem::StatusType_Finish);
		// 生成TextItem
		LCTextItem* textItem = new LCTextItem;
		textItem->Init(message);
		// 把TextItem添加到MessageItem
		item->SetTextItem(textItem);
		// 添加到用户聊天记录中
		userItem->InsertSortMsgList(item);

		// 插入列表
		InsertInviteUser(userItem);

		// 请求获取用户信息（排序分值）
		m_liveChatClient->GetUserInfo(fromId);
	}

	// 从列表中获取
	if (!m_inviteUserList.empty()
		&& (m_preHandleTime == 0
			|| m_preHandleTime + g_handleTimeInterval <= getCurrentTime()))
	{
		// 生成随机处理次数
		if (m_handleCount == 0) {
			m_randomHandle = GetRandomValue() % g_maxNoRandomCount;
		}

		// 从列表获取邀请用户
		LCUserItem* inviteUserItem = NULL;
		if (m_handleCount == m_randomHandle) {
			// 随机从列表抽出邀请
			int index = GetRandomValue() % m_inviteUserList.size();
			inviteUserItem = GetAndRemoveUserWithPos(index);
		}
		else {
			// 获取列表第一个邀请
			inviteUserItem = GetAndRemoveUserWithPos(0);
		}

		// 获取邀请消息
		if (NULL != inviteUserItem
			&& !inviteUserItem->m_msgList.empty())
		{
			// 添加到UserManager
			LCUserItem* userItem = m_userMgr->GetUserItem(inviteUserItem->m_userId);
			userItem->m_inviteId = inviteUserItem->m_inviteId;
			userItem->m_userName = inviteUserItem->m_userName;
			userItem->m_chatType = inviteUserItem->m_chatType;
			userItem->m_statusType = inviteUserItem->m_statusType;
			userItem->LockMsgList();
			for (LCMessageList::iterator iter = inviteUserItem->m_msgList.begin();
				iter != inviteUserItem->m_msgList.end();
				iter++)
			{
				userItem->InsertSortMsgList(*iter);
			}
			userItem->UnlockMsgList();

			// 抛出最后一条消息给外面显示
			if (!userItem->m_msgList.empty()) {
				LCMessageList::iterator iter = (userItem->m_msgList.end()--);
				item = (*iter);
			}
		}

		// 更新处理次数
		m_handleCount = (m_handleCount + 1) % g_maxNoRandomCount;
		// 更新处理时间
		m_preHandleTime = getCurrentTime();
	}
	else {
		item = NULL;
	}

	UnlockInviteUsersList();

	return item;
}

// 更新用户排序分值
void LCInviteManager::UpdateUserOrderValue(const string& userId, int orderValue)
{
	LockInviteUsersList();
	LCUserItem* item = GetUserNotCreate(userId);
	if (NULL != item) {
		item->m_order = orderValue;
		SortInviteList();
	}
	UnlockInviteUsersList();
}

// 比较函数
bool LCInviteManager::Sort(LCUserItem* item1, LCUserItem* item2)
{
	// true在前面，false在后面
	bool result = false;

	if (item1->m_order == item2->m_order)
	{
		item1->LockMsgList();
		item2->LockMsgList();
		if (!item1->m_msgList.empty() && !item2->m_msgList.empty())
		{
			// 消息早收到的优先
			LCMessageList::iterator iter1 = item1->m_msgList.begin();
			LCMessageList::iterator iter2 = item2->m_msgList.begin();
			result = ((*iter1)->m_createTime >= (*iter2)->m_createTime);
		}
		else if (item1->m_msgList.empty() && item2->m_msgList.empty())
		{
			// 按原来顺序（不应该出现的情况）
			result = true;
		}
		else {
			// 有消息的优先（不应该出现的情况）
			result = !item1->m_msgList.empty();
		}
		item2->UnlockMsgList();
		item1->UnlockMsgList();
	}
	else
	{
		// 按分数排序
		result = (item1->m_order > item2->m_order);
	}

	return result;
}

// 邀请用户列表加锁
void LCInviteManager::LockInviteUsersList()
{
	if (NULL != m_inviteUserListLock) {
		m_inviteUserListLock->Lock();
	}
}

// 邀请用户列表解锁
void LCInviteManager::UnlockInviteUsersList()
{
	if (NULL != m_inviteUserListLock) {
		m_inviteUserListLock->Unlock();
	}
}
