// testCURL.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <livechat/ILiveChatClient.h>
#include <common/StringHandle.h>
#include <livechat/ISocketHandler.h>
#include <livechat/Counter.h>
#include <string>
#include <livechat/IThreadHandler.h>
#include <common/list_lock.h>
#include <common/CommonFunc.h>

using namespace std;

// LiveChat�ͻ���
ILiveChatClient* g_client = NULL;
// LiveChat��Ϣ������
Counter g_msgCounter;
// �ȴ��ص���event
HANDLE g_waitRespondEvent = NULL;

// ���������
bool HandleCmd(const char* command, bool& isWait);

// ��ȡ�����item��ӡ���ַ���
string GetThemeInfoItemPrintStr(const ThemeInfoItem& item)
{
	string str("");
	char buffer[32] = {0};

	// themeId
	str += "themeId:";
	str += item.themeId;
	str += ", ";

	// manId
	str += "manId:";
	str += item.manId;
	str += ", ";

	// womanId
	str += "womanId:";
	str += item.womanId;
	str += ", ";

	// startTime
	str += "startTime:";
	snprintf(buffer, sizeof(buffer), "%d", item.startTime);
	str += buffer;
	str += ", ";

	// endTime
	str += "endTime:";
	snprintf(buffer, sizeof(buffer), "%d", item.endTime);
	str += buffer;
	str += ", ";

	// nowTime
	str += "nowTime:";
	snprintf(buffer, sizeof(buffer), "%d", item.nowTime);
	str += buffer;
	str += ", ";

	// updateTime
	str += "updateTime:";
	snprintf(buffer, sizeof(buffer), "%d", item.updateTime);
	str += buffer;
	str += "\n";

	return str;
}

// ��ȡTalkUserList��TalkSessionList��ӡ���ַ���
string GetTaskUserString(const TalkUserList& userList, const TalkSessionList& sessionList)
{
	string str("");
	char buffer[32] = {0};

	TalkUserList::const_iterator userIter = userList.begin();
	TalkSessionList::const_iterator sessionIter = sessionList.begin();
	for (; userIter != userList.end() && sessionIter != sessionList.end(); userIter++, sessionIter++)
	{
		str += (*userIter).userId;
		str += "(";
		str += (*sessionIter).invitedId;
		str += ")";
		str += "-";
		if ((*sessionIter).charget) {
			str += "charge";
		}
		else {
			str += "nocharge";
		}
		str += "\n";
	}

	return str;
}

