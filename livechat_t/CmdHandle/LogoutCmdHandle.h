/*
*author:Alex shum
*  date:2016-8-15
*  file:LogoutCmdhandle.h
*  desc:����ǳ�����Ͽ�����
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
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool LogoutHandle(list<string>& cmdList,bool &exit);//�ǳ�
};
