/*
*author:Alex shum
*  date:2016-8-15
*  file:LoopEmotionCmdHandle.h
*  desc:ѭ�����͸߼�������Ϣ���������loopemotion��Ϊloopemotionproc����,��loop�̰߳�loopemotionprocת�����������handlecmd��
*/
#pragma once
#include "ICmdhandle.h"

class LoopEmotionCmdHandle :
	public ICmdHandle
{
public:
	LoopEmotionCmdHandle(void);
	virtual ~LoopEmotionCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool SendLoopEmotionHandle(list<string>& cmdList,bool &exit);//����ѭ���߼�����������⺯��û�з���
	//void InsertLoopEmotionProcCmd(const list<string>& userList);

};