class LiveChatClientListener : public ILiveChatClientListener
{
public:
	LiveChatClientListener() {}
	virtual ~LiveChatClientListener() {}

public:
	// �ͻ�����������
	// �ص������Ĳ����� err ֮ǰ��Ϊ����������� errmsg ֮��Ϊ���ز���
	virtual void OnLogin(LCC_ERR_TYPE err, const string& errmsg)
	{
		// ��ӡ���
		printf("OnLogin() err:%d, errmsg:%s\n", err, errmsg.c_str());
		// ����������״̬
		SetEvent(g_waitRespondEvent);
	}
	virtual void OnLogout(LCC_ERR_TYPE err, const string& errmsg)
	{
		// ��ӡ���
		printf("OnLogout() err:%d, errmsg:%s\n", err, errmsg.c_str());
		// ����������״̬
		SetEvent(g_waitRespondEvent);
	}
	virtual void OnSetStatus(LCC_ERR_TYPE err, const string& errmsg)
	{
	}
	virtual void OnEndTalk(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg)
	{
	}
	virtual void OnGetUserStatus(const UserIdList& inList, LCC_ERR_TYPE err, const string& errmsg, const UserStatusList& list)
	{
	}
	virtual void OnGetTalkInfo(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const string& userId, const string& invitedId, bool charge, unsigned int chatTime)
	{
	}
	virtual void OnSendTextMessage(const string& inUserId, const string& inMessage, int inTicket, LCC_ERR_TYPE err, const string& errmsg)
	{
		// ��ӡ���
		printf("OnSendMessage() err:%d errmsg:%s, userId:%s, msg:%s\n", err, errmsg.c_str(), inUserId.c_str(), inMessage.c_str());
		// ����������״̬
		SetEvent(g_waitRespondEvent);
	}
	virtual void OnSendEmotion(const string& inUserId, const string& inEmotionId, int inTicket, LCC_ERR_TYPE err, const string& errmsg)
	{
		// ��ӡ���
		printf("OnSendEmotion() err:%d errmsg:%s, userId:%s, emotion:%s\n", err, errmsg.c_str(), inUserId.c_str(), inEmotionId.c_str());
		// ����������״̬
		SetEvent(g_waitRespondEvent);
	}
	virtual void OnSendVGift(const string& inUserId, const string& inGiftId, int inTicket, LCC_ERR_TYPE err, const string& errmsg)
	{
	}
	virtual void OnGetVoiceCode(const string& inUserId, int ticket, LCC_ERR_TYPE err, const string& errmsg, const string& voiceCode){}
	virtual void OnSendVoice(const string& inUserId, const string& inVoiceId, int inTicket, LCC_ERR_TYPE err, const string& errmsg){}
	virtual void OnUseTryTicket(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const string& userId, TRY_TICKET_EVENT tickEvent){}
	virtual void OnGetTalkList(int inListType, LCC_ERR_TYPE err, const string& errmsg, const TalkListInfo& talkListInfo)
	{
		string str("");
		str += "---- inchat:\n";
		str += GetTaskUserString(talkListInfo.chating, talkListInfo.chatingSession);
		str += "---- pause:\n";
		str += GetTaskUserString(talkListInfo.pause, talkListInfo.pauseSession);
		str += "---- invite:\n";
		str += GetTaskUserString(talkListInfo.invite, talkListInfo.inviteSession);
		str += "---- invited:\n";
		str += GetTaskUserString(talkListInfo.invited, talkListInfo.invitedSession);

		// ��ӡ���յ���Ϣ
		printf(">>>");
		printf("OnGetTalkList() err:%d, errmsg:%s\n%s", err, errmsg.c_str(), str.c_str());
		printf("> ");

		// ����������״̬
		SetEvent(g_waitRespondEvent);
	}
	virtual void OnSendPhoto(LCC_ERR_TYPE err, const string& errmsg, int ticket){}
	virtual void OnSendLadyPhoto(LCC_ERR_TYPE err, const string& errmsg, int ticket){}
	virtual void OnShowPhoto(LCC_ERR_TYPE err, const string& errmsg, int ticket){}
	virtual void OnGetUserInfo(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const UserInfoItem& userInfo)
	{
		// userInfoתstring
		char buffer[2048] = {0};
		sprintf_s(buffer, sizeof(buffer), "id:%s, name:%s, clientType:%d, clientVersion:%s"
			, userInfo.userId.c_str(), userInfo.userName.c_str(), userInfo.clientType, userInfo.clientVersion.c_str());

		// ��ӡ���
		printf("OnGetUserInfo() err:%d, errmsg:%s, %s\n", err, errmsg.c_str(), buffer);

		// ����������״̬
		SetEvent(g_waitRespondEvent);
	}
	virtual void OnGetUsersInfo(LCC_ERR_TYPE err, const string& errmsg, int seq, const UserInfoList& userList){}
	virtual void OnGetContactList(CONTACT_LIST_TYPE inListType, LCC_ERR_TYPE err, const string& errmsg, const TalkUserList& list){}
	virtual void OnGetBlockUsers(LCC_ERR_TYPE err, const string& errmsg, const list<string>& users){}
	virtual void OnSearchOnlineMan(LCC_ERR_TYPE err, const string& errmsg, const list<string>& userList){}
	virtual void OnReplyIdentifyCode(LCC_ERR_TYPE err, const string& errmsg){}
	virtual void OnGetRecentContactList(LCC_ERR_TYPE err, const string& errmsg, const list<string>& userList){}
	virtual void OnGetFeeRecentContactList(LCC_ERR_TYPE err, const string& errmsg, const list<string>& userList){}
	virtual void OnGetLadyChatInfo(LCC_ERR_TYPE err, const string& errmsg, const list<string>& chattingList, const list<string>& chattingInviteIdList, const list<string>& missingList, const list<string>& missingInviteIdList){}
	virtual void OnPlayVideo(LCC_ERR_TYPE err, const string& errmsg, int ticket){}
	virtual void OnSendLadyVideo(LCC_ERR_TYPE err, const string& errmsg, int ticket){}
	virtual void OnSendMagicIcon(const string& inUserId, const string& inIconId, int inTicket, LCC_ERR_TYPE err, const string& errmsg)
	{
		// ��ӡ���
		printf("OnSendMagicIcon err:%d, errmsg:%s, userId:%s, icon:%s\n", err, errmsg.c_str(), inUserId.c_str(), inIconId.c_str());
		// ����������״̬
		SetEvent(g_waitRespondEvent);
	}
	virtual void OnGetLadyCondition(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const LadyConditionItem& item)
	{
		// ��ӡlog
		printf(">>>");
		printf("OnGetLadyCondition() userId:%s, err:%d, errmsg:%s\n", inUserId.c_str(), err, errmsg.c_str());
		printf("> ");
		// ����������״̬
		SetEvent(g_waitRespondEvent);
	}
	virtual void OnGetLadyCustomTemplate(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const vector<string>& contents, const vector<bool>& flags)
	{
		// ��ӡlog
		printf(">>>");
		printf("OnGetLadyCustomTemplate() userId:%s, err:%d, errmsg:%s\n", inUserId.c_str(), err, errmsg.c_str());
		printf("> ");
		// ����������״̬
		SetEvent(g_waitRespondEvent);
	}
	virtual void OnGetPaidTheme(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const ThemeInfoList& themeList) 
	{
		string str("");
		for (ThemeInfoList::const_iterator iter = themeList.begin();
			iter != themeList.end();
			iter++)
		{
			str += GetThemeInfoItemPrintStr((*iter));
		}

		// ��ӡlog
		printf(">>>");
		printf("OnGetPaidTheme() userId:%s, err:%d, errmsg:%s\n%s", inUserId.c_str(), err, errmsg.c_str(), str.c_str());
		printf("> ");

		// ����������״̬
		SetEvent(g_waitRespondEvent);
	}
	virtual void OnGetAllPaidTheme(LCC_ERR_TYPE err, const string& errmsg, const ThemeInfoList& themeInfoList) 
	{
		string str("");
		for (ThemeInfoList::const_iterator iter = themeInfoList.begin();
			iter != themeInfoList.end();
			iter++)
		{
			str += GetThemeInfoItemPrintStr((*iter));
		}

		// ��ӡlog
		printf(">>>");
		printf("OnGetAllPaidTheme() err:%d, errmsg:%s\n%s", err, errmsg.c_str(), str.c_str());
		printf("> ");

		// ����������״̬
		SetEvent(g_waitRespondEvent);
	}
	virtual void OnManFeeTheme(const string& inUserId, const string& inThemeId, LCC_ERR_TYPE err, const string& errmsg, const ThemeInfoItem& item) 
	{
		string str = GetThemeInfoItemPrintStr(item);

		// ��ӡlog
		printf(">>>");
		printf("OnManFeeTheme() userId:%s, themeId:%s, err:%d, errmsg:%s\n%s", inUserId.c_str(), inThemeId.c_str(), err, errmsg.c_str(), str.c_str());
		printf("> ");

		// ����������״̬
		SetEvent(g_waitRespondEvent);
	}
	virtual void OnManApplyTheme(const string& inUserId, const string& inThemeId, LCC_ERR_TYPE err, const string& errmsg, const ThemeInfoItem& item) 
	{
		string str = GetThemeInfoItemPrintStr(item);

		// ��ӡlog
		printf(">>>");
		printf("OnManApplyTheme() userId:%s, themeId:%s, err:%d, errmsg:%s\n%s", inUserId.c_str(), inThemeId.c_str(), err, errmsg.c_str(), str.c_str());
		printf("> ");

		// ����������״̬
		SetEvent(g_waitRespondEvent);
	}
	virtual void OnPlayThemeMotion(const string& inUserId, const string& inThemeId, LCC_ERR_TYPE err, const string& errmsg, bool success) 
	{
		// ��ӡlog
		printf(">>>");
		printf("OnPlayThemeMotion() userId:%s, themeId:%s, err:%d, errmsg:%s, success:%d\n", inUserId.c_str(), inThemeId.c_str(), err, errmsg.c_str(), success);
		printf("> ");

		// ����������״̬
		SetEvent(g_waitRespondEvent);
	}
	// ��������������
	virtual void OnRecvMessage(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, int ticket, TALK_MSG_TYPE msgType, const string& message)
	{
		// ��ӡ���յ���Ϣ
		printf(">>>");
		printf("OnRecvMessage() toId:%s, fromId:%s, fromName:%s, msg:%s\n", toId.c_str(), fromId.c_str(), fromName.c_str(), message.c_str());
		printf("> ");
	}
	virtual void OnRecvEmotion(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, int ticket, TALK_MSG_TYPE msgType, const string& emotionId)
	{
		// ��ӡ���յ���Ϣ
		printf(">>>");
		printf("OnRecvEmotion() toId:%s, fromId:%s, fromName:%s, emotionId:%s\n", toId.c_str(), fromId.c_str(), fromName.c_str(), emotionId.c_str());
		printf("> ");
	}
	virtual void OnRecvVoice(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, TALK_MSG_TYPE msgType, const string& voiceId, const string& fileType, int timeLen){}
	virtual void OnRecvWarning(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, int ticket, TALK_MSG_TYPE msgType, const string& message){}
	virtual void OnUpdateStatus(const string& userId, const string& server, CLIENT_TYPE clientType, USER_STATUS_TYPE statusType)
	{
		// ��ӡ���յ���Ϣ
		printf(">>>");
		printf("OnUpdateStatus() userId:%s, server:%s, clientType:%d, statusType:%s\n", userId.c_str(), server.c_str(), clientType, statusType);
		printf("> ");
	}
	virtual void OnUpdateTicket(const string& fromId, int ticket){}
	virtual void OnRecvEditMsg(const string& fromId){}
	virtual void OnRecvTalkEvent(const string& userId, TALK_EVENT_TYPE eventType){}
	virtual void OnRecvTryTalkBegin(const string& toId, const string& fromId, int time){}
	virtual void OnRecvTryTalkEnd(const string& userId){}
	virtual void OnRecvEMFNotice(const string& fromId, TALK_EMF_NOTICE_TYPE noticeType){}
	virtual void OnRecvKickOffline(KICK_OFFLINE_TYPE kickType){}
	virtual void OnRecvPhoto(const string& toId, const string& fromId, const string& fromName, const string& inviteId, const string& photoId, const string& sendId, bool charge, const string& photoDesc, int ticket){}
	virtual void OnRecvShowPhoto(const string& toId, const string& fromId, const string& fromName, const string& inviteId, const string& photoId, const string& sendId, bool charge, const string& photoDec, int ticket){}
	virtual void OnRecvLadyVoiceCode(const string& voiceCode){}
	virtual void OnRecvIdentifyCode(const unsigned char* data, long dataLen){}
	virtual void OnRecvVideo(const string& toId, const string& fromId, const string& fromName, const string& inviteId, const string& videoId, const string& sendId, bool charge, const string& videoDesc, int ticket){}
	virtual void OnRecvShowVideo(const string& toId, const string& fromId, const string& fromName, const string& inviteId, const string& videoId, const string& sendId, bool charge, const string& videoDesc, int ticket){}
	virtual void OnRecvAutoInviteMsg(const string& womanId, const string& manId, const string& key) 
	{
		// ��ӡlog
		printf(">>>");
		printf("OnRecvAutoInviteMsg() womanId:%s, manId:%s, key:%s\n", womanId.c_str(), manId.c_str(), key.c_str());
		printf("> ");

		g_client->UploadPopLadyAutoInvite(womanId, "1", key);
	};
	virtual void OnRecvAutoChargeResult(const string& manId, double money, TAUTO_CHARGE_TYPE type, bool result, const string& code, const string& msg)
	{
		// ��ӡlog
		printf(">>>");
		printf("OnRecvAutoChargeResult() manId:%s, money:%f, type:%d, result:%d, code:%s, msg:%s\n", manId.c_str(), money, type, result, code.c_str(), msg.c_str());
		printf("> ");
	};
	virtual void OnRecvMagicIcon(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, int ticket, TALK_MSG_TYPE msgType, const string& iconId)
	{
		// ��ӡ���յ���Ϣ
		printf(">>>");
		printf("OnRecvMagicIcon() toId:%s, fromId:%s, fromName:%s, icon:%s\n", toId.c_str(), fromId.c_str(), fromName.c_str(), iconId.c_str());
		printf("> ");
	}
	virtual void OnRecvThemeMotion(const string& themeId, const string& manId, const string& womanId) 
	{
		// ��ӡlog
		printf(">>>");
		printf("OnRecvThemeMotion() themeId:%s, manId:%s, womanId:%s\n", themeId.c_str(), manId.c_str(), womanId.c_str());
		printf("> ");
	}
	virtual void OnRecvThemeRecommend(const string& themeId, const string& manId, const string& womanId) 
	{
		// ��ӡlog
		printf(">>>");
		printf("OnRecvThemeRecommend() themeId:%s, manId:%s, womanId:%s\n", themeId.c_str(), manId.c_str(), womanId.c_str());
		printf("> ");
	}
};

