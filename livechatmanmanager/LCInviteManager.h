/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCInviteManager.h
 *   desc: LiveChat高级表情管理类
 */

#pragma once

#include <livechat/ILiveChatClientDef.h>
#include "LCUserItem.h"
#include <string>
using namespace std;

class ILiveChatClient;
class LCBlockManager;
class LCContactManager;
class LCUserManager;
class LCUserItem;
class LCMessageItem;
class IAutoLock;
class Counter;

class LCInviteManager
{
public:
	// 邀请消息处理结果类型
	typedef enum {
		LOST,	// 丢弃
		PASS,	// 通过
		HANDLE,	// 需要处理
	} HandleInviteMsgType;

public:
	LCInviteManager();
	virtual ~LCInviteManager();

public:
	// 初始化
	bool Init(LCUserManager* userMgr, LCBlockManager* blockMgr, LCContactManager* contactMgr, ILiveChatClient* liveChatClient);
	// 判断是否需要处理的邀请消息
	HandleInviteMsgType IsToHandleInviteMsg(const string& userId, bool charge, TALK_MSG_TYPE type);
	// 更新用户排序分值
	void UpdateUserOrderValue(const string& userId, int orderValue);
	// 处理邀请消息
	LCMessageItem* HandleInviteMessage(
			Counter& msgIdIndex
			, const string& toId
			, const string& fromId
			, const string& fromName
			, const string& inviteId
			, bool charget
			, int ticket
			, TALK_MSG_TYPE msgType
			, const string& message);

private:
	// 获取用户item（不存在不创建）
	LCUserItem* GetUserNotCreate(const string& userId);
	// 获取用户item（不存在则创建）
	LCUserItem* GetUser(const string& userId);
	// 获取并移除指定邀请用户
	LCUserItem* GetAndRemoveUserWithPos(int pos);
	// 移除超时邀请
	void RemoveOverTimeInvite();
	// 插入邀请用户
	bool InsertInviteUser(LCUserItem* item);
	// 对邀请列表排序
	void SortInviteList();
	// 邀请用户列表加锁
	void LockInviteUsersList();
	// 邀请用户列表解锁
	void UnlockInviteUsersList();

	// 比较函数
	static bool Sort(LCUserItem* item1, LCUserItem* item2);

private:
	bool				m_isInit;			// 是否已初始化
	ILiveChatClient*	m_liveChatClient;	// LiveChat客户端
	LCUserManager* 		m_userMgr;			// 用户管理器
	LCBlockManager*		m_blockMgr;			// 黑名单管理器
	LCContactManager*	m_contactMgr;		// LiveChat联系人管理器
	LCUserList			m_inviteUserList;	// 邀请用户列表
	IAutoLock*			m_inviteUserListLock;	// 邀请用户列表锁

	long long			m_preHandleTime;	// 上一次处理邀请的时间
	int 				m_handleCount;		// 当前处理次数
	int 				m_randomHandle;		// 随机处理次数（第几次处理是随机的）
};
