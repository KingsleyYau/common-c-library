/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCBlockManager.h
 *   desc: LiveChat黑名单管理器
 */

#include "LCBlockManager.h"
#include <common/CommonFunc.h>

LCBlockManager::LCBlockManager()
{
	m_blockListMapLock = IAutoLock::CreateAutoLock();
	if (NULL != m_blockListMapLock) {
		m_blockListMapLock->Init();
	}

	m_blockUsersMapLock = IAutoLock::CreateAutoLock();
	if (NULL != m_blockUsersMapLock) {
		m_blockUsersMapLock->Init();
	}
}

LCBlockManager::~LCBlockManager()
{
	IAutoLock::ReleaseAutoLock(m_blockListMapLock);
	IAutoLock::ReleaseAutoLock(m_blockUsersMapLock);
}

// 更新黑名单列表
void LCBlockManager::UpdateWithBlockList(const TalkUserList& userList)
{
	// 加锁
	if (NULL != m_blockListMapLock) {
		m_blockListMapLock->Lock();
	}

	// 清空列表
	m_blockListMap.clear();

	// 插入黑名单map表
	for (TalkUserList::const_iterator iter;
		iter != userList.end();
		iter++)
	{
		m_blockListMap.insert(BlockUserMap::value_type((*iter).userId), false);
	}

	// 解锁
	if (NULL != m_blockListMapLock) {
		m_blockListMapLock->Unlock();
	}
}

// 更新被屏蔽女士列表
void LCBlockManager::UpdateWithBlockUsers(const list<string>& userList)
{
	// 加锁
	if (NULL != m_blockUsersMapLock) {
		m_blockUsersMapLock->Lock();
	}

	// 清空列表
	m_blockUsersMap.clear();

	// 插入黑名单map表
	for (list<string>::const_iterator iter;
		iter != userList.end();
		iter++)
	{
		m_blockUsersMap.insert(BlockUserMap::value_type((*iter)), false);
	}

	// 解锁
	if (NULL != m_blockUsersMapLock) {
		m_blockUsersMapLock->Unlock();
	}
}

// 用户是否存在于黑名单
bool LCBlockManager::IsExist(const string& userId)
{
	bool result = false;

	// 加锁
	if (NULL != m_blockListMapLock) {
		m_blockListMapLock->Lock();
	}
	if (NULL != m_blockUsersMapLock) {
		m_blockUsersMapLock->Lock();
	}

	BlockUserMap::const_iterator iter;
	// 判断黑名单
	iter = m_blockListMap.find(userId);
	result = (iter != m_blockListMap.end());

	// 判断被屏蔽女士列表
	iter = m_blockUsersMap.find(userId);
	result = result || (iter != m_blockUsersMap.end());

	// 解锁
	if (NULL != m_blockUsersMapLock) {
		m_blockUsersMapLock->Unlock();
	}
	if (NULL != m_blockListMapLock) {
		m_blockListMapLock->Unlock();
	}

	return result;
}
