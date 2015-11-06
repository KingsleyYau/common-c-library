/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LiveChatManManager.h
 *   desc: LiveChat男士Manager
 */

#pragma once

#include "ILiveChatManManager.h"
#include "ILiveChatClient.h"
#include "LCUserItem.h"
#include <Counter.h>

#include <list>
#include <string>
using namespace std;

// 消息管理器
class LCTextManager;
class LCEmotionManager;
class LCVoiceManager;
class LCPhotoManager;
class LCVideoManager;
// 用户管理器
class LCUserManager;
class LCInviteManager;
class LCBlockManager;
class LCContactManager;
// LiveChat客户端
class ILiveChatClient;

class LiveChatManManager : private ILiveChatClientListener
									, LCEmotionManagerCallback
{
public:
	LiveChatManManager();
	virtual ~LiveChatManManager();

public:
	// 设置本地缓存目录路径
	bool SetDirPath(const string& path);
	// 初始化
	bool Init(list<string> ipList, int port, const string& webHost);


private:
	string GetLogPath();

	// ------------------- LCEmotionManagerCallback -------------------
private:
	virtual void OnDownloadEmotionImage(bool result, LCEmotionItem* emotionItem);
	virtual void OnDownloadEmotionPlayImage(bool result, LCEmotionItem* emotionItem);

	// ------------------- ILiveChatClientListener -------------------
private:
	// 客户端主动请求
	virtual void OnLogin(LCC_ERR_TYPE err, const string& errmsg);
	virtual void OnLogout(LCC_ERR_TYPE err, const string& errmsg);
	virtual void OnSetStatus(LCC_ERR_TYPE err, const string& errmsg);
	virtual void OnEndTalk(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg);
	virtual void OnGetUserStatus(const UserIdList& inList, LCC_ERR_TYPE err, const string& errmsg, const UserStatusList& list);
	virtual void OnGetTalkInfo(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const string& userId, const string& invitedId, bool charge, unsigned int chatTime);
	virtual void OnSendMessage(const string& inUserId, const string& inMessage, int inTicket, LCC_ERR_TYPE err, const string& errmsg);
	virtual void OnSendEmotion(const string& inUserId, const string& inEmotionId, int inTicket, LCC_ERR_TYPE err, const string& errmsg);
	virtual void OnSendVGift(const string& inUserId, const string& inGiftId, int inTicket, LCC_ERR_TYPE err, const string& errmsg);
	virtual void OnGetVoiceCode(const string& inUserId, int ticket, LCC_ERR_TYPE err, const string& errmsg, const string& voiceCode);
	virtual void OnSendVoice(const string& inUserId, const string& inVoiceId, int inTicket, LCC_ERR_TYPE err, const string& errmsg);
	virtual void OnUseTryTicket(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const string& userId, TRY_TICKET_EVENT tickEvent);
	virtual void OnGetTalkList(int inListType, LCC_ERR_TYPE err, const string& errmsg, const TalkListInfo& talkListInfo);
	virtual void OnSendPhoto(LCC_ERR_TYPE err, const string& errmsg, int ticket);
	virtual void OnShowPhoto(LCC_ERR_TYPE err, const string& errmsg, int ticket);
	virtual void OnGetUserInfo(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const UserInfoItem& userInfo);
	virtual void OnGetUsersInfo(LCC_ERR_TYPE err, const string& errmsg, const UserInfoList& userList);
	virtual void OnGetContactList(CONTACT_LIST_TYPE inListType, LCC_ERR_TYPE err, const string& errmsg, const TalkUserList& list);
	virtual void OnGetBlockUsers(LCC_ERR_TYPE err, const string& errmsg, const list<string>& users);
	virtual void OnSearchOnlineMan(LCC_ERR_TYPE err, const string& errmsg, const list<string>& userList);
	virtual void OnReplyIdentifyCode(LCC_ERR_TYPE err, const string& errmsg);
	virtual void OnGetRecentContactList(LCC_ERR_TYPE err, const string& errmsg, const list<string>& userList);
	virtual void OnGetFeeRecentContactList(LCC_ERR_TYPE err, const string& errmsg, const list<string>& userList);
	virtual void OnGetLadyChatInfo(LCC_ERR_TYPE err, const string& errmsg, const list<string>& chattingList, const list<string>& chattingInviteIdList, const list<string>& missingList, const list<string>& missingInviteIdList);
	virtual void OnPlayVideo(LCC_ERR_TYPE err, const string& errmsg, int ticket);
	// 服务器主动请求
	virtual void OnRecvMessage(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, int ticket, TALK_MSG_TYPE msgType, const string& message);
	virtual void OnRecvEmotion(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, int ticket, TALK_MSG_TYPE msgType, const string& emotionId);
	virtual void OnRecvVoice(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, TALK_MSG_TYPE msgType, const string& voiceId, const string& fileType, int timeLen);
	virtual void OnRecvWarning(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, int ticket, TALK_MSG_TYPE msgType, const string& message);
	virtual void OnUpdateStatus(const string& userId, const string& server, CLIENT_TYPE clientType, USER_STATUS_TYPE statusType);
	virtual void OnUpdateTicket(const string& fromId, int ticket);
	virtual void OnRecvEditMsg(const string& fromId);
	virtual void OnRecvTalkEvent(const string& userId, TALK_EVENT_TYPE eventType);
	virtual void OnRecvTryTalkBegin(const string& toId, const string& fromId, int time);
	virtual void OnRecvTryTalkEnd(const string& userId);
	virtual void OnRecvEMFNotice(const string& fromId, TALK_EMF_NOTICE_TYPE noticeType);
	virtual void OnRecvKickOffline(KICK_OFFLINE_TYPE kickType);
	virtual void OnRecvPhoto(const string& toId, const string& fromId, const string& fromName, const string& inviteId, const string& photoId, const string& sendId, bool charge, const string& photoDesc, int ticket);
	virtual void OnRecvLadyVoiceCode(const string& voiceCode);
	virtual void OnRecvIdentifyCode(const unsigned char* data, long dataLen);
	virtual void OnRecvVideo(const string& toId, const string& fromId, const string& fromName, const string& inviteId, const string& videoId, const string& sendId, bool charge, const string& videoDesc, int ticket);

private:
	ILiveChatClient	m_client;		// LiveChat客户端

	string		m_dirPath;			// 本地缓存目录路径
	list<string>	m_ipList;		// LiveChat服务器IP列表
	int			m_port;				// LiveChat服务器端口
	string		m_webSite;			// LiveChat接口站点

	string		m_userId;			// 用户Id
	string		m_sId;				// sId
	string		m_deviceId;			// 设备唯一标识
	bool		m_riskControl;		// 风控标志（true:需要风控）
	bool		m_isRecvVideoMsg;	// 是否接收视频消息
	bool		m_isLogin;			// 是否已经登录
	bool		m_isAutoLogin;		// 是否尝试自动重登录（如断线后自动尝试重）
	long		m_getUsersHistoryMsgRequestId;	// 获取多个用户历史聊天记录的requestId
	Counter		m_msgIdBuilder;		// 消息ID生成器

	// 消息管理器
	LCTextManager*		m_textMgr;		// 文本管理器
	LCEmotionManager*	m_emotionMgr;	// 高级表情管理器
	LCVoiceManager*		m_voiceMgr;		// 语音管理器
	LCPhotoManager*		m_photoMgr;		// 图片管理器
	LCVideoManager*		m_videoMgr;		// 视频管理器

	// 用户管理器
	LCUserManager*		m_userMgr;		// 用户管理器
	LCInviteManager*	m_inviteMgr;	// 邀请管理器
	LCBlockManager*		m_blockMgr;		// 黑名单管理器
	LCContactManager*	m_contactMgr;	// 联系人管理器
};
