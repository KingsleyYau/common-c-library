/*
 * author: Samson.Fan
 *   date: 2015-03-19
 *   file: ILiveChatManManager.cpp
 *   desc: LiveChat男士Manager接口类
 */

#include "ILiveChatManManager.h"
#include "LiveChatManManager.h"

ILiveChatManManager* ILiveChatManManager::Create()
{
	ILiveChatManManager* pManager = new LiveChatManManager;
	return pManager;
}

void ILiveChatManManager::Release(ILiveChatManManager* obj)
{
	delete obj;
}
