/*
 *author:Alex shum
 *  date:2016-8-15
 *  file:BuyThemeCmdHandle.h
 *  desc:处理购买主题包命令
*/
#pragma once
#include "ICmdhandle.h"

class BuyThemeCmdHandle :
	public ICmdHandle
{
public:
	BuyThemeCmdHandle(void);
	virtual ~BuyThemeCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool BuyThemeHandle(list<string>& cmdList,bool &exit);//解析命令，向想服务器请求购买主题包
};
