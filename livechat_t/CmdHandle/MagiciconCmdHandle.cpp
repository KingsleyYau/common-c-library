#include "MagiciconCmdHandle.h"

MagiciconCmdHandle::MagiciconCmdHandle(void)
{
}

MagiciconCmdHandle::~MagiciconCmdHandle(void)
{
}
bool MagiciconCmdHandle::Init()
{
	return true;
}
//��������cmd����
bool MagiciconCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return SendMagicIconHandle(cmdList,exit);
}

//�����������С����
bool MagiciconCmdHandle::SendMagicIconHandle(list<string>& cmdList,bool &exit)
{
	bool isWait = true;

	if (cmdList.size() > 2) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// userId
		string userId = (*iter);
		iter++;

		// iconId
		string iconId = (*iter);
		iter++;

		int ticket = g_msgCounter.GetAndIncrement();
		bool result = g_client->SendMagicIcon(userId, iconId, ticket);
		if (!result) {
			printf("magicicon fail!\n");
		}
	}
	else {
		SendMagicIconInfo();
		isWait = false;
	}

	return isWait;
}
