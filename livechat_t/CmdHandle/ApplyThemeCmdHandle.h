/*
 *author:Alex shum
 *  date:2016-8-15
 *  file:ApplyThemeCmdHandle.h
 *  desc:处理应用主题包命令实现类
*/

#pragma once
#include "ICmdhandle.h"

class ApplyThemeCmdHandle :
	public ICmdHandle
{
public:
	ApplyThemeCmdHandle(void);
	virtual ~ApplyThemeCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数，里面调用
private:
	bool ApplyThemeHandle(list<string>& cmdList,bool &exit);//解析命令，想服务器请求应用主题包
};
