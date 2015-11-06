/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCContactManager.h
 *   desc: LiveChat联系人管理器
 */

#include "LCContactManager.h"
#include <common/CommonFunc.h>

LCContactManager::LCContactManager()
{
	m_contactListMapLock = IAutoLock::CreateAutoLock();
	if (NULL != m_contactListMapLock) {
		m_contactListMapLock->Init();
	}
}

LCContactManager::~LCContactManager()
{
	IAutoLock::ReleaseAutoLock(m_contactListMapLock);
}

// 更新联系人列表
void LCContactManager::UpdateWithContactList(const TalkUserList& userList)
{
	// 加锁
	if (NULL != m_contactListMapLock) {
		m_contactListMapLock->Lock();
	}

	// 清空列表
	m_contactListMap.clear();

	// 插入联系人map表
	for (TalkUserList::const_iterator iter;
		iter != userList.end();
		iter++)
	{
		m_contactListMap.insert(ContactUserMap::value_type((*iter).userId), false);
	}

	// 解锁
	if (NULL != m_contactListMapLock) {
		m_contactListMapLock->Unlock();
	}
}

// 用户是否存在于联系人列表
bool LCContactManager::IsExist(const string& userId)
{
	bool result = false;

	// 加锁
	if (NULL != m_contactListMapLock) {
		m_contactListMapLock->Lock();
	}

	ContactUserMap::const_iterator iter;
	// 判断联系人
	iter = m_contactListMap.find(userId);
	result = (iter != m_blockListMap.end());

	// 解锁
	if (NULL != m_contactListMapLock) {
		m_contactListMapLock->Unlock();
	}

	return result;
}
