#include "LoopTextProcCmdHandle.h"

LoopTextProcCmdHandle::LoopTextProcCmdHandle(void)
{
}

LoopTextProcCmdHandle::~LoopTextProcCmdHandle(void)
{
}
bool LoopTextProcCmdHandle::Init()
{
	return true;
}
//公共处理cmd函数
bool LoopTextProcCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return SendLoopTextProc(cmdList,exit);
}

//循环发送消息
bool LoopTextProcCmdHandle::SendLoopTextProc(list<string>& cmdList,bool &exit)
{
	bool isWait = false;
	if (cmdList.size() >= 2) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// 生成消息
		char message[32] = {0};
		_itoa_s(g_loopTextCounter.GetAndIncrement(), message, sizeof(message), 10);

		// userList
		list<string> userList;
		while (iter != cmdList.end())
		{
			// 发送消息
			int ticket = g_msgCounter.GetAndIncrement();
			g_client->SendTextMessage(*iter, message, false, ticket);

			userList.push_back(*iter);
			iter++;
		}

		InsertLoopTextProcCmd(userList);
	}

	return isWait;
}

