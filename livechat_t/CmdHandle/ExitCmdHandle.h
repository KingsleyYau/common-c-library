/*
 *aythor:Alex shum
 *  date:2016-8-12
 *  file:ExitCmdHandle
 *  desc:�����˳�����ʵ���࣬��ӡexit
*/
#pragma once
#include "ICmdHandle.h"

using namespace std;
class ExitCmdHandle : public ICmdHandle
{
public:
	ExitCmdHandle(void);
	virtual ~ExitCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
};
