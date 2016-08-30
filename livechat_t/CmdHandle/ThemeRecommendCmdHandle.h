/*
 *author:Alex shum
 *  date:2016-8-24
 *  file:ThemeRecommendCmdHandle.h
 *  desc:处理女士推荐男士购买主题包命令实现类
*/

#pragma once
#include "ICmdhandle.h"

class ThemeRecommendCmdHandle :
	public ICmdHandle
{
public:
	ThemeRecommendCmdHandle(void);
	virtual ~ThemeRecommendCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool ThemeRecommendHandle(list<string>& cmdList,bool &exit);//解析命令，向服务器请求女士推荐男士购买主题包
};
