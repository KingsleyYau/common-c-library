/*
*author:Alex shum
*  date:2016-8-15
*  file:LoopTextCmdHandle.h
*  desc:ѭ��������Ϣ���������looptextת��Ϊlooptextproc����,��loop�̰߳�looptextprocת�����������handlecmd��
*/
#pragma once
#include "ICmdhandle.h"

class LoopTextCmdHandle :
	public ICmdHandle
{
public:
	LoopTextCmdHandle(void);
	virtual ~LoopTextCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool SendLoopTextHandle(list<string>& cmdList,bool &exit);//������Ϣ
};
