#include "TextCmdHandle.h"

TextCmdHandle::TextCmdHandle(void)
{
}

TextCmdHandle::~TextCmdHandle(void)
{
}

//公共处理cmd函数
bool TextCmdHandle::Init()
{
	return true;
}

//发送消息
bool TextCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return SendTextHandle(cmdList,exit);
}
bool TextCmdHandle::SendTextHandle(list<string>& cmdList,bool &exit)
{
	bool wait = true;
		if (cmdList.size() > 2) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// userId
		string userId = (*iter);
		iter++;

		// message
		string message = (*iter);
		iter++;

		int ticket = g_msgCounter.GetAndIncrement();
		bool result = g_client->SendTextMessage(userId, message, false, ticket);
		if (!result) {
			printf("text fail!\n");
		}
	}
	else {
		SendTextInfo();
		wait = false;
	}
	return wait;
}