#include "HelpCmdHandle.h"

HelpCmdHandle::HelpCmdHandle(void)
{
}

HelpCmdHandle::~HelpCmdHandle(void)
{
}
bool HelpCmdHandle::Init()
{
	return true;
}
//��������cmd����
bool HelpCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
	return HelpHandle(cmdList,exit);
}
//help������
bool HelpCmdHandle::HelpHandle(list<string>& cmdList,bool &exit)
{
	bool isWait = false;
    
	// ��ӡ����֧�ֵ�����
	ExitInfo();
	InitInfo();
	UploadVerInfo();
	LoginInfo();
	LogoutInfo();
	SendTextInfo();
	SendLoopTextInfo();
	SendEmotionInfo();
	SendLoopEmotionInfo();
	SendMagicIconInfo();
	GetUserInfo();

	// ������������
	UploadThemeVerInfo();
	GetUserThemeInfo();
	GetAllThemeInfo();
	BuyThemeInfo();
	ApplyThemeInfo();
	SendAutoInviteInfo();
	GetAutoInviteStatusInfo();
	SendThemeRecommendInfo();
    exit = false;
	return isWait;
}
