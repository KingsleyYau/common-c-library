/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LiveChatManManager.h
 *   desc: LiveChat男士Manager
 */

#pragma once

#include "ILiveChatManManager.h"

#include <livechat/ILiveChatClient.h>
#include <livechat/Counter.h>
#include <livechat/IThreadHandler.h>

#include <manrequesthandler/RequestLiveChatController.h>
#include <manrequesthandler/RequestOtherController.h>

#include <common/map_lock.h>
#include <common/CommonFunc.h>

#include "LCUserItem.h"
#include "LCEmotionManager.h"
#include "LCPhotoManager.h"
#include "LCVideoManager.h"
//添加小高级表情管理器
#include "LCMagicIconManager.h"

#include <list>
#include <string>
using namespace std;

// 消息管理器
class LCTextManager;
class LCVoiceManager;
class LCVideoManager;
// 用户管理器
class LCUserManager;
class LCInviteManager;
class LCBlockManager;
class LCContactManager;
// LiveChat客户端
class ILiveChatClient;
// 其它
class IThreadHandler;
class IAutoLock;

class LiveChatManManager : public ILiveChatManManager
								, ILiveChatClientListener
								, IRequestLiveChatControllerCallback
								, IRequestOtherControllerCallback
								, LCEmotionManagerCallback
								, LCPhotoManagerCallback
								, LCVideoManagerCallback
                                , LCMagicIconManagerCallback
{
public:
	// 请求task定义
	typedef enum {
		REQUEST_TASK_Unknow,						// 未知请求类型
		REQUEST_TASK_GetEmotionConfig,				// 获取高级表情配置
        REQUEST_TASK_GetMagicIconConfig,            // alex获取小高级表情配置
		REQUEST_TASK_AutoRelogin,					// 执行自动重登录流程
		REQUEST_TASK_GetUsersHistoryMessage,        // 获取用户的聊天历史记录
		REQUEST_TASK_CheckCouponWithToSendUser,		// 有待发消息的用户调用检测试聊券流程
		REQUEST_TASK_SendMessageList,				// 发送指定用户的待发消息
		REQUEST_TASK_SendMessageListNoMoneyFail,	// 处理指定用户的待发消息发送不成功(余额不足)
		REQUEST_TASK_SendMessageListConnectFail,	// 处理指定用户的待发消息发送不成功(连接失败)
		REQUEST_TASK_TryUseTicket,					// 执行使用试聊券流程
		REQUEST_TASK_GetCount,						// 执行获取余额流程
		REQUEST_TASK_LoginManagerLogout,			// LoginManager注销
		REQUEST_TASK_ReleaseEmotionDownloader,		// 释放高级表情downloader
		REQUEST_TASK_ReleasePhotoDownloader,		// 释放图片downloader
		REQUEST_TASK_ReleaseVideoPhotoDownloader,	// 释放视频图片downloader
		REQUEST_TASK_ReleaseVideoDownloader,		// 释放视频downloader
        REQUEST_TASK_ReleaseMagicIconDownloader,	// 释放高级表情downloader
	} REQUEST_TASK_TYPE;
	// 请求item
	typedef struct _tagRequestItem
	{
		REQUEST_TASK_TYPE		requestType;
		unsigned long	param;
		long long	delayTime;

		_tagRequestItem()
		{
			requestType = REQUEST_TASK_Unknow;
			param = 0;
			delayTime = -1;
		}

		_tagRequestItem(const _tagRequestItem& item)
		{
			requestType = item.requestType;
			param = item.param;
			delayTime = item.delayTime;
		}

		void SetDelayTime(long milliseconds)
		{
			delayTime = getCurrentTime() + milliseconds;
		}
	} RequestItem;
	// 请求map表
	typedef map<long, void*> RequestMap;

public:
	LiveChatManManager();
	virtual ~LiveChatManManager();

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
					, ILiveChatManManagerListener* listener) override;
	// 登录
	virtual bool Login(const string& userId, const string& sid, CLIENT_TYPE clientType, const list<string>& cookies, const string& deviceId, bool isRecvVideoMsg) override;
	// 注销
	virtual bool Logout(bool isResetParam) override;
    // 重新登录
    virtual bool Relogin() override;
	// 是否已经登录
	virtual bool IsLogin() override;
    // 是否获取历史记录
    virtual bool IsGetHistory() override;

	// ---------- 会话操作 ----------
	// 检测是否可使用试聊券
	virtual bool CheckCoupon(const string& userId) override;
	// 结束会话
	virtual bool EndTalk(const string& userId) override;

    // ---------- 在线状态操作 ----------
    // 获取用户状态(多个)
    virtual bool GetUserStatus(const list<string>& userIds) override;
    
    // ---------- 获取用户信息操作 ----------
    // 获取用户信息
    virtual bool GetUserInfo(const string& userId) override;
    // 获取多个用户信息
    virtual bool GetUsersInfo(const list<string>& userList) override;
    
	// ---------- 公共操作 ----------
	// 获取指定消息Id的消息Item
	virtual LCMessageItem* GetMessageWithMsgId(const string& userId, int msgId) override;
	// 获取指定用户Id的用户item
	virtual LCUserItem* GetUserWithId(const string& userId) override;
	// 获取邀请的用户列表（使用前需要先完成GetTalkList()调用）
	virtual LCUserList GetInviteUsers() override;
	// 获取最后一个邀请用户
	virtual LCUserItem* GetLastInviteUser() override;
	// 获取在聊用户列表（使用前需要先完成GetTalkList()调用）
	virtual LCUserList GetChatingUsers() override;
    // 获取用户最后一条聊天消息
    virtual LCMessageItem* GetLastMessage(const string& userId) override;
    // 获取对方最后一条聊天消息
    virtual LCMessageItem* GetTheOtherLastMessage(const string& userId) override;

	// -------- 文本消息 --------
	// 发送文本消息
	virtual LCMessageItem* SendTextMessage(const string& userId, const string& message) override;
	// 获取单个用户历史聊天记录（包括文本、高级表情、语音、图片）
	virtual bool GetHistoryMessage(const string& userId) override;
	// 删除历史消息记录
	virtual bool RemoveHistoryMessage(const string& userId, int msgId) override;
	// 插入历史消息记录
	virtual bool InsertHistoryMessage(const string& userId, LCMessageItem* msgItem) override;
	// 获取消息处理状态
	virtual LCMessageItem::StatusType GetMessageItemStatus(const string& userId, int msgId) override;

	// --------- 高级表情消息 --------
	// 发送高级表情
	virtual LCMessageItem* SendEmotion(const string& userId, const string& emotionId) override;
	// 获取高级表情配置item
	virtual OtherEmotionConfigItem GetEmotionConfigItem() const override;
	// 获取高级表情item
	virtual LCEmotionItem* GetEmotionInfo(const string& emotionId) override;
	// 手动下载/更新高级表情图片文件
	virtual bool GetEmotionImage(const string& emotionId) override;
	// 手动下载/更新高级表情图片文件
	virtual bool GetEmotionPlayImage(const string& emotionId) override;

	// ---------- 语音消息 ----------
	// 发送语音（包括获取语音验证码(livechat)、上传语音文件(livechat)、发送语音(livechat)）
	virtual LCMessageItem* SendVoice(const string& userId, const string& voicePath, const string& fileType, int timeLength) override;
	// 获取语音（包括下载语音(livechat)）
	virtual bool GetVoice(const string& userId, int msgId) override;

	// ---------- 图片消息 ----------
	// 发送图片（包括上传图片文件(php)、发送图片(livechat)）
	virtual LCMessageItem* SendPhoto(const string& userId, const string& photoPath) override;
	// 购买图片（包括付费购买图片(php)）
	//virtual bool PhotoFee(const string& userId, int msgId);
    virtual bool PhotoFee(const string& userId, const string& photoId) override;
	// 根据消息ID获取图片(模糊或清晰)（包括获取/下载对方私密照片(php)、显示图片(livechat)）
    virtual bool GetPhoto(const string& userId, const string& photoId, GETPHOTO_PHOTOSIZE_TYPE sizeType, LCMessageItem::SendType sendType) override;

	// --------- 视频消息 --------
	// 获取微视频图片
	virtual bool GetVideoPhoto(const string& userId, const string& videoId, const string& inviteId) override;
	// 购买微视频
	virtual bool VideoFee(const string& userId, int msgId) override;
	// 获取微视频播放文件
	virtual bool GetVideo(const string& userId, const string& videoId, const string& inviteId, const string& videoUrl) override;
	// 获取视频当前下载状态
	virtual bool IsGetingVideo(const string& videoId) override;
	// 获取视频图片文件路径（仅文件存在）
	virtual string GetVideoPhotoPathWithExist(const string& userId, const string& inviteId, const string& videoId, VIDEO_PHOTO_TYPE type) override;
	// 获取视频文件路径（仅文件存在）
	virtual string GetVideoPathWithExist(const string& userId, const string& inviteId, const string& videoId) override;
    
    // --------- 小高级表情消息（小高表） --------
    // 发送小高级表情
    virtual LCMessageItem* SendMagicIcon(const string& userId, const string& iconId) override;
    // 获取小高级表情配置item
    virtual MagicIconConfig GetMagicIconConfigItem() const override;
    // 获取小高级表情item
    virtual LCMagicIconItem* GetMagicIconInfo(const string& magicIconId) override;
    // 手动下载／更新小高级表情原图
    virtual bool GetMagicIconSrcImage(const string& magicIconId) override;
    // 手动下载／更新小高级表情拇子图
    virtual bool GetMagicIconThumbImage(const string& magicIconId) override;
