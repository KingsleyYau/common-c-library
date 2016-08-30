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
//��������cmd����
bool LoopTextCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return SendLoopTextHandle(cmdList,exit);
}

//������Ϣ
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
				// ��������
				g_loopTextRun  = true;

				// ��ʼ������
				g_loopTextCounter.Init();

				// ���淢��ʱ����
				g_loopTextStepTime = sec;

				// ����ѭ����������
				InsertLoopTextProcCmd(userList);
			}
			else {
				printf("looptext is started.\n");
			}
		}
		else if (cmd == "end") {
			isHandle = true;
			if (g_loopTextRun) {
				// ֹͣ
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
