/*
 *author:Alex shum
 *  date:2016-8-15
 *  file:BuyThemeCmdHandle.h
 *  desc:���������������
*/
#pragma once
#include "ICmdhandle.h"

class BuyThemeCmdHandle :
	public ICmdHandle
{
public:
	BuyThemeCmdHandle(void);
	virtual ~BuyThemeCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool BuyThemeHandle(list<string>& cmdList,bool &exit);//���������������������������
};
