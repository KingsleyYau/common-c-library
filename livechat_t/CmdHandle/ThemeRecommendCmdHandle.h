/*
 *author:Alex shum
 *  date:2016-8-24
 *  file:ThemeRecommendCmdHandle.h
 *  desc:����Ůʿ�Ƽ���ʿ�������������ʵ����
*/

#pragma once
#include "ICmdhandle.h"

class ThemeRecommendCmdHandle :
	public ICmdHandle
{
public:
	ThemeRecommendCmdHandle(void);
	virtual ~ThemeRecommendCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool ThemeRecommendHandle(list<string>& cmdList,bool &exit);//������������������Ůʿ�Ƽ���ʿ���������
};