LiveChatClientListener g_clientListener;

// ------ ѭ�������̺߳��� ------
// ѭ������������־
bool g_isLoopThreadRun = false;
// ѭ������item
typedef struct _tagLoopCmdItem
{
	string cmd;
	long long handleTime;
	_tagLoopCmdItem() {
		handleTime = 0;
	}
	_tagLoopCmdItem(const struct _tagLoopCmdItem& item) {
		cmd = item.cmd;
		handleTime = item.handleTime;
	}
} LoopCmdItem;
// ѭ���������
list_lock<LoopCmdItem>	g_loopCmdList;
static TH_RETURN_PARAM LoopThreadProc(void* arg)
{
	LoopCmdItem item;
	bool isLoopCmdListEmpty = true;
	while (g_isLoopThreadRun)
	{
		// ��ȡ����
		g_loopCmdList.lock();
		isLoopCmdListEmpty = g_loopCmdList.empty();
		if (!isLoopCmdListEmpty) {
			item = *g_loopCmdList.begin();
			g_loopCmdList.pop_front();
		}
		g_loopCmdList.unlock();

		if (!isLoopCmdListEmpty)
		{
			// ��������
			if (getCurrentTime() >= item.handleTime) {
				// ִ������
				bool isWait = false;
				HandleCmd(item.cmd.c_str(), isWait);
			}
			else {
				// ����δ��ʱ�䣬�������β
				g_loopCmdList.lock();
				g_loopCmdList.push_back(item);
				g_loopCmdList.unlock();
			}
		}
		Sleep(1000);
	}

	return 0;
}

