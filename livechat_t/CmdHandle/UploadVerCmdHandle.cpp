#include "UploadVerCmdHandle.h"

UploadVerCmdHandle::UploadVerCmdHandle(void)
{
}

UploadVerCmdHandle::~UploadVerCmdHandle(void)
{
}
bool UploadVerCmdHandle::Init()
{
	return true;
}

//公共处理cmd函数
bool UploadVerCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return UploadVerHandle(cmdList,exit);
}

//解析命令，向服务器请求传版本号
bool UploadVerCmdHandle::UploadVerHandle(list<string>& cmdList,bool &exit)
{
	bool wait = false;
	if (cmdList.size() > 1) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// userId
		string ver = (*iter);
		iter++;

		bool result = g_client->UploadVer(ver);
		if (!result) {
			printf("uploadver fail!\n");
		}
	}
	else {
		UploadVerInfo();
	}
	return wait;
}