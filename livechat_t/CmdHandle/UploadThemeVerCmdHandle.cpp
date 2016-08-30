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
//公共处理cmd函数
bool UploadThemeVerCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return UploadThemeVerHandle(cmdList,exit);
}

//解析命令，向服务器请求上传主题包列表版本号
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
