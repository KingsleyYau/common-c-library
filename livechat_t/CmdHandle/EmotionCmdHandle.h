/*
*author:Alex shum
*  date:2016-8-15
*  file:EmotionCmdHandle.h
*  desc:����߼���Ϣ����ʵ����
*/
#pragma once
#include "ICmdhandle.h"

class EmotionCmdHandle :
	public ICmdHandle
{
public:
	EmotionCmdHandle(void);
	virtual ~EmotionCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool SendEmotionHandle(list<string>& cmdList,bool &exit);//���͸߼�������Ϣ����
};
