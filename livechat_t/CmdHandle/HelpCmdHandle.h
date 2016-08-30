/*
 *author: Alex shum
 *  date: 2016-8-12
 *  file: HelpCmdHandle
 *  desc: 处理help命令实现类，输出帮助命令行
 */
#pragma once
#include "ICmdHandle.h"

using namespace std;
class HelpCmdHandle :
	public ICmdHandle
{
public:
	HelpCmdHandle(void);
	virtual ~HelpCmdHandle(void);public:
	//ICmdHandle的接口
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	//help处理函数
	bool HelpHandle(list<string>& cmdList,bool &exit);
};
