#include "LoginCmdHandle.h"
#include <windows.h>

LoginCmdHandle::LoginCmdHandle(void)
{
}

LoginCmdHandle::~LoginCmdHandle(void)
{
}

bool LoginCmdHandle::Init()
{
	return true;
}
//��������cmd����
bool LoginCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
	return LoginHandle(cmdList,exit);
	
}

//��¼
bool LoginCmdHandle::LoginHandle(list<string>& cmdList,bool &exit)
{
	//��¼������Ҫ�ȴ�
	bool isWait = true;
	if(cmdList.size() >3)
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;
	    
				// account
		string account = (*iter);
		iter++;

		// password
		string password = (*iter);
		iter++;

		// sex
		USER_SEX_TYPE sex = (USER_SEX_TYPE)atoi((*iter).c_str());
		iter++;

		// authType
		AUTH_TYPE authType = AUTH_TYPE_PWD;
		if (iter != cmdList.end()) {
			authType = (AUTH_TYPE)atoi((*iter).c_str());
			iter++;
		}

		// deviceId
		string deviceId = "000000000";
		if (iter != cmdList.end()) {
			deviceId = (*iter);
			iter++;
		}

		bool result = g_client->Login(account, password, deviceId, CLIENT_ANDROID, sex, authType);
		if (!result) {
			printf("Login fail!\n");
		}
		else {
			// set console title
			SetConsoleTitle(account.c_str());
		}

	}
	else
	{
		LoginInfo();
	}
	return isWait;
}
