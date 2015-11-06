/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCBlockManager.h
 *   desc: LiveChat黑名单管理器
 */

#pragma once

#include <ILiveChatClientDef.h>
#include <IAutoLock.h>
#include <string>
#include <map>


using namespace std;

class LCBlockManager
{
private:
	typedef map<string, bool> BlockUserMap;		// key: userId, value: unused

public:
	LCBlockManager();
	virtual ~LCBlockManager();

public:
	// 更新黑名单列表
	void UpdateWithBlockList(const TalkUserList& userList);
	// 更新被屏蔽女士列表
	void UpdateWithBlockUsers(const list<string>& userList);
	// 用户是否存在于黑名单
	bool IsExist(const string& userId);

private:
	BlockUserMap	m_blockListMap;		// 黑名单列表
	IAutoLock*		m_blockListMapLock;	// 黑名单列表锁

	BlockUserMap	m_blockUsersMap;		// 被屏蔽女士列表
	IAutoLock*		m_blockUsersMapLock;	// 被屏蔽女士列表锁
};
