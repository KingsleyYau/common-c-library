/*
 *author:Alex shum
 *  date:2016-8-15
 *  file:GetUserThemeCmdHandle.h
 *  desc:处理获取男/女士主题包命令实现类
*/

#pragma once
#include "ICmdhandle.h"

class GetUserThemeCmdHandle :
	public ICmdHandle
{
public:
	GetUserThemeCmdHandle(void);
	virtual ~GetUserThemeCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool GetUserThemeHandle(list<string>& cmdList,bool &exit);//向服务器请求获取男/女士主题包
};
