/*
*author:Alex shum
*  date:2016-8-15
*  file:TextCmdHandle.h
*  desc:��������Ϣ����
*/
#pragma once
#include "ICmdhandle.h"

class TextCmdHandle :
	public ICmdHandle
{
public:
	TextCmdHandle(void);
	virtual ~TextCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool SendTextHandle(list<string>& cmdList,bool &exit);//������Ϣ����
};
