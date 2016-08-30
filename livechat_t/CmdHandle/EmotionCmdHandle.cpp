#include "EmotionCmdHandle.h"

EmotionCmdHandle::EmotionCmdHandle(void)
{
}

EmotionCmdHandle::~EmotionCmdHandle(void)
{
}

bool EmotionCmdHandle::Init()
{
	return true;
}
//��������cmd����
bool EmotionCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return SendEmotionHandle(cmdList,exit);
}

//���͸߼�������Ϣ����
bool EmotionCmdHandle::SendEmotionHandle(list<string>& cmdList,bool &exit)
{
	bool isWait = true;
	if (cmdList.size() > 2) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// userId
		string userId = (*iter);
		iter++;

		// message
		string emotionId = (*iter);
		iter++;

		int ticket = g_msgCounter.GetAndIncrement();
		bool result = g_client->SendEmotion(userId, emotionId, ticket);
		if (!result) {
			printf("text fail!\n");
		}
	}
	else {
		SendEmotionInfo();
		isWait = false;
	}

	return isWait;
}