/*
*author:Alex shum
*  date:2016-8-15
*  file:UploadThemeVerCmdHandle.h
*  desc:处理上传主题包列表版本号命令 实现类
*/

#pragma once
#include "ICmdhandle.h"

class UploadThemeVerCmdHandle :
	public ICmdHandle
{
public:
	UploadThemeVerCmdHandle(void);
	virtual ~UploadThemeVerCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool UploadThemeVerHandle(list<string>& cmdList,bool &exit);//解析命令，向服务器请求上传主题包列表版本号
};
