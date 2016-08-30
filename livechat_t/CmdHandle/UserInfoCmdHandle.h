/*
*author:Alex shum
*  date:2016-8-15
*  file:UserInfoCmdHandle.h
*  desc:处理获取男/女士主题包命令 实现类
*/


#pragma once
#include "ICmdhandle.h"

class UserInfoCmdHandle :
	public ICmdHandle
{
public:
	UserInfoCmdHandle(void);
	virtual ~UserInfoCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool GetUserInfoHandle(list<string>& cmdList,bool &exit);//解析命令，向服务器请求获取男/女士主题包

};
