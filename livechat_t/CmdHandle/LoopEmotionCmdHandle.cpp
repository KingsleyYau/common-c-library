#include "LoopEmotionCmdHandle.h"

LoopEmotionCmdHandle::LoopEmotionCmdHandle(void)
{
}

LoopEmotionCmdHandle::~LoopEmotionCmdHandle(void)
{
}

bool LoopEmotionCmdHandle::Init()
{
	return true;
}
//公共处理cmd函数
bool LoopEmotionCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return SendLoopEmotionHandle(cmdList,exit);
}

//处理循环高级表情命令，但这函数没有发送
bool LoopEmotionCmdHandle::SendLoopEmotionHandle(list<string>& cmdList,bool &exit)
{
	bool wait = false;
	if (cmdList.size() >= 2) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// cmd
		string cmd = *iter;
		iter++;

		// sec
		int sec = 5;
		if (iter != cmdList.end())
		{
			string strSec = *iter;
			int temp = atoi(strSec.c_str());
			if (temp > 0) {
				sec = temp;
				iter++;
			}
		}

		// userList
		list<string> userList;
		while (iter != cmdList.end())
		{
			userList.push_back(*iter);
			iter++;
		}

		bool isHandle = false;
		if (cmd == "start")
		{
			isHandle = true;
			if (!g_loopEmotionRun) 
			{
				// 立即启动
				g_loopEmotionRun  = true;

				// 初始化参数
				g_loopEmotionCounter.Init();

				// 保存发送时间间隔
				g_loopEmotionStepTime = sec;

				// 插入循环命令处理队列
				InsertLoopEmotionProcCmd(userList);
			}
			else {
				printf("loopemotion is started.\n");
			}
		}
		else if (cmd == "end") {
			isHandle = true;
			if (g_loopEmotionRun) {
				// 停止
				g_loopCmdList.lock();
				for (list_lock<LoopCmdItem>::iterator itemIter = g_loopCmdList.begin();
					itemIter != g_loopCmdList.end();
					itemIter++)
				{
					if (NULL != strstr((*itemIter).cmd.c_str(), "loopemotionproc"))
					{
						g_loopCmdList.erase(itemIter);
						break;
					}
				}
				g_loopCmdList.unlock();

				g_loopEmotionRun = false;
			}
			else {
				printf("loopemotion is not start.\n");
			}
		}

		if (!isHandle) {
			SendLoopEmotionInfo();
		}
	}
	else {
		SendLoopEmotionInfo();
	}
	return wait;
}