private:
	// 获取log路径
	string GetLogPath();
	// 设置本地缓存目录路径
	bool SetDirPath(const string& path);
	// 设置http认证帐号
	bool SetAuthorization(const string& user, const string& password);
	// 清除所有图片、视频等资源文件
	void RemoveSourceFile();
	// 重置参数（用于非自动登录时重置参数）
	void ResetParamWithNotAutoLogin();
	// 重置参数（用于自动登录时重置参数）
	void ResetParamWithAutoLogin();
	// 是否自动重登录
	bool IsAutoRelogin(LCC_ERR_TYPE err);
	// 插入重登录任务
	void InsertReloginTask();
	// 自动重登录
	void AutoRelogin();
	// 设置用户在线状态，若用户在线状态改变则callback通知listener
	void SetUserOnlineStatus(LCUserItem* userItem, USER_STATUS_TYPE statusType);
    // 根据错误类型设置用户在线状态，若用户在线状态改变则callback通知listener
    void SetUserOnlineStatusWithLccErrType(LCUserItem* userItem, LCC_ERR_TYPE errType);
	// 是否处理发送操作
	bool IsHandleSendOpt();
	// 是否立即发送消息给用户
	bool IsSendMessageNow(LCUserItem* userItem);
    // 检测试聊券，或者马上发送消息
    bool SendMsgWithCoupon(LCUserItem* userItem, LCMessageItem* msgItem);
    // 是否检测试聊券
    bool IsCheckCoupon(LCUserItem* userItem);
	// 发送待发消息列表
	void SendMessageList(LCUserItem* userItem);
	// 发送待发消息列表处理
	void SendMessageListProc(LCUserItem* userItem);
	// 返回待发送消息错误处理
	void SendMessageListFailProc(LCUserItem* userItem, LCC_ERR_TYPE errType);
	// 发送消息item
	void SendMessageItem(LCMessageItem* item);
	// 获取多个用户历史聊天记录（包括文本、高级表情、语音、图片）
	bool GetUsersHistoryMessage(const list<string>& userIds);
	// --------- 文本消息 --------
	// 发送文本消息处理
	void SendTextMessageProc(LCMessageItem* item);
	// --------- 警告消息 --------
	// 根据错误类型生成警告消息
	void BuildAndInsertWarningWithErrType(const string& userId, LCC_ERR_TYPE errType);
	// 生成警告消息
	void BuildAndInsertWarningMsg(const string& userId, LCWarningItem::CodeType codeType);
	// --------- 系统消息 --------
	// 生成系统消息
	void BuildAndInsertSystemMsg(const string& userId, LCSystemItem::CodeType codeType);
	// --------- 高级表情消息 --------
	// 获取高级表情配置
	bool GetEmotionConfig();
	// 发送高级表情消息处理
	void SendEmotionProc(LCMessageItem* item);
	// --------- 语音消息 --------
	// 发送语音处理
	void SendVoiceProc(LCMessageItem* item);
	// --------- 图片消息 --------
	// 发送图片处理
	void SendPhotoProc(LCMessageItem* item);
	// 购买图片（包括付费购买图片(php)）
	bool PhotoFee(LCMessageItem* item);
	// 获取图片(模糊或清晰)（包括获取/下载对方私密照片(php)、显示图片(livechat)）
	bool GetPhoto(LCMessageItem* item, GETPHOTO_PHOTOSIZE_TYPE sizeType);
    // --------- 小高级表情消息 --------
    // 获取小高级表情配置
    bool GetMagicIconConfig();
    // 发送小高级表情消息处理
    void SendMagicIconProc(LCMessageItem* item);

	// ------------------- LCEmotionManagerCallback -------------------
