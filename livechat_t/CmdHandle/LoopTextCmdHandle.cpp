#include "LoopTextCmdHandle.h"

LoopTextCmdHandle::LoopTextCmdHandle(void)
{
}

LoopTextCmdHandle::~LoopTextCmdHandle(void)
{
}

bool LoopTextCmdHandle::Init()
{
	return true;
}
//公共处理cmd函数
bool LoopTextCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return SendLoopTextHandle(cmdList,exit);
}

//发送消息
bool LoopTextCmdHandle::SendLoopTextHandle(list<string>& cmdList,bool &exit)
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
			if (!g_loopTextRun) 
			{
				// 立即启动
				g_loopTextRun  = true;

				// 初始化参数
				g_loopTextCounter.Init();

				// 保存发送时间间隔
				g_loopTextStepTime = sec;

				// 插入循环命令处理队列
				InsertLoopTextProcCmd(userList);
			}
			else {
				printf("looptext is started.\n");
			}
		}
		else if (cmd == "end") {
			isHandle = true;
			if (g_loopTextRun) {
				// 停止
				g_loopCmdList.lock();
				for (list_lock<LoopCmdItem>::iterator itemIter = g_loopCmdList.begin();
					itemIter != g_loopCmdList.end();
					itemIter++)
				{
					if (NULL != strstr((*itemIter).cmd.c_str(), "looptextproc"))
					{
						g_loopCmdList.erase(itemIter);
						break;
					}
				}
				g_loopCmdList.unlock();

				g_loopTextRun = false;
			}
			else {
				printf("looptext is not start.\n");
			}
		}

		if (!isHandle) {
			SendLoopTextInfo();
		}
	}
	else {
		SendLoopTextInfo();
	}
	return wait;
}
