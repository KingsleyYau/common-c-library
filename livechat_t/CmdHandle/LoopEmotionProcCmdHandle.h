/*
*author:Alex shum
*  date:2016-8-15
*  file:LoopEmotionProcCmdHandle.h
*  desc:ѭ�����͸߼�������Ϣ��������ʵ����
*/

#pragma once
#include "ICmdhandle.h"

class LoopEmotionProcCmdHandle :
	public ICmdHandle
{
public:
	LoopEmotionProcCmdHandle(void);
	virtual ~LoopEmotionProcCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool SendLoopEmotionProc(list<string>& cmdList,bool &exit);//ѭ�����͸߼����鴦��

};