private:
	virtual void OnDownloadEmotionImage(bool result, LCEmotionItem* emotionItem) override;
	virtual void OnDownloadEmotionPlayImage(bool result, LCEmotionItem* emotionItem) override;

	// ------------------- LCPhotoManagerCallback -------------------
private:
	virtual void OnDownloadPhoto(bool success, GETPHOTO_PHOTOSIZE_TYPE sizeType, const string& errnum, const string& errmsg, const LCMessageList& msgList) override;

	// ------------------- LCVideoManagerCallback -------------------
private:
		// 视频图片下载完成回调
	virtual void OnDownloadVideoPhoto(
					bool success
					, const string& errnum
					, const string& errmsg
					, const string& womanId
					, const string& inviteId
					, const string& videoId
					, VIDEO_PHOTO_TYPE type
					, const string& filePath
					, const LCMessageList& msgList) override;
	// 视频下载完成回调
	virtual void OnDownloadVideo(bool success, const string& userId, const string& videoId, const string& inviteId, const string& filePath, const LCMessageList& msgList) override;

    // ------------------- LCMagicIconManagerCallback -------------------
    // 小高级表情原图下载完成回调
    virtual void OnDownloadMagicIconImage(bool result, LCMagicIconItem* magicIconItem) override;
    // 小高级表情拇子图下载完成回调
    virtual void OnDownloadMagicIconThumbImage(bool result, LCMagicIconItem* magicIconItem) override;
    
	// ------------------- ILiveChatClientListener -------------------
