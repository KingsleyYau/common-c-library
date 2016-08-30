/*
*author:Alex shum
*  date:2016-8-15
*  file:PlayThemeCmdHandle.h
*  desc:处理播放主题包动画命令 实现类
*/

#pragma once
#include "ICmdhandle.h"

class PlayThemeCmdHandle :
	public ICmdHandle
{
public:
	PlayThemeCmdHandle(void);
	virtual ~PlayThemeCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool PlayThemeHandle(list<string>& cmdList,bool &exit);//解析命令，向服务器请求播放主题包动画

};