// ------ init������� ------
void InitInfo()
{
	printf("init ip1,ip2,...ipn port\n");
}

bool InitHandle(const list<string>& cmdList)
{
	bool isWait = false;

	if (cmdList.size() == 3)
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		string ipListCmd = (*iter);
		list<string> ipList = StringHandle::split(ipListCmd, ",");
		iter++;

		int port = atoi((*iter).c_str());

		if (g_client->Init(ipList, port, &g_clientListener)) {
			printf("init success!\n");
		}
		else {
			printf("init fail!\n");
		}
	}
	else
	{
		InitInfo();
	}

	return isWait;
}

// ------ login������� ------
void LoginInfo()
{
	printf("login account password sex[female:0/male:1] [authtype: sid:0/pwd:1] [deviceid]\n");
}

bool LoginHandle(const list<string>& cmdList)
{
	bool isWait = true;

	if (cmdList.size() > 3)
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// account
		string account = (*iter);
		iter++;

		// password
		string password = (*iter);
		iter++;

		// sex
		USER_SEX_TYPE sex = (USER_SEX_TYPE)atoi((*iter).c_str());
		iter++;

		// authType
		AUTH_TYPE authType = AUTH_TYPE_PWD;
		if (iter != cmdList.end()) {
			authType = (AUTH_TYPE)atoi((*iter).c_str());
			iter++;
		}

		// deviceId
		string deviceId = "000000000";
		if (iter != cmdList.end()) {
			deviceId = (*iter);
			iter++;
		}

		bool result = g_client->Login(account, password, deviceId, CLIENT_ANDROID, sex, authType);
		if (!result) {
			printf("Login fail!\n");
		}
		else {
			// set console title
			SetConsoleTitle(account.c_str());
		}
	}
	else {
		LoginInfo();
		isWait = false;
	}
	return isWait;
}

