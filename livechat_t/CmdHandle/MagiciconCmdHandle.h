/*
*author:Alex shum
*  date:2016-8-15
*  file:MagiciconCmdHandle.h
*  desc:����С��������ʵ����
*/

#pragma once
#include "ICmdhandle.h"

class MagiciconCmdHandle :
	public ICmdHandle
{
public:
	MagiciconCmdHandle(void);
	virtual ~MagiciconCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool SendMagicIconHandle(list<string>& cmdList,bool &exit);//�����������С����
};
