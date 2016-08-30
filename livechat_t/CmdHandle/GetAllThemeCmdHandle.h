/*
 *author:Alex shum
 *  date:2016-8-15
 *  file:GetAllThemeCmdHandle.h
 *  desc:处理获取所有主题命令实现类
*/

#pragma once
#include "ICmdhandle.h"

class GetAllThemeCmdHandle :
	public ICmdHandle
{
public:
	GetAllThemeCmdHandle(void);
	virtual ~GetAllThemeCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool GetAllThemeHandle(list<string>& cmdList,bool &exit);//向服务器获取所有主题包
};
