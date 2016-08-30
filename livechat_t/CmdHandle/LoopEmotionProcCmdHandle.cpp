#include "LoopEmotionProcCmdHandle.h"

LoopEmotionProcCmdHandle::LoopEmotionProcCmdHandle(void)
{
}

LoopEmotionProcCmdHandle::~LoopEmotionProcCmdHandle(void)
{
}

bool LoopEmotionProcCmdHandle::Init()
{
	return true;
}
//公共处理cmd函数
bool LoopEmotionProcCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return SendLoopEmotionProc(cmdList,exit);
}

//循环发送高级表情处理
bool LoopEmotionProcCmdHandle::SendLoopEmotionProc(list<string>& cmdList,bool &exit)
{
	bool wait = false;
	if (cmdList.size() >= 2) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// 生成消息
		char message[32] = {0};
		_itoa_s(g_loopEmotionCounter.GetAndIncrement(), message, sizeof(message), 10);

		// userList
		list<string> userList;
		while (iter != cmdList.end())
		{
			// 发送消息
			int ticket = g_msgCounter.GetAndIncrement();
			// 发送消息单条高级表情
			g_client->SendEmotion(*iter, "W03", ticket);

			userList.push_back(*iter);
			iter++;
		}

		// 插入循环命令处理队列,里面又会回调到这里的函数，形成循环
		InsertLoopEmotionProcCmd(userList);
	}
	return wait;
}