private:
	// 客户端主动请求
	virtual void OnLogin(LCC_ERR_TYPE err, const string& errmsg) override;
	virtual void OnLogout(LCC_ERR_TYPE err, const string& errmsg) override;
	virtual void OnSetStatus(LCC_ERR_TYPE err, const string& errmsg) override;
	virtual void OnEndTalk(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg) override;
	virtual void OnGetUserStatus(const UserIdList& inList, LCC_ERR_TYPE err, const string& errmsg, const UserStatusList& userList) override;
	virtual void OnGetTalkInfo(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const string& userId, const string& invitedId, bool charge, unsigned int chatTime) override;
	virtual void OnSendTextMessage(const string& inUserId, const string& inMessage, int inTicket, LCC_ERR_TYPE err, const string& errmsg) override;
	virtual void OnSendEmotion(const string& inUserId, const string& inEmotionId, int inTicket, LCC_ERR_TYPE err, const string& errmsg) override;
	virtual void OnSendVGift(const string& inUserId, const string& inGiftId, int inTicket, LCC_ERR_TYPE err, const string& errmsg) override;
	virtual void OnGetVoiceCode(const string& inUserId, int ticket, LCC_ERR_TYPE err, const string& errmsg, const string& voiceCode) override;
	virtual void OnSendVoice(const string& inUserId, const string& inVoiceId, int inTicket, LCC_ERR_TYPE err, const string& errmsg) override;
	virtual void OnUseTryTicket(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const string& userId, TRY_TICKET_EVENT tickEvent) override;
	virtual void OnGetTalkList(int inListType, LCC_ERR_TYPE err, const string& errmsg, const TalkListInfo& talkListInfo) override;
	virtual void OnSendPhoto(LCC_ERR_TYPE err, const string& errmsg, int ticket) override;
	virtual void OnSendLadyPhoto(LCC_ERR_TYPE err, const string& errmsg, int ticket) override;
	virtual void OnShowPhoto(LCC_ERR_TYPE err, const string& errmsg, int ticket) override;
	virtual void OnGetUserInfo(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const UserInfoItem& userInfo) override ;
	virtual void OnGetUsersInfo(LCC_ERR_TYPE err, const string& errmsg, int seq, const list<string>& userIdList, const UserInfoList& userList) override;;
	virtual void OnGetContactList(CONTACT_LIST_TYPE inListType, LCC_ERR_TYPE err, const string& errmsg, const TalkUserList& userList) override;
	virtual void OnGetBlockUsers(LCC_ERR_TYPE err, const string& errmsg, const list<string>& users) override;
	virtual void OnSearchOnlineMan(LCC_ERR_TYPE err, const string& errmsg, const list<string>& userList) override;
	virtual void OnReplyIdentifyCode(LCC_ERR_TYPE err, const string& errmsg) override;
	virtual void OnGetRecentContactList(LCC_ERR_TYPE err, const string& errmsg, const list<string>& userList) override;
	virtual void OnGetFeeRecentContactList(LCC_ERR_TYPE err, const string& errmsg, const list<string>& userList) override;
	virtual void OnGetLadyChatInfo(LCC_ERR_TYPE err, const string& errmsg, const list<string>& chattingList, const list<string>& chattingInviteIdList, const list<string>& missingList, const list<string>& missingInviteIdList) override;
	virtual void OnPlayVideo(LCC_ERR_TYPE err, const string& errmsg, int ticket) override;
	virtual void OnSendLadyVideo(LCC_ERR_TYPE err, const string& errmsg, int ticket) override;
	virtual void OnGetLadyCondition(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const LadyConditionItem& item) override;
	virtual void OnGetLadyCustomTemplate(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const vector<string>& contents, const vector<bool>& flags) override;
	virtual void OnSendMagicIcon(const string& inUserId, const string& inIconId, int inTicket, LCC_ERR_TYPE err, const string& errmsg) override;
	virtual void OnGetPaidTheme(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const ThemeInfoList& themeList) override;
	virtual void OnGetAllPaidTheme(LCC_ERR_TYPE err, const string& errmsg, const ThemeInfoList& themeInfoList) override;
	virtual void OnManFeeTheme(const string& inUserId, const string& inThemeId, LCC_ERR_TYPE err, const string& errmsg, const ThemeInfoItem& item) override;
	virtual void OnManApplyTheme(const string& inUserId, const string& inThemeId, LCC_ERR_TYPE err, const string& errmsg, const ThemeInfoItem& item) override;
	virtual void OnPlayThemeMotion(const string& inUserId, const string& inThemeId, LCC_ERR_TYPE err, const string& errmsg, bool success) override;
	// 服务器主动请求
	virtual void OnRecvMessage(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, int ticket, TALK_MSG_TYPE msgType, const string& message,INVITE_TYPE inviteType) override;
	virtual void OnRecvEmotion(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, int ticket, TALK_MSG_TYPE msgType, const string& emotionId) override;
	virtual void OnRecvVoice(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, TALK_MSG_TYPE msgType, const string& voiceId, const string& fileType, int timeLen) override;
	virtual void OnRecvWarning(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, int ticket, TALK_MSG_TYPE msgType, const string& message) override;
	virtual void OnUpdateStatus(const string& userId, const string& server, CLIENT_TYPE clientType, USER_STATUS_TYPE statusType) override;
	virtual void OnUpdateTicket(const string& fromId, int ticket) override;
	virtual void OnRecvEditMsg(const string& fromId) override;
	virtual void OnRecvTalkEvent(const string& userId, TALK_EVENT_TYPE eventType) override;
	virtual void OnRecvTryTalkBegin(const string& toId, const string& fromId, int time) override;
	virtual void OnRecvTryTalkEnd(const string& userId) override;
	virtual void OnRecvEMFNotice(const string& fromId, TALK_EMF_NOTICE_TYPE noticeType) override;
	virtual void OnRecvKickOffline(KICK_OFFLINE_TYPE kickType) override;
	virtual void OnRecvPhoto(const string& toId, const string& fromId, const string& fromName, const string& inviteId, const string& photoId, const string& sendId, bool charge, const string& photoDesc, int ticket) override;
	virtual void OnRecvShowPhoto(const string& toId, const string& fromId, const string& fromName, const string& inviteId, const string& photoId, const string& sendId, bool charge, const string& photoDec, int ticket) override;
	virtual void OnRecvLadyVoiceCode(const string& voiceCode) override;
	virtual void OnRecvIdentifyCode(const unsigned char* data, long dataLen) override;
	virtual void OnRecvVideo(const string& toId, const string& fromId, const string& fromName, const string& inviteId, const string& videoId, const string& sendId, bool charge, const string& videoDesc, int ticket) override;
	virtual void OnRecvShowVideo(const string& toId, const string& fromId, const string& fromName, const string& inviteId, const string& videoId, const string& sendId, bool charge, const string& videoDec, int ticket) override;
	virtual void OnRecvAutoInviteMsg(const string& womanId, const string& manId, const string& key) override;
	virtual void OnRecvAutoChargeResult(const string& manId, double money, TAUTO_CHARGE_TYPE type, bool result, const string& code, const string& msg) override;
	virtual void OnRecvMagicIcon(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, int ticket, TALK_MSG_TYPE msgType, const string& iconId) override;
	virtual void OnRecvThemeMotion(const string& themeId, const string& manId, const string& womanId) override;
	virtual void OnRecvThemeRecommend(const string& themeId, const string& manId, const string& womanId) override;

	// ------------------- IRequestLiveChatControllerCallback -------------------
