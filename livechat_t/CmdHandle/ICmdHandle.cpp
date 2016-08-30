#include "ICmdHandle.h"
#include "ExitCmdHandle.h"
#include "HelpCmdHandle.h"
#include "InitCmdHandle.h"
#include "LoginCmdHandle.h"
#include "LogoutCmdHandle.h"
#include "TextCmdHandle.h"
#include "LoopTextCmdHandle.h"
#include "LoopTextProcCmdHandle.h"
#include "EmotionCmdHandle.h"
#include "LoopEmotionCmdHandle.h"
#include "LoopEmotionProcCmdHandle.h"
#include "MagiciconCmdHandle.h"
#include "UserInfoCmdHandle.h"
#include "UploadVerCmdHandle.h"
#include "UploadThemeVerCmdHandle.h"
#include "GetUserThemeCmdHandle.h"
#include "GetAllThemeCmdHandle.h"
#include "BuyThemeCmdHandle.h"
#include "ApplyThemeCmdHandle.h"
#include "PlayThemeCmdHandle.h"
#include "GetTalkListCmdHandle.h"
#include "GetAutoInviteStatusCmdHandle.h"
#include "AutoInviteCmdHandle.h"
#include "ThemeRecommendCmdHandle.h"

ICmdHandle* ICmdHandle::CreateCmdHandleWithCmd(string cmd)
{
	ICmdHandle* cmdHandle = NULL;
	if(0 == cmd.compare("help"))
	{
		cmdHandle = new HelpCmdHandle();
	}
	else if(0 == cmd.compare("exit"))
	{
		cmdHandle =  new ExitCmdHandle();
	}
	else if(0 == cmd.compare("init"))
	{
		cmdHandle =  new InitCmdHandle();
	}
	else if(0 == cmd.compare("login"))
	{
		cmdHandle =  new LoginCmdHandle();
	}
	else if(0 == cmd.compare("logout"))
	{
		cmdHandle =  new LogoutCmdHandle();
	}
	else if(0 == cmd.compare("text"))
	{
		cmdHandle =  new TextCmdHandle();
	}
	else if(0 == cmd.compare("looptext"))
	{
		cmdHandle =  new LoopTextCmdHandle();
	}
	else if(0 == cmd.compare("looptextproc"))
	{
		cmdHandle =  new LoopTextProcCmdHandle();
	}
	else if(0 == cmd.compare("emotion"))
	{
		cmdHandle =  new EmotionCmdHandle();
	}
	else if(0 == cmd.compare("loopemotion"))
	{
		cmdHandle =  new LoopEmotionCmdHandle();
	}
	else if(0 == cmd.compare("loopemotionproc"))
	{
		cmdHandle =  new LoopEmotionProcCmdHandle();
	}
	else if(0 == cmd.compare("magicicon"))
	{
		cmdHandle =  new MagiciconCmdHandle();
	}
	else if(0 == cmd.compare("userinfo"))
	{
		cmdHandle =  new UserInfoCmdHandle();
	}
	else if(0 == cmd.compare("uploadver"))
	{
		cmdHandle =  new UploadVerCmdHandle();
	}
	else if(0 == cmd.compare("uploadthemever"))
	{
		cmdHandle =  new UploadThemeVerCmdHandle();
	}
	else if(0 == cmd.compare("getusertheme"))
	{
		cmdHandle =  new GetUserThemeCmdHandle();
	}
	else if(0 == cmd.compare("getalltheme"))
	{
		cmdHandle =  new GetAllThemeCmdHandle();
	}
	else if(0 == cmd.compare("buytheme"))
	{
		cmdHandle =  new BuyThemeCmdHandle();
	}
	else if(0 == cmd.compare("applytheme"))
	{
		cmdHandle =  new ApplyThemeCmdHandle();
	}
	else if(0 == cmd.compare("playtheme"))
	{
		cmdHandle =  new PlayThemeCmdHandle();
	}
	else if(0 == cmd.compare("gettalklist"))
	{
		cmdHandle =  new GetTalkListCmdHandle();
	}
	else if(0 == cmd.compare("getautoinvitestatus"))
	{
		cmdHandle =  new GetAutoInviteStatusCmdHandle();
	}
	else if(0 == cmd.compare("autoinvite"))
	{
		cmdHandle =  new AutoInviteCmdHandle();
	}
	else if(0 == cmd.compare("themerecommend"))
	{
		cmdHandle =  new ThemeRecommendCmdHandle();
	}
	else
	{
		printf("Unknow command, 'help' print all command!\n");
	}
	return cmdHandle;
}