// ------ logout������� ------
void LogoutInfo()
{
	printf("logout\n");
}

bool LogoutHandle(const list<string>& cmdList)
{
	bool isWait = true;

	g_client->Logout();
	return isWait;
}

// ------ text������� ------
void SendTextInfo()
{
	printf("text userId message\n");
}

bool SendTextHandle(const list<string>& cmdList)
{
	bool isWait = true;

	if (cmdList.size() > 2) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// userId
		string userId = (*iter);
		iter++;

		// message
		string message = (*iter);
		iter++;

		int ticket = g_msgCounter.GetAndIncrement();
		bool result = g_client->SendTextMessage(userId, message, false, ticket);
		if (!result) {
			printf("text fail!\n");
		}
	}
	else {
		SendTextInfo();
		isWait = false;
	}

	return isWait;
}

// ------ looptext������� ------
void SendLoopTextInfo()
{
	printf("looptext start [second:5] userId\n");
	printf("looptext end userId\n");
}

// ѭ���ı���Ϣ������־
bool g_loopTextRun = false;
// ѭ���ı���Ϣ������
Counter g_loopTextCounter;
// ѭ���ı���Ϣ�����û��б�
list<string> g_loopTextUserList;
// ѭ���ı���Ϣ���ͼ��ʱ��
int g_loopTextStepTime = 5;
void InsertLoopTextProcCmd(const list<string>& userList)
{
	LoopCmdItem item;

	// ��������
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

	// ����ʱ��
	item.handleTime = getCurrentTime() + (g_loopTextStepTime * 1000);

	// ����ѭ����������
	g_loopCmdList.lock();
	g_loopCmdList.push_back(item);
	g_loopCmdList.unlock();
}

bool SendLoopTextHandle(const list<string>& cmdList)
{
	bool isWait = false;

	if (cmdList.size() >= 2) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// cmd
		string cmd = *iter;
		iter++;

		// sec
		int sec = 5;
		if (iter != cmdList.end())
		{
			string strSec = *iter;
			int temp = atoi(strSec.c_str());
			if (temp > 0) {
				sec = temp;
				iter++;
			}
		}

		// userList
		list<string> userList;
		while (iter != cmdList.end())
		{
			userList.push_back(*iter);
			iter++;
		}

		bool isHandle = false;
		if (cmd == "start")
		{
			isHandle = true;
			if (!g_loopTextRun) 
			{
				// ��������
				g_loopTextRun  = true;

				// ��ʼ������
				g_loopTextCounter.Init();

				// ���淢��ʱ����
				g_loopTextStepTime = sec;

				// ����ѭ����������
				InsertLoopTextProcCmd(userList);
			}
			else {
				printf("looptext is started.\n");
			}
		}
		else if (cmd == "end") {
			isHandle = true;
			if (g_loopTextRun) {
				// ֹͣ
				g_loopCmdList.lock();
				for (list_lock<LoopCmdItem>::iterator itemIter = g_loopCmdList.begin();
					itemIter != g_loopCmdList.end();
					itemIter++)
				{
					if (NULL != strstr((*itemIter).cmd.c_str(), "looptextproc"))
					{
						g_loopCmdList.erase(itemIter);
						break;
					}
				}
				g_loopCmdList.unlock();

				g_loopTextRun = false;
			}
			else {
				printf("looptext is not start.\n");
			}
		}

		if (!isHandle) {
			SendLoopTextInfo();
		}
	}
	else {
		SendLoopTextInfo();
	}

	return isWait;
}

bool SendLoopTextProc(const list<string>& cmdList)
{
	bool isWait = false;
	if (cmdList.size() >= 2) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// ������Ϣ
		char message[32] = {0};
		_itoa_s(g_loopTextCounter.GetAndIncrement(), message, sizeof(message), 10);

		// userList
		list<string> userList;
		while (iter != cmdList.end())
		{
			// ������Ϣ
			int ticket = g_msgCounter.GetAndIncrement();
			g_client->SendTextMessage(*iter, message, false, ticket);

			userList.push_back(*iter);
			iter++;
		}

		InsertLoopTextProcCmd(userList);
	}

	return isWait;
}

// ------ emotion������� ------
void SendEmotionInfo()
{
	printf("emotion userId message\n");
}