private:
	virtual void OnCheckCoupon(long requestId, bool success, const Coupon& item, const string& userId, const string& errnum, const string& errmsg) override;
	virtual void OnUseCoupon(long requestId, bool success, const string& errnum, const string& errmsg, const string& userId, const string& couponid) override;
	virtual void OnQueryChatRecord(long requestId, bool success, int dbTime, const list<Record>& recordList, const string& errnum, const string& errmsg, const string& inviteId) override;
	virtual void OnQueryChatRecordMutiple(long requestId, bool success, int dbTime, const list<RecordMutiple>& recordMutiList, const string& errnum, const string& errmsg) override;
	virtual void OnSendPhoto(long requestId, bool success, const string& errnum, const string& errmsg, const LCSendPhotoItem& item) override;
	virtual void OnPhotoFee(long requestId, bool success, const string& errnum, const string& errmsg) override;
	virtual void OnGetPhoto(long requestId, bool success, const string& errnum, const string& errmsg, const string& filePath) override;
	virtual void OnUploadVoice(long requestId, bool success, const string& errnum, const string& errmsg, const string& voiceId) override;
	virtual void OnPlayVoice(long requestId, bool success, const string& errnum, const string& errmsg, const string& filePath) override;
	virtual void OnGetVideoPhoto(long requestId, bool success, const string& errnum, const string& errmsg, const string& filePath) override;
	virtual void OnGetVideo(long requestId, bool success, const string& errnum, const string& errmsg, const string& url) override;
    //获取小高级表情配置回调 alex 2016-09-09
    virtual void OnGetMagicIconConfig(long requestId, bool success, const string& errnum, const string& errmsg,const MagicIconConfig& config) override;
    

	// ------------------- IRequestOtherControllerCallback -------------------
