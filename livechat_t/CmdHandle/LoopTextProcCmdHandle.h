/*
*author:Alex shum
*  date:2016-8-15
*  file:LooptextProcCmdHandle.h
*  desc:ѭ��������Ϣ��������ʵ����
*/


#pragma once
#include "ICmdhandle.h"

class LoopTextProcCmdHandle :
	public ICmdHandle
{
public:
	LoopTextProcCmdHandle(void);
	virtual ~LoopTextProcCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool SendLoopTextProc(list<string>& cmdList,bool &exit);//ѭ��������Ϣ
};