bool SendEmotionHandle(const list<string>& cmdList)
{
	bool isWait = true;

	if (cmdList.size() > 2) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// userId
		string userId = (*iter);
		iter++;

		// message
		string emotionId = (*iter);
		iter++;

		int ticket = g_msgCounter.GetAndIncrement();
		bool result = g_client->SendEmotion(userId, emotionId, ticket);
		if (!result) {
			printf("text fail!\n");
		}
	}
	else {
		SendEmotionInfo();
		isWait = false;
	}

	return isWait;
}

// ------ loopemotion������� ------
void SendLoopEmotionInfo()
{
	printf("loopemotion start [second:5] userId\n");
	printf("loopemotion end userId\n");
}

// ѭ���߱���Ϣ������־
bool g_loopEmotionRun = false;
// ѭ���߱���Ϣ������
Counter g_loopEmotionCounter;
// ѭ���߱���Ϣ�����û��б�
list<string> g_loopEmotionUserList;
// ѭ���߱���Ϣ���ͼ��ʱ��
int g_loopEmotionStepTime = 5;
void InsertLoopEmotionProcCmd(const list<string>& userList)
{
	LoopCmdItem item;

	// ��������
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

	// ����ʱ��
	item.handleTime = getCurrentTime() + (g_loopEmotionStepTime * 1000);

	// ����ѭ����������
	g_loopCmdList.lock();
	g_loopCmdList.push_back(item);
	g_loopCmdList.unlock();
}

bool SendLoopEmotionHandle(const list<string>& cmdList)
{
	bool isWait = false;

	if (cmdList.size() >= 2) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// cmd
		string cmd = *iter;
		iter++;

		// sec
		int sec = 5;
		if (iter != cmdList.end())
		{
			string strSec = *iter;
			int temp = atoi(strSec.c_str());
			if (temp > 0) {
				sec = temp;
				iter++;
			}
		}

		// userList
		list<string> userList;
		while (iter != cmdList.end())
		{
			userList.push_back(*iter);
			iter++;
		}

		bool isHandle = false;
		if (cmd == "start")
		{
			isHandle = true;
			if (!g_loopEmotionRun) 
			{
				// ��������
				g_loopEmotionRun  = true;

				// ��ʼ������
				g_loopEmotionCounter.Init();

				// ���淢��ʱ����
				g_loopEmotionStepTime = sec;

				// ����ѭ����������
				InsertLoopEmotionProcCmd(userList);
			}
			else {
				printf("loopemotion is started.\n");
			}
		}
		else if (cmd == "end") {
			isHandle = true;
			if (g_loopEmotionRun) {
				// ֹͣ
				g_loopCmdList.lock();
				for (list_lock<LoopCmdItem>::iterator itemIter = g_loopCmdList.begin();
					itemIter != g_loopCmdList.end();
					itemIter++)
				{
					if (NULL != strstr((*itemIter).cmd.c_str(), "loopemotionproc"))
					{
						g_loopCmdList.erase(itemIter);
						break;
					}
				}
				g_loopCmdList.unlock();

				g_loopEmotionRun = false;
			}
			else {
				printf("loopemotion is not start.\n");
			}
		}

		if (!isHandle) {
			SendLoopEmotionInfo();
		}
	}
	else {
		SendLoopEmotionInfo();
	}

	return isWait;
}

bool SendLoopEmotionProc(const list<string>& cmdList)
{
	bool isWait = false;
	if (cmdList.size() >= 2) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// ������Ϣ
		char message[32] = {0};
		_itoa_s(g_loopEmotionCounter.GetAndIncrement(), message, sizeof(message), 10);

		// userList
		list<string> userList;
		while (iter != cmdList.end())
		{
			// ������Ϣ
			int ticket = g_msgCounter.GetAndIncrement();
			g_client->SendEmotion(*iter, "W03", ticket);

			userList.push_back(*iter);
			iter++;
		}

		InsertLoopEmotionProcCmd(userList);
	}

	return isWait;
}

// ------ magicIcon������� ------
void SendMagicIconInfo()
{
	printf("magicicon userId iconId\n");
}

bool SendMagicIconHandle(const list<string>& cmdList)
{
	bool isWait = true;

	if (cmdList.size() > 2) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// userId
		string userId = (*iter);
		iter++;

		// iconId
		string iconId = (*iter);
		iter++;

		int ticket = g_msgCounter.GetAndIncrement();
		bool result = g_client->SendMagicIcon(userId, iconId, ticket);
		if (!result) {
			printf("magicicon fail!\n");
		}
	}
	else {
		SendMagicIconInfo();
		isWait = false;
	}

	return isWait;
}

// ------ userinfo������� ------
void GetUserInfo()
{
	printf("userinfo userId\n");
}

bool GetUserInfoHandle(const list<string>& cmdList)
{
	bool isWait = true;

	if (cmdList.size() > 1) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// userId
		string userId = (*iter);
		iter++;

		bool result = g_client->GetUserInfo(userId);
		if (!result) {
			printf("userinfo fail!\n");
		}
	}
	else {
		GetUserInfo();
		isWait = false;
	}

	return isWait;
}

// ------ uploadver������� ------
void UploadVerInfo()
{
	printf("uploadver version\n");
}

