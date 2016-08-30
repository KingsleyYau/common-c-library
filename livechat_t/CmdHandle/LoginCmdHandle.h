/*
 *author:Alex shum
 *  date:2016-8-12
 *  file:LoginCmdHandle.h
 *  desc:处理登录命令，主要得到账号密码，给客服端类调用
*/
#pragma once
#include "ICmdHandle.h"

using namespace std;

class LoginCmdHandle :
	public ICmdHandle
{
public:
	LoginCmdHandle(void);
	virtual ~LoginCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool LoginHandle(list<string>& cmdList,bool &exit);
};
