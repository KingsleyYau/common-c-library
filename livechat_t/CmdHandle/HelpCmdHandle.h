/*
 *author: Alex shum
 *  date: 2016-8-12
 *  file: HelpCmdHandle
 *  desc: ����help����ʵ���࣬�������������
 */
#pragma once
#include "ICmdHandle.h"

using namespace std;
class HelpCmdHandle :
	public ICmdHandle
{
public:
	HelpCmdHandle(void);
	virtual ~HelpCmdHandle(void);public:
	//ICmdHandle�Ľӿ�
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	//help������
	bool HelpHandle(list<string>& cmdList,bool &exit);
};