bool UploadVerHandle(const list<string>& cmdList)
{
	bool isWait = false;

	if (cmdList.size() > 1) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// userId
		string ver = (*iter);
		iter++;

		bool result = g_client->UploadVer(ver);
		if (!result) {
			printf("uploadver fail!\n");
		}
	}
	else {
		UploadVerInfo();
	}

	return isWait;
}

// ------ uploadthemever������� ------
void UploadThemeVerInfo()
{
	printf("uploadthemever version\n");
}

bool UploadThemeVerHandle(const list<string>& cmdList)
{
	bool isWait = false;

	if (cmdList.size() > 1) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// themeVersion
		string ver = (*iter);
		iter++;

		int iVer = atoi(ver.c_str());
		bool result = g_client->UploadThemeListVer(iVer);
		if (!result) {
			printf("uploadthemever fail!\n");
		}
	}
	else {
		UploadThemeVerInfo();
	}

	return isWait;
}

// ------ getusertheme������� ------
void GetUserThemeInfo()
{
	printf("getusertheme userId\n");
}

bool GetUserThemeHandle(const list<string>& cmdList)
{
	bool isWait = false;

	if (cmdList.size() > 1) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// userId
		string userId = (*iter);
		iter++;

		bool result = g_client->GetPaidTheme(userId);
		if (!result) {
			printf("getusertheme fail!\n");
		}

		isWait = result;
	}
	else {
		GetUserThemeInfo();
	}

	return isWait;
}

// ------ getalltheme������� ------
void GetAllThemeInfo()
{
	printf("getalltheme userId\n");
}

bool GetAllThemeHandle(const list<string>& cmdList)
{
	bool isWait = false;

	if (cmdList.size() > 0) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		bool result = g_client->GetAllPaidTheme();
		if (!result) {
			printf("getusertheme fail!\n");
		}

		isWait = result;
	}
	else {
		GetAllThemeInfo();
	}

	return isWait;
}

// ------ buytheme������� ------
void BuyThemeInfo()
{
	printf("buytheme userId themeId\n");
}

bool BuyThemeHandle(const list<string>& cmdList)
{
	bool isWait = false;

	if (cmdList.size() > 2) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// userId
		string userId = (*iter);
		iter++;

		// themeId
		string themeId = (*iter);
		iter++;

		bool result = g_client->ManFeeTheme(userId, themeId);
		if (!result) {
			printf("buytheme fail!\n");
		}

		isWait = result;
	}
	else {
		BuyThemeInfo();
	}

	return isWait;
}

// ------ applytheme������� ------
void ApplyThemeInfo()
{
	printf("applytheme userId themeId\n");
}

bool ApplyThemeHandle(const list<string>& cmdList)
{
	bool isWait = false;

	if (cmdList.size() > 2) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// userId
		string userId = (*iter);
		iter++;

		// themeId
		string themeId = (*iter);
		iter++;

		bool result = g_client->ManApplyTheme(userId, themeId);
		if (!result) {
			printf("applytheme fail!\n");
		}

		isWait = result;
	}
	else {
		ApplyThemeInfo();
	}

	return isWait;
}

// ------ playtheme������� ------
void PlayThemeInfo()
{
	printf("playtheme userId themeId\n");
}

bool PlayThemeHandle(const list<string>& cmdList)
{
	bool isWait = false;

	if (cmdList.size() > 2) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// userId
		string userId = (*iter);
		iter++;

		// themeId
		string themeId = (*iter);
		iter++;

		bool result = g_client->PlayThemeMotion(userId, themeId);
		if (!result) {
			printf("playtheme fail!\n");
		}

		isWait = result;
	}
	else {
		PlayThemeInfo();
	}

	return isWait;
}

// ------ gettalklist������� ------
void GetTalkListInfo()
{
	printf("gettalklist\n");
}

bool GetTalkListHandle(const list<string>& cmdList)
{
	bool isWait = false;

	if (cmdList.size() > 0) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		bool result = g_client->GetTalkList(0x03);
		if (!result) {
			printf("gettalklist fail!\n");
		}

		isWait = result;
	}
	else {
		GetTalkListInfo();
	}

	return isWait;
}

// ------ exit������� ------
void ExitInfo()
{
	printf("exit\n");
}

// ------ help������� ------
bool HelpHandle(const list<string>& cmdList)
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

	return isWait;
}

