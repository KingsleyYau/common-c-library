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
//��������cmd����
bool LoopEmotionProcCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return SendLoopEmotionProc(cmdList,exit);
}

//ѭ�����͸߼����鴦��
bool LoopEmotionProcCmdHandle::SendLoopEmotionProc(list<string>& cmdList,bool &exit)
{
	bool wait = false;
	if (cmdList.size() >= 2) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// ������Ϣ
		char message[32] = {0};
		_itoa_s(g_loopEmotionCounter.GetAndIncrement(), message, sizeof(message), 10);

		// userList
		list<string> userList;
		while (iter != cmdList.end())
		{
			// ������Ϣ
			int ticket = g_msgCounter.GetAndIncrement();
			// ������Ϣ�����߼�����
			g_client->SendEmotion(*iter, "W03", ticket);

			userList.push_back(*iter);
			iter++;
		}

		// ����ѭ����������,�����ֻ�ص�������ĺ������γ�ѭ��
		InsertLoopEmotionProcCmd(userList);
	}
	return wait;
}

