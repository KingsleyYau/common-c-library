#include "UploadThemeVerCmdHandle.h"

UploadThemeVerCmdHandle::UploadThemeVerCmdHandle(void)
{
}

UploadThemeVerCmdHandle::~UploadThemeVerCmdHandle(void)
{
}
bool UploadThemeVerCmdHandle::Init()
{
	return true;
}
//��������cmd����
bool UploadThemeVerCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return UploadThemeVerHandle(cmdList,exit);
}

//�������������������ϴ�������б�汾��
bool UploadThemeVerCmdHandle::UploadThemeVerHandle(list<string>& cmdList,bool &exit)
{
	bool wait = false;
	if (cmdList.size() > 1) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// themeVersion
		string ver = (*iter);
		iter++;

		int iVer = atoi(ver.c_str());
		bool result = g_client->UploadThemeListVer(iVer);
		if (!result) {
			printf("uploadthemever fail!\n");
		}
	}
	else {
		UploadThemeVerInfo();
	}
	return wait;
}