bool HandleArg(const char* arg)
{
	bool exit = false;

	// �ָ�������
	list<string> cmdList = StringHandle::split(arg, ";");

	// ����������
	bool isWait = false;
	for (list<string>::iterator iter = cmdList.begin();
		iter != cmdList.end() && !exit;
		iter++)
	{
		// �ȴ��ص��¼�
		WaitForSingleObject(g_waitRespondEvent, INFINITE);

		// ��ӡ����
		printf("> %s\n", (*iter).c_str());

		// ��������
		exit = HandleCmd((*iter).c_str(), isWait);

		// ���õȴ��ص�����ı��ź�������������������״̬
		if (!isWait) {
			SetEvent(g_waitRespondEvent);
		}
	}

	return exit;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// ��ʼ��socket����
	ISocketHandler::InitEnvironment();
	
	// ��ʼ����Ϣ������
	g_msgCounter.Init();

	// ��������ʼ��
	g_client = ILiveChatClient::CreateClient();
	
	// ����������״̬
	if (NULL != g_client)
	{
		// ��ʼ��
		printf("LiveChatClient is running! Please input command, 'help' print all command.\n");
		char command[2048] = {0};
		g_waitRespondEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		bool exit = false;

		// ѭ�������߳�
		g_isLoopThreadRun = true;
		IThreadHandler* threadHandler = IThreadHandler::CreateThreadHandler();
		threadHandler->Start(LoopThreadProc, NULL);

		// ����main����
		if (argc >= 2) {
			exit = HandleArg(argv[1]);
		}

		// �����û�����
		while (!exit)
		{
			// �ȴ��ص��¼�
			WaitForSingleObject(g_waitRespondEvent, INFINITE);

			// ��ӡ����������ʾ��
			printf("> ");

			// �û���������
			gets_s(command, sizeof(command));

			// ��������
			bool isWait = false;
			exit = HandleCmd(command, isWait);
			if (exit) {
				break;
			}

			// ���õȴ��ص�����ı��ź�������������������״̬
			if (!isWait) {
				SetEvent(g_waitRespondEvent);
			}
		}

		// ֹͣѭ�������߳�
		g_isLoopThreadRun = false;
		threadHandler->WaitAndStop();
		IThreadHandler::ReleaseThreadHandler(threadHandler);

		// �ͷ�
		ILiveChatClient::ReleaseClient(g_client);
		g_client = NULL;
	}
	else 
	{
		printf("CreateClient fail!\n");
	}

	printf("shutdown Now!\n");

	// �ͷ�socket����
	ISocketHandler::ReleaseEnvironment();

	return 0;
}

// ���������
bool HandleCmd(const char* command, bool& isWait)
{
	bool exit = false;

	// �ָ�����
	list<string> cmdList = StringHandle::split(command, " ");

	isWait = false;
	if (cmdList.empty() || 0 == (*cmdList.begin()).compare("help")) {
		// ����(��ӡ��������)
		isWait = HelpHandle(cmdList);
	}
	else if ((*cmdList.begin()) == "exit") {
		// �˳�
		exit = true;
	}
	else if ((*cmdList.begin()) == "init") {
		// ��ʼ��
		isWait = InitHandle(cmdList);
	}
	else if ((*cmdList.begin()) == "login") {
		// ��¼
		isWait = LoginHandle(cmdList);
	}
	else if ((*cmdList.begin()) == "logout") {
		// ע��
		isWait = LogoutHandle(cmdList);
	}
	else if ((*cmdList.begin()) == "text") {
		// �����ı���Ϣ
		isWait = SendTextHandle(cmdList);
	}
	else if ((*cmdList.begin()) == "looptext") {
		// ����ѭ���ı���Ϣ
		isWait = SendLoopTextHandle(cmdList);
	}
	else if ((*cmdList.begin()) == "looptextproc") {
		// ����ѭ���ı���Ϣ����
		isWait = SendLoopTextProc(cmdList);
	}
	else if ((*cmdList.begin()) == "emotion") {
		// ���͸߼�������Ϣ 
		isWait = SendEmotionHandle(cmdList);
	}
	else if ((*cmdList.begin()) == "loopemotion") {
		// ����ѭ���߼�������Ϣ
		isWait = SendLoopEmotionHandle(cmdList);
	}
	else if ((*cmdList.begin()) == "loopemotionproc") {
		// ����ѭ���߼�������Ϣ����
		isWait = SendLoopEmotionProc(cmdList);
	}
	else if ((*cmdList.begin()) == "magicicon") {
		// ����С�߼�������Ϣ
		isWait = SendMagicIconHandle(cmdList);
	}
	else if ((*cmdList.begin()) == "userinfo") {
		// ��ȡ�û���Ϣ
		isWait = GetUserInfoHandle(cmdList);
	}
	else if ((*cmdList.begin()) == "uploadver") {
		// �ϴ��汾��
		isWait = UploadVerHandle(cmdList);
	}
	else if ((*cmdList.begin()) == "uploadthemever") {
		// �ϴ�������б�汾��
		isWait = UploadThemeVerHandle(cmdList);
	}
	else if ((*cmdList.begin()) == "getusertheme") {
		// ��ȡ��/Ůʿ�����
		isWait = GetUserThemeHandle(cmdList);
	}
	else if ((*cmdList.begin()) == "getalltheme") {
		// ��ȡ���������
		isWait = GetAllThemeHandle(cmdList);
	}
	else if (*cmdList.begin() == "buytheme") {
		// ���������
		isWait = BuyThemeHandle(cmdList);
	}
	else if (*cmdList.begin() == "applytheme") {
		// Ӧ�������
		isWait = ApplyThemeHandle(cmdList);
	}
	else if (*cmdList.begin() == "playtheme") {
		// �������������
		isWait = PlayThemeHandle(cmdList);
	}
	else if (*cmdList.begin() == "gettalklist") {
		// ��ȡTalkList
		isWait = GetTalkListHandle(cmdList);
	}
	else {
		// error
		printf("Unknow command, 'help' print all command!\n");
	}

	return exit;
}
