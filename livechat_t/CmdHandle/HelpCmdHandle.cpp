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
//公共处理cmd函数
bool HelpCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
	return HelpHandle(cmdList,exit);
}
//help处理函数
bool HelpCmdHandle::HelpHandle(list<string>& cmdList,bool &exit)
{
	bool isWait = false;
    
	// 打印所有支持的命令
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

	// 主题包相关命令
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
