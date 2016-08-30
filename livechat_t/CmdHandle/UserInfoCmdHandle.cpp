#include "UserInfoCmdHandle.h"

UserInfoCmdHandle::UserInfoCmdHandle(void)
{
}

UserInfoCmdHandle::~UserInfoCmdHandle(void)
{
}
bool UserInfoCmdHandle::Init()
{
	return true;
}

//��������cmd����
bool UserInfoCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return GetUserInfoHandle(cmdList,exit);
}

//�������������������ȡ��/Ůʿ�����
bool UserInfoCmdHandle::GetUserInfoHandle(list<string>& cmdList,bool &exit)
{
	bool isWait = true;
	if (cmdList.size() > 1) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// userId
		string userId = (*iter);
		iter++;

		bool result = g_client->GetUserInfo(userId);
		if (!result) {
			printf("userinfo fail!\n");
		}
	}
	else {
		GetUserInfo();
		isWait = false;
	}
	return isWait;
}