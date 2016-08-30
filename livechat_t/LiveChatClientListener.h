/*
 * author: Alex shum
 *   date: 2015-06-16
 *   file: LiveChatClientListener.h
 *   desc: �ͷ��˼���ʵ���࣬��Ҫ�����ǿͻ���������Ҫ�������ظ�����£��õ��ظ���һ��Onrevc֮���task->handle()�������Ĵ������е��Ƿ��������͹���������
 */

#pragma once
#include "stdafx.h"
#include <livechat/ILiveChatClient.h>
#include <common/StringHandle.h>
#include <livechat/ISocketHandler.h>
#include <livechat/Counter.h>
#include <string>
#include <livechat/IThreadHandler.h>
#include <common/list_lock.h>
#include <common/CommonFunc.h>
//#include "ICmdHandle.h"

using namespace std;

// LiveChat�ͻ���
extern ILiveChatClient* g_client;
// �ȴ��ص���event
extern HANDLE g_waitRespondEvent;


string GetThemeInfoItemPrintStr(const ThemeInfoItem& item);
// ��ȡTalkUserList��TalkSessionList��ӡ���ַ���
string GetTaskUserString(const TalkUserList& userList, const TalkSessionList& sessionList);

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

	virtual void OnGetAutoInviteMsgSwitchStatus(LCC_ERR_TYPE err, const string& errmsg, bool isOpenStatus)
	{
		// ��ӡlog
		printf(">>>");
		printf("OnGetAutoInviteStatus() err:%d, errmsg:%s, AutoInviteStatus:%d\n",err, errmsg.c_str(), isOpenStatus);
		printf("> ");

		// ����������״̬
		SetEvent(g_waitRespondEvent);
	}

	virtual void OnSwitchAutoInviteMsg(LCC_ERR_TYPE err, const string& errmsg, bool isOpenStatus)
	{
				// ��ӡlog
		printf(">>>");
		printf("OnSendAutoInvite() err:%d, errmsg:%s, AutoInviteStatus:%d\n",err, errmsg.c_str(), isOpenStatus);
		printf("> ");

		// ����������״̬
		SetEvent(g_waitRespondEvent);
	}

	virtual void OnRecommendThemeToMan(const string& inUserId, const string& inThemeId,LCC_ERR_TYPE err, const string& errmsg){}


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
		printf("OnUpdateStatus() userId:%s, server:%s, clientType:%d, statusType:%d\n", userId.c_str(), server.c_str(), clientType, statusType);
		printf("> ");
	}
	virtual void OnUpdateTicket(const string& fromId, int ticket){}
	virtual void OnRecvEditMsg(const string& fromId){}
	virtual void OnRecvTalkEvent(const string& userId, TALK_EVENT_TYPE eventType){}
	virtual void OnRecvTryTalkBegin(const string& toId, const string& fromId, int time){}
	virtual void OnRecvTryTalkEnd(const string& userId){}
	virtual void OnRecvEMFNotice(const string& fromId, TALK_EMF_NOTICE_TYPE noticeType){}
	virtual void OnRecvKickOffline(KICK_OFFLINE_TYPE kickType){}
	virtual void OnRecvPhoto(const string& NottoId, const string& fromId, const string& fromName, const string& inviteId, const string& photoId, const string& sendId, bool charge, const string& photoDesc, int ticket){}
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

	virtual void OnAutoInviteStatusUpdate(bool isOpenStatus)
	{
		// ��ӡlog
		printf(">>>");
		printf("OnRecvLadyAutoInviteStatus() AutoInviteStatus:%d\n", isOpenStatus);
		printf("> ");
	}

	virtual void OnRecvAutoInviteNotify(const string& womanId,const string& manId,const string& message,const string& inviteId)
	{
		// ��ӡlog
		printf(">>>");
		printf("OnRecvLadyAutoInvite()womanId:%s, manId:%s, message:%s, inviteId:%s\n", womanId.c_str(), manId.c_str(), message.c_str(), inviteId.c_str());
		printf("> ");
	}

	virtual void OnManApplyThemeNotify(const ThemeInfoItem& item)
	{
		string str = GetThemeInfoItemPrintStr(item);
		// ��ӡlog
		printf(">>>");
		printf("OnRecvManApplyTheme() %s", str.c_str());
		printf("> ");
	}

	virtual void OnManBuyThemeNotify(const ThemeInfoItem& item)
	{
		string str = GetThemeInfoItemPrintStr(item);
		// ��ӡlog
		printf(">>>");
		printf("OnRecvManFeeTheme() %s", str.c_str());
		printf("> ");
	}
};