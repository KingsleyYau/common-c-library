/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: ILiveChatManManager.h
 *   desc: LiveChat男士Manager接口类
 */

#pragma once

#include "LCUserItem.h"
#include "LCMessageItem.h"
#include <livechat/ILiveChatClientDef.h>
#include <manrequesthandler/RequestLiveChatDefine.h>
#include <manrequesthandler/item/Other.h>
#include <manrequesthandler/item/MagicIconConfig.h>

class ILiveChatManManagerListener
{
public:
	ILiveChatManManagerListener() {};
	virtual ~ILiveChatManManagerListener() {};

public:
	// ------- login/logout listener -------
	virtual void OnLogin(LCC_ERR_TYPE errType, const string& errMsg, bool isAutoLogin) = 0;
	virtual void OnLogout(LCC_ERR_TYPE errType, const string& errMsg, bool isAutoLogin) = 0;
	
	// ------- online status listener -------
	virtual void OnRecvKickOffline(KICK_OFFLINE_TYPE kickType) = 0;
	virtual void OnSetStatus(LCC_ERR_TYPE errType, const string& errMsg) = 0;
	virtual void OnChangeOnlineStatus(LCUserItem* userItem) = 0;
	virtual void OnGetUserStatus(LCC_ERR_TYPE errType, const string& errMsg, const LCUserList& userList) = 0;
	virtual void OnUpdateStatus(LCUserItem* userItem) = 0;
    
    // ------- user info listener -------
    virtual void OnGetUserInfo(const string& userId, LCC_ERR_TYPE errType, const string& errMsg, const UserInfoItem& userInfo) = 0;
    virtual void OnGetUsersInfo(LCC_ERR_TYPE errType, const string& errMsg, const UserInfoList& userList) = 0;
	
	// ------- chat status listener -------
	virtual void OnGetTalkList(LCC_ERR_TYPE errType, const string& errMsg) = 0;
	virtual void OnEndTalk(LCC_ERR_TYPE errType, const string& errMsg, LCUserItem* userItem) = 0;
	virtual void OnRecvTalkEvent(LCUserItem* userItem) = 0;
	
	// ------- notice listener -------
	virtual void OnRecvEMFNotice(const string& fromId, TALK_EMF_NOTICE_TYPE noticeType) = 0;
	
	// ------- try ticket listener -------
	virtual void OnCheckCoupon(bool success, const string& errNo, const string& errMsg, const string& userId, CouponStatus status) = 0;
	virtual void OnRecvTryTalkBegin(LCUserItem* userItem, int time) = 0;
	virtual void OnRecvTryTalkEnd(LCUserItem* userItem) = 0;
	virtual void OnUseTryTicket(LCC_ERR_TYPE err, const string& errmsg, const string& userId, TRY_TICKET_EVENT tickEvent) = 0;	

	// ------- message listener -------
	virtual void OnRecvEditMsg(const string& fromId) = 0;
	virtual void OnRecvMessage(LCMessageItem* msgItem) = 0;
	virtual void OnRecvSystemMsg(LCMessageItem* msgItem) = 0;
	virtual void OnRecvWarningMsg(LCMessageItem* msgItem) = 0;
	virtual void OnSendTextMessage(LCC_ERR_TYPE errType, const string& errMsg, LCMessageItem* msgItem) = 0;
	virtual void OnSendMessageListFail(LCC_ERR_TYPE errType, const LCMessageList& msgList) = 0;
	virtual void OnGetHistoryMessage(bool success, const string& errNo, const string& errMsg, LCUserItem* userItem) = 0;
	virtual void OnGetUsersHistoryMessage(bool success, const string& errNo, const string& errMsg, const LCUserList& userList) = 0;

	// ------- emotion listener -------
	virtual void OnGetEmotionConfig(bool success, const string& errNo, const string& errMsg, const OtherEmotionConfigItem& config) = 0;
	virtual void OnGetEmotionImage(bool success, const LCEmotionItem* item) = 0;
	virtual void OnGetEmotionPlayImage(bool success, const LCEmotionItem* item) = 0;
	virtual void OnRecvEmotion(LCMessageItem* msgItem) = 0;
	virtual void OnSendEmotion(LCC_ERR_TYPE errType, const string& errMsg, LCMessageItem* msgItem) = 0;

	// ------- voice listener -------
	virtual void OnGetVoice(LCC_ERR_TYPE errType, const string& errNo, const string& errMsg, LCMessageItem* msgItem) = 0;
	virtual void OnRecvVoice(LCMessageItem* msgItem) = 0;
	virtual void OnSendVoice(LCC_ERR_TYPE errType, const string& errNo, const string& errMsg, LCMessageItem* msgItem) = 0;

	// ------- photo listener -------
	virtual void OnGetPhoto(GETPHOTO_PHOTOSIZE_TYPE sizeType, LCC_ERR_TYPE errType, const string& errNo, const string& errMsg, const LCMessageList& msglist) = 0;
	virtual void OnPhotoFee(bool success, const string& errNo, const string& errMsg, LCMessageItem* msgItem) = 0;
	virtual void OnRecvPhoto(LCMessageItem* msgItem) = 0;
	virtual void OnSendPhoto(LCC_ERR_TYPE errType, const string& errNo, const string& errMsg, LCMessageItem* msgItem) = 0;

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
	virtual void OnRecvVideo(LCMessageItem* msgItem) = 0;
	virtual void OnVideoFee(bool success, const string& errNo, const string& errMsg, LCMessageItem* msgItem) = 0;
    
    //------- magicIcon listener -------
	// 小高级表情配置的回调（在Onlogin 更新／获取小高级表情配置）
    virtual void OnGetMagicIconConfig(bool success, const string& errNo, const string& errMsg, const MagicIconConfig& config) = 0;
    // 手动下载／更新小高级表情原图下载
	virtual void OnGetMagicIconSrcImage(bool success, const LCMagicIconItem* item) = 0;
	// 手动下载／更新小高级表情拇子图
    virtual void OnGetMagicIconThumbImage(bool success, const LCMagicIconItem* item) = 0;
	// 发送小高级表情
    virtual void OnSendMagicIcon(LCC_ERR_TYPE errType, const string& errMsg, LCMessageItem* msgItem) = 0;
	// 接收小高级表情
    virtual void OnRecvMagicIcon(LCMessageItem* msgItem) = 0;
};

class HttpRequestManager;
class ILiveChatManManager
{
public:
	static ILiveChatManManager* Create();
	static void Release(ILiveChatManManager* obj);

protected:
	ILiveChatManManager() {}
	virtual ~ILiveChatManManager() {}

public:
	// -------- 初始化/登录/注销 --------
	// 初始化
	virtual bool Init(list<string> ipList
					, int port
					, OTHER_SITE_TYPE siteType
					, const string& webHost
					, const string& appHost
					, const string& chatVoiceHost
					, const string& httpUser
					, const string& httpPassword
					, const string& appVer
					, const string& path
					, double minBalance
					, ILiveChatManManagerListener* listener) = 0;
	// 登录
	virtual bool Login(const string& userId, const string& sid, CLIENT_TYPE clientType, const list<string>& cookies, const string& deviceId, bool isRecvVideoMsg) = 0;
	// 注销
	virtual bool Logout(bool isResetParam) = 0;
    // 重新登录
    virtual bool Relogin() = 0;
	// 是否已经登录
	virtual bool IsLogin() = 0;
    // 是否获取历史记录
    virtual bool IsGetHistory() = 0;

	// ---------- 会话操作 ----------
	// 检测是否可使用试聊券
	virtual bool CheckCoupon(const string& userId) = 0;
	// 结束会话
	virtual bool EndTalk(const string& userId) = 0;
    
    // ---------- 在线状态操作 ----------
    // 获取用户状态(多个)
    virtual bool GetUserStatus(const list<string>& userIds) = 0;
    
    // ---------- 获取用户信息操作 ----------
    // 获取用户信息
    virtual bool GetUserInfo(const string& userId) = 0;
    // 获取多个用户信息
    virtual bool GetUsersInfo(const list<string>& userList) = 0;

	// ---------- 公共操作 ----------
	// 获取指定消息Id的消息Item
	virtual LCMessageItem* GetMessageWithMsgId(const string& userId, int msgId) = 0;
	// 获取指定用户Id的用户item
	virtual LCUserItem* GetUserWithId(const string& userId) = 0;
	// 获取邀请的用户列表（使用前需要先完成GetTalkList()调用）
	virtual LCUserList GetInviteUsers() = 0;
	// 获取最后一个邀请用户
	virtual LCUserItem* GetLastInviteUser() = 0;
	// 获取在聊用户列表（使用前需要先完成GetTalkList()调用）
	virtual LCUserList GetChatingUsers() = 0;
    // 获取用户最后一条聊天消息
    virtual LCMessageItem* GetLastMessage(const string& userId) = 0;
    // 获取对方最后一条聊天消息
    virtual LCMessageItem* GetTheOtherLastMessage(const string& userId) = 0;

	// -------- 文本消息 --------
	// 发送文本消息
	virtual LCMessageItem* SendTextMessage(const string& userId, const string& message) = 0;
	// 获取单个用户历史聊天记录（包括文本、高级表情、语音、图片）
	virtual bool GetHistoryMessage(const string& userId) = 0;
	// 删除历史消息记录
	virtual bool RemoveHistoryMessage(const string& userId, int msgId) = 0;
	// 插入历史消息记录
	virtual bool InsertHistoryMessage(const string& userId, LCMessageItem* msgItem) = 0;
	// 获取消息处理状态
	virtual LCMessageItem::StatusType GetMessageItemStatus(const string& userId, int msgId) = 0;

	// --------- 高级表情消息 --------
	// 发送高级表情
	virtual LCMessageItem* SendEmotion(const string& userId, const string& emotionId) = 0;
	// 获取高级表情配置item
	virtual OtherEmotionConfigItem GetEmotionConfigItem() const = 0;
	// 获取高级表情item
	virtual LCEmotionItem* GetEmotionInfo(const string& emotionId) = 0;
	// 手动下载/更新高级表情图片文件
	virtual bool GetEmotionImage(const string& emotionId) = 0;
	// 手动下载/更新高级表情图片文件
	virtual bool GetEmotionPlayImage(const string& emotionId) = 0;

	// ---------- 语音消息 ----------
	// 发送语音（包括获取语音验证码(livechat)、上传语音文件(livechat)、发送语音(livechat)）
	virtual LCMessageItem* SendVoice(const string& userId, const string& voicePath, const string& fileType, int timeLength) = 0;
	// 获取语音（包括下载语音(livechat)）
	virtual bool GetVoice(const string& userId, int msgId) = 0;

	// ---------- 图片消息 ----------
	// 发送图片（包括上传图片文件(php)、发送图片(livechat)）
	virtual LCMessageItem* SendPhoto(const string& userId, const string& photoPath) = 0;
	// 购买图片（包括付费购买图片(php)）
	//virtual bool PhotoFee(const string& userId, int msgId) = 0;
    // 购买图片（包括付费购买图片(php)）
    virtual bool PhotoFee(const string& userId, const string& photoId) = 0;
	// 根据消息ID获取图片(模糊或清晰)（包括获取/下载对方私密照片(php)、显示图片(livechat)）    
    virtual bool GetPhoto(const string& userId, const string& photoId, GETPHOTO_PHOTOSIZE_TYPE sizeType, LCMessageItem::SendType sendType) = 0;

	// --------- 视频消息 --------
	// 获取微视频图片
	virtual bool GetVideoPhoto(const string& userId, const string& videoId, const string& inviteId) = 0;
	// 购买微视频
	virtual bool VideoFee(const string& userId, int msgId) = 0;
	// 获取微视频播放文件
	virtual bool GetVideo(const string& userId, const string& videoId, const string& inviteId, const string& videoUrl) = 0;
	// 获取视频当前下载状态
	virtual bool IsGetingVideo(const string& videoId) = 0;
	// 获取视频图片文件路径（仅文件存在）
	virtual string GetVideoPhotoPathWithExist(const string& userId, const string& inviteId, const string& videoId, VIDEO_PHOTO_TYPE type) = 0;
	// 获取视频文件路径（仅文件存在）
	virtual string GetVideoPathWithExist(const string& userId, const string& inviteId, const string& videoId) = 0;
    
    // --------- 小高级表情消息 --------
    // 发送小高级表情消息
    virtual LCMessageItem* SendMagicIcon(const string& userId, const string& iconId) = 0;
    // 获取小高级表情配置item
    virtual MagicIconConfig GetMagicIconConfigItem() const = 0;
    // 获取小高级表情item
    virtual LCMagicIconItem* GetMagicIconInfo(const string& magicIconId) = 0;
    // 手动下载／更新小高级表情原图
    virtual bool GetMagicIconSrcImage(const string& magicIconId) = 0;
    // 手动下载／更新小高级表情拇子图
    virtual bool GetMagicIconThumbImage(const string& magicIconId) = 0;
};
