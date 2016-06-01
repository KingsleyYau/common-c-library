/*
 * author: Samson.Fan
 *   date: 2015-03-19
 *   file: ILiveChatClient.h
 *   desc: LiveChat客户端接口类
 */

#include "ILiveChatClient.h"
#include "LiveChatClient.h"
#include <common/CheckMemoryLeak.h>

// 生成LiveChat客户端实现类
ILiveChatClient* ILiveChatClient::CreateClient()
{
	ILiveChatClient* client = new CLiveChatClient();
	return client;
}

// 释放LiveChat客户端实现类
void ILiveChatClient::ReleaseClient(ILiveChatClient* client) 
{
	delete client;
}
