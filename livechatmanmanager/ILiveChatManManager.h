/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: ILiveChatManManager.h
 *   desc: LiveChat男士Manager接口类
 */

#pragma once

#include "LCUserItem.h"
#include "LCMessageItem.h"
#include "ILiveChatClientDef.h"
#include "RequestLiveChatDefine.h"
#include <Other.h>

class ILiveChatManManagerListener
{
public:
	ILiveChatManManagerListener() {};
	virtual ~ILiveChatManMangerListener() {};

public:
	// ------- message listener -------
	virtual void OnRecvEditMsg(const string& fromId) = 0;
	virtual void OnRecvMessage(const LCMessageItem* msgItem) = 0;
	virtual void OnRecvSystemMsg(const LCMessageItem* msgItem) = 0;
	virtual void OnRecvWarningMsg(const LCMessageItem* msgItem) = 0;
	virtual void OnSendMessage(LCC_ERR_TYPE errType, const string& errMsg, const LCMessageItem* msgItem) = 0;
	virtual void OnSendMessageListFail(LCC_ERR_TYPE errType, const string& errMsg, const LCMessageItem* msgItem) = 0;

	// ------- emotion listener -------
	virtual void OnGetEmotionConfig(bool success, const string& errNo, const string& errMsg, const OtherEmotionConfigItem& config) = 0;
	virtual void OnGetEmotionImage(bool success, const LCEmotionItem* item) = 0;
	virtual void OnGetEmotionPlayImage(bool success, const LCEmotionItem* item) = 0;
	virtual void OnRecvEmotion(const LCMessageItem* msgItem) = 0;
	virtual void OnSendEmotion(LCC_ERR_TYPE errType, const string& errMsg, const LCMessageItem* msgItem) = 0;

	// ------- voice listener -------
	virtual void OnGetVoice(LCC_ERR_TYPE errType, const string& errMsg, const LCMessageItem* msgItem) = 0;
	virtual void OnRecvVoice(const LCMessageItem* msgItem) = 0;
	virtual void OnSendVoice(LCC_ERR_TYPE errType, const string& errNo, const string& errMsg, const LCMessageItem* msgItem) = 0;

	// ------- photo listener -------
	virtual void OnGetPhoto(LCC_ERR_TYPE errType, const string& errNo, const string& errMsg, const LCMessageItem* msgItem) = 0;
	virtual void OnPhotoFee(bool success, const string& errNo, const string& errMsg, const LCMessageItem* msgItem) = 0;
	virtual void OnRecvPhoto(const LCMessageItem* msgItem) = 0;
	virtual void OnSendPhoto(LCC_ERR_TYPE errType, const string& errNo, const string& errMsg, const LCMessageItem* msgItem) = 0;

	// ------- video listener -------
	virtual void OnGetVideo(
			LCC_ERR_TYPE errType
			, const string& userId
			, const string& videoId
			, const string& inviteId
			, const string& videoPath
			, const LCMessageList& msgList) = 0;
	virtual void OnGetVideoPhoto(
			LCC_ERR_TYPE errType
			, const string& errNo
			, const string& errMsg
			, const string& userId
			, const string& inviteId
			, const string& videoId
			, VIDEO_PHOTO_TYPE
			, const string& filePath
			, const LCMessageList& msgList) = 0;
	virtual void OnRecvVideo(const LCMessageItem* msgItem) = 0;
	virtual void OnStartGetVideo(
			const string& userId
			, const string& videoId
			, const string& inviteId
			, const string& videoPhoto
			, const LCMessageList& msgList) = 0;
	virtual void OnVideoFee(bool success, const string& errNo, const string& errMsg, const LCMessageItem* msgItem) = 0;

	// ------- other listener -------
	virtual void OnChangeOnlineStatus(const LCUserItem* userItem) = 0;
	virtual void OnGetHistoryMessage(bool success, const string& errNo, const string& errMsg, const LCUserItem* userItem) = 0;
	virtual void OnGetUsersHistoryMessage(bool success, const string& errNo, const string& errMsg, const LCUserList& userList) = 0;
	virtual void OnGetTalkList(LCC_ERR_TYPE errType, const string& errMsg) = 0;
	virtual void OnGetUserStatus(LCC_ERR_TYPE errType, const string& errMsg, const LCUserList& userList) = 0;
	virtual void OnLogin(LCC_ERR_TYPE errType, const string& errMsg, bool isAutoLogin) = 0;
	virtual void OnLogout(LCC_ERR_TYPE errType, const string& errMsg, bool isAutoLogin) = 0;
	virtual void OnRecvEMFNotice(const string& fromId, TALK_EMF_NOTICE_TYPE noticeType) = 0;
	virtual void OnRecvKickOffline(KICK_OFFLINE_TYPE kickType) = 0;
	virtual void OnSetStatus(LCC_ERR_TYPE errType, const string& errMsg) = 0;
	virtual void OnUpdateStatus(const LCUserItem* userItem) = 0;
};

class ILiveChatManManager
{
public:
	static ILiveChatManManager* Create();
	static void Release(ILiveChatManManager* obj);

private:
	ILiveChatManManager() {}
	virtual ~ILiveChatManManager() {}

public:
	virtual void SetListener(ILiveChatManManagerListener* listener) = 0;
};
