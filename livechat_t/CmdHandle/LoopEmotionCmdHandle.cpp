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
//��������cmd����
bool LoopEmotionCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return SendLoopEmotionHandle(cmdList,exit);
}

//����ѭ���߼�����������⺯��û�з���
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
				// ��������
				g_loopEmotionRun  = true;

				// ��ʼ������
				g_loopEmotionCounter.Init();

				// ���淢��ʱ����
				g_loopEmotionStepTime = sec;

				// ����ѭ����������
				InsertLoopEmotionProcCmd(userList);
			}
			else {
				printf("loopemotion is started.\n");
			}
		}
		else if (cmd == "end") {
			isHandle = true;
			if (g_loopEmotionRun) {
				// ֹͣ
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
