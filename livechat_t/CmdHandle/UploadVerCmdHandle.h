/*
*author:Alex shum
*  date:2016-8-15
*  file:UploadVerCmdHandle.h
*  desc:处理上传版本号命令 实现类
*/

#pragma once
#include "ICmdhandle.h"

class UploadVerCmdHandle :
	public ICmdHandle
{
public:
	UploadVerCmdHandle(void);
	virtual ~UploadVerCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool UploadVerHandle(list<string>& cmdList,bool &exit);//解析命令，向服务器请求传版本号
};