private:
	virtual void OnEmotionConfig(long requestId, bool success, const string& errnum, const string& errmsg, const OtherEmotionConfigItem& item) override;
    virtual void OnGetCount(long requestId, bool success, const string& errnum, const string& errmsg, const OtherGetCountItem& item) override;
	// ------------------- 请求线程 -------------------
private:
	static TH_RETURN_PARAM RequestThread(void* obj);
	void RequestThreadProc();
	bool StartRequestThread();
	void StopRequestThread();
	// 请求队列操作函数
	bool IsRequestQueueEmpty();
	RequestItem PopRequestTask();
	bool PushRequestTask(const RequestItem& task);
	void CleanRequestTask();
	void InsertRequestTask(REQUEST_TASK_TYPE requestType, unsigned long param = 0, long delayTime = -1);
	// 请求参数map表操作函数
	bool PushRequestMap(long requestId, void* param);
	void* RemoveRequestMap(long requestId);
	void CleanRequestMap();
	// 请求处理函数
	bool CheckCouponProc(LCUserItem* userItem);
	bool UseTryTicketProc(LCUserItem* userItem);
	bool GetCountProc(LCUserItem* userItem);

private:
	ILiveChatClient*	m_client;		// LiveChat客户端
	ILiveChatManManagerListener*	m_listener;	// 回调

	string		m_dirPath;			// 本地缓存目录路径
	list<string>	m_ipList;		// LiveChat服务器IP列表(从同步配置获取)
	int			m_port;				// LiveChat服务器端口(从同步配置获取)
	int			m_siteType;			// 站点ID
	string 		m_appVer;			// app的版本号
	double			m_minBalance;		// 最少可以聊天的点数(从同步配置获取)

	string		m_userId;			// 用户Id
	string		m_sId;				// sId
    CLIENT_TYPE   m_clientType;   // 设备类型
	list<string>	m_cookies;		// cookies
	string		m_deviceId;			// 设备唯一标识
	bool		m_riskControl;		// 风控标志（true:需要风控）
	bool		m_isRecvVideoMsg;	// 是否接收视频消息
	bool		m_isLogin;			// 是否已经登录
    bool        m_isGetHistory;     // 是否获取历史记录
    bool        m_isResetParam;     // 是否重置参数
	bool		m_isAutoLogin;		// 是否尝试自动重登录（如断线后自动尝试重）
	long		m_getUsersHistoryMsgRequestId;	// 获取多个用户历史聊天记录的requestId
	Counter		m_msgIdBuilder;		// 消息ID生成器

	// 消息管理器
	LCTextManager*		m_textMgr;		// 文本管理器
	LCEmotionManager*	m_emotionMgr;	// 高级表情管理器
	LCVoiceManager*		m_voiceMgr;		// 语音管理器
	LCPhotoManager*		m_photoMgr;		// 图片管理器
	LCVideoManager*		m_videoMgr;		// 视频管理器
    //添加小高级管理器 alex 2016－09-09
    LCMagicIconManager* m_magicIconMgr; //小高级表情管理器

	// 用户管理器
	LCUserManager*		m_userMgr;		// 用户管理器
	LCInviteManager*	m_inviteMgr;	// 邀请管理器
	LCBlockManager*		m_blockMgr;		// 黑名单管理器
	LCContactManager*	m_contactMgr;	// 联系人管理器

	// 请求线程
	list<RequestItem>	m_requestQueue;	// 请求队列
	IAutoLock*	m_requestQueueLock;		// 请求队列锁
	RequestMap	m_requestMap;			// 请求参数map表
	IAutoLock*	m_requestMapLock;		// 请求参数map表锁
	IThreadHandler*	m_requestThread;	// 请求线程
	bool		m_requestThreadStart;	// 请求线程启动标记
	HttpRequestManager*	m_httpRequestManager;			// 请求管理器
	HttpRequestHostManager* m_httpRequestHostManager;	// 请求host管理器
	RequestLiveChatController* m_requestController;		// LiveChat请求控制器
	RequestOtherController*	m_requestOtherController;	// Other请求控制器
	map_lock<long, long>	m_checkCouponOptMap;		// 检测试聊请求map表
    
    map_lock<long, LCUserItem*> m_inviteMsgMap;
};
