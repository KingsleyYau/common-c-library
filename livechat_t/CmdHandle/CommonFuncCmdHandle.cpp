#include "CommonFuncCmdHandle.h"
using namespace std;


// 循环文本消息发送间隔时间
extern int g_loopTextStepTime;
// 循环命令队列
extern list_lock<LoopCmdItem>	g_loopCmdList;
// 循环高表消息发送间隔时间
extern int g_loopEmotionStepTime;

// 打印所有支持的命令
void ExitInfo()
{
	printf("exit\n");
}
void InitInfo()
{
	printf("init ip1,ip2,...ipn port\n");
}
void UploadVerInfo()
{
	printf("uploadver version\n");
}
void LoginInfo()
{
	printf("login account password sex[female:0/male:1] [authtype: sid:0/pwd:1] [deviceid]\n");
}
void LogoutInfo()
{
	printf("logout\n");
}
void SendTextInfo()
{
	printf("text userId message\n");
}
void SendLoopTextInfo()
{
	printf("looptext start [second:5] userId\n");
	printf("looptext end userId\n");
}
void SendEmotionInfo()
{
	printf("emotion userId message\n");
}
void SendLoopEmotionInfo()
{
	printf("loopemotion start [second:5] userId\n");
	printf("loopemotion end userId\n");
}
void SendMagicIconInfo()
{
	printf("magicicon userId iconId\n");
}
void GetUserInfo()
{
	printf("userinfo userId\n");
}
	// 主题包相关命令
void UploadThemeVerInfo()
{
	printf("uploadthemever version\n");
}
void GetUserThemeInfo()
{
	printf("getusertheme userId\n");
}
void GetAllThemeInfo()
{
	printf("getalltheme userId");
}
void BuyThemeInfo()
{
	printf("buyteme userId themeId\n");
}
void ApplyThemeInfo()
{
	printf("applytheme userId themeId\n");
}

void PlayThemeInfo()
{
	printf("playtheme userId themeId\n");
}

void GetTalkListInfo()
{
	printf("gettalklist\n");
}

//打印自动邀请状态
void GetAutoInviteStatusInfo()
{
	printf("getautoinvitestatus\n");
}

//打印自动邀请消息
void SendAutoInviteInfo()
{
	printf("autoinvite isOpen[close:0/Open:1]\n");
}

//打印女士推荐男士购买主题包
void SendThemeRecommendInfo()
{
	printf("themerecommend userId themeId\n\n");
}

void InsertLoopTextProcCmd(const list<string>& userList)
{
	LoopCmdItem item;

	// 生成命令
	string userIds("");
	for (list<string>::const_iterator userIter = userList.begin();
		userIter != userList.end();
		userIter++)
	{
		if (!userIds.empty()) {
			userIds += " ";
		}
		userIds += *userIter;
	}
	item.cmd = "looptextproc";
	item.cmd += " ";
	item.cmd += userIds;

	// 生成时间
	item.handleTime = getCurrentTime() + (g_loopTextStepTime * 1000);

	// 插入循环命令处理队列
	g_loopCmdList.lock();
	g_loopCmdList.push_back(item);
	g_loopCmdList.unlock();
}

void InsertLoopEmotionProcCmd(const list<string>& userList)
{
	LoopCmdItem item;

	// 生成命令
	string userIds("");
	for (list<string>::const_iterator userIter = userList.begin();
		userIter != userList.end();
		userIter++)
	{
		if (!userIds.empty()) {
			userIds += " ";
		}
		userIds += *userIter;
	}
	item.cmd = "loopemotionproc";
	item.cmd += " ";
	item.cmd += userIds;

	// 生成时间
	item.handleTime = getCurrentTime() + (g_loopEmotionStepTime * 1000);

	// 插入循环命令处理队列
	g_loopCmdList.lock();
	g_loopCmdList.push_back(item);
	g_loopCmdList.unlock();
}