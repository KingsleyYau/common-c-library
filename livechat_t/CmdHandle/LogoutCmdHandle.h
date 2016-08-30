/*
*author:Alex shum
*  date:2016-8-15
*  file:LogoutCmdhandle.h
*  desc:处理登出命令，断开连接
*/
#pragma once
#include "ICmdhandle.h"

class LogoutCmdHandle :
	public ICmdHandle
{
public:
	LogoutCmdHandle(void);
	virtual ~LogoutCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool LogoutHandle(list<string>& cmdList,bool &exit);//登出
};
