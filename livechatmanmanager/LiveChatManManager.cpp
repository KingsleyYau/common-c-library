/*
 * author: Samson.Fan
 *   date: 2015-03-19
 *   file: LiveChatManManager.cpp
 *   desc: LiveChat客户端实现类
 */

#include "LiveChatManManager.h"
#include <HttpRequestDefine.h>
#include <CommonFunc.h>
#include <KLog.h>

// 消息管理器
#include "LCTextManager.h"
#include "LCEmotionManager.h"
#include "LCVoiceManager.h"
#include "LCPhotoManager.h"
#include "LCVideoManager.h"

// 用户管理器
#include "LCUserManager.h"
#include "LCInviteManager.h"
#include "LCBlockManager.h"
#include "LCContactManager.h"

static const int s_msgIdBegin = 1;		// 消息ID起始值

const char* LOG_DIR = "log/";						// log目录路径
const char* EMOTION_DIR = "livechat/emotion";		// 高级表情目录路径
const char* VOICE_DIR = "livechat/voice";			// 语音目录路径
const char* PHOTO_DIR = "livechat/photo";			// 图片目录路径
const char* PHOTO_TEMP_DIR = "livechat/photo/temp";	// 图片临时目录路径
const char* VIDEO_DIR = "livechat/video";			// 视频目录路径

LiveChatManManager::LiveChatManManager()
{
	m_dirPath = "";
	m_port = 0;
	m_webSite = "";

	m_userId = "";
	m_sId = "";
	m_deviceId = "";
	m_riskControl = false;
	m_isRecvVideoMsg = true;
	m_isLogin = false;
	m_isAutoLogin = false;
	m_getUsersHistoryMsgRequestId = HTTPREQUEST_INVALIDREQUESTID;
	m_msgIdBuilder.Init(s_msgIdBegin);

	// LiveChat客户端
	m_client = ILiveChatClient::CreateClient();

	// 消息管理器
	m_textMgr = new LCTextManager;			// 文本管理器
	m_emotionMgr = new LCEmotionManager;	// 高级表情管理器
	m_voiceMgr = new LCVoiceManager;		// 语音管理器
	m_photoMgr = new LCPhotoManager;		// 图片管理器
	m_videoMgr = new LCVideoManager;		// 视频管理器

	// 用户管理器
	m_userMgr = new LCUserManager;			// 用户管理器
	m_inviteMgr = new LCInviteManager;		// 邀请管理器
	m_blockMgr = new LCBlockManager;		// 黑名单管理器
	m_contactMgr = new LCContactManager;	// 联系人管理器
}

LiveChatManManager::~LiveChatManManager()
{
	ILiveChatClient::ReleaseClient(m_client);

	delete m_textMgr;
	delete m_emotionMgr;
	delete m_voiceMgr;
	delete m_photoMgr;
	delete m_videoMgr;

	delete m_userMgr;
	delete m_inviteMgr;
	delete m_blockMgr;
	delete m_contactMgr;
}

// 设置本地缓存目录路径
bool LiveChatManManager::SetDirPath(const string& path)
{
	bool result = false;
	if (!path.empty()
		&& MakeDir(path))
	{
		result = true;

		m_dirPath = path;
		if (m_dirPath.at(m_dirPath.length()-1) != '/'
			&& m_dirPath.at(m_dirPath.length()-1) != '\\')
		{
			m_dirPath += '/';
		}

		// log目录
		string logPath = m_dirPath + LOG_DIR;
		KLog::SetLogDirectory(logPath);

		// 设置emotion manager本地缓存目录
		string emotionPath = m_dirPath + EMOTION_DIR;
		result = result && m_emotionMgr->SetDirPath(emotionPath, logPath);

		// 设置voice manager本地缓存目录
		string voicePath = m_dirPath + VOICE_DIR;
		result = result && m_voiceMgr->Init(voicePath);

		// 设置photo manager本地缓存目录
		string photoPath = m_dirPath + PHOTO_DIR;
		result = result && m_photoMgr->Init(photoPath);

		// 设置video manager本地缓存目录
		string videoPath = m_dirPath + VIDEO_DIR;
		result = result && m_videoMgr->Init(videoPath);
	}
	return result;
}

// 初始化
bool LiveChatManManager::Init(list<string> ipList, int port, const string& webHost)
{
	bool result = false;

	if (!ipList.empty()
		&& port > 0
		&& !webHost.empty())
	{
		result = m_emotionMgr->Init(webHost, this);
		result = result && m_client->Init(ipList, port, this);
	}

	if (result)
	{
		// 清除资源文件
		RemoveSourceFile();
	}

	FileLog("livechat", "LiveChatManager::Init() end, result:%b", result);
	return result;
}

// 重置参数（用于注销后或登录前）
void LiveChatManManager::ResetParam()
{
	m_userId = "";
	m_sId = "";
	m_deviceId = "";
	m_riskControl = false;
	m_isRecvVideoMsg = true;
	m_msgIdBuilder.Init(s_msgIdBegin);

	FileLog("livechat", "ResetParam() clear emotion begin");
	// 停止获取高级表情配置请求
	if (RequestJni.InvalidRequestId != m_emotionMgr->mEmotionConfigReqId) {
//			RequestJni.StopRequest(m_emotionMgr->mEmotionConfigReqId);
		m_emotionMgr->mEmotionConfigReqId = RequestJni.InvalidRequestId;
	}
	FileLog("livechat", "ResetParam() clear emotion StopAllDownloadImage");
	m_emotionMgr->StopAllDownloadImage();
	FileLog("livechat", "ResetParam() clear emotion removeAllSendingItems");
	m_emotionMgr->RemoveAllSendingItems();

	FileLog("livechat", "ResetParam() clear photo begin");
	// 停止所有图片请求
	m_photoMgr->ClearAllRequestItems();
//		ArrayList<Long> photoRequestIds = m_photoMgr->clearAllRequestItems();
//		if (null != photoRequestIds) {
//			for (Iterator<Long> iter = photoRequestIds.iterator(); iter.hasNext(); ) {
//				long requestId = iter.next();
//				RequestJni.StopRequest(requestId);
//			}
//		}
	FileLog("livechat", "ResetParam() clear photo clearAllSendingItems");
	m_photoMgr->ClearAllSendingItems();

	FileLog("livechat", "ResetParam() clear voice begin");
	// 停止所有语音请求
	m_voiceMgr->ClearAllRequestItem();
//		ArrayList<Long> voiceRequestIds = m_voiceMgr->clearAllRequestItem();
//		if (null != voiceRequestIds) {
//			for (Iterator<Long> iter = voiceRequestIds.iterator(); iter.hasNext(); ) {
//				long requestId = iter.next();
//				RequestJni.StopRequest(requestId);
//			}
//		}
	FileLog("livechat", "ResetParam() clear voice clearAllSendingItems");
	m_voiceMgr->ClearAllSendingItems();

	FileLog("livechat", "ResetParam() clear other begin");
	m_textMgr->RemoveAllSendingItems();
	FileLog("livechat", "ResetParam() clear other removeAllUserItem");
	m_userMgr->RemoveAllUserItem();
}

/**
 * 清除所有图片、视频等资源文件
 */
private void removeSourceFile()
{
	// 清除图片文件
	m_photoMgr->removeAllPhotoFile();
	// 清除视频文件
	mVideoMgr.removeAllVideoFile();
}

/**
 * 登录
 * @param userId	用户ID
 * @param password	php登录成功的session
 * @param deviceId	设备唯一标识
 * @return
 */
public synchronized bool Login(string userId, string sid, string deviceId, bool isRecvVideoMsg)
{
	FileLog("livechat", "LiveChatManager::Login() begin, userId:%s, mIsLogin:%s", userId, string.valueOf(mIsLogin));

	bool result = false;
	if (mIsLogin) {
		result = mIsLogin;
	}
	else {
		if (!mIsAutoRelogin) {
			// 重置参数
			ResetParam();
		}

		// LiveChat登录
		result = LiveChatClient.Login(userId, sid, deviceId, ClientType.CLIENT_ANDROID, UserSexType.USER_SEX_MALE);
		if (result)
		{
			mIsAutoRelogin = true;
			mUserId = userId;
			mSid = sid;
			mDeviceId = deviceId;
			mIsRecvVideoMsg = isRecvVideoMsg;
		}
	}

	FileLog("livechat", "LiveChatManager::Login() end, userId:%s, result:%s", userId, Boolean.tostring(result));
	return result;
}

/**
 * 是否自动重登录
 * @return
 */
private bool IsAutoRelogin(LiveChatErrType errType)
{
	if (mIsAutoRelogin)
	{
		mIsAutoRelogin = (errType == LiveChatErrType.ConnectFail);
	}
	return mIsAutoRelogin;
}

/**
 * 自动重登录
 */
private void AutoRelogin()
{
	FileLog("livechat", "LiveChatManager::AutoRelogin() begin, mUserId:%s, mSid:%s, mDeviceId:%s", mUserId, mSid, mDeviceId);

	if (null != mUserId && !mUserId.empty()
		&& null != mSid && !mSid.empty()
		&& null != mDeviceId && !mDeviceId.empty())
	{
		Login(mUserId, mSid, mDeviceId, mIsRecvVideoMsg);
	}

	FileLog("livechat", "LiveChatManager::AutoRelogin() end");
}

/**
 * 注销
 * @return
 */
public synchronized bool Logout()
{
	FileLog("livechat", "LiveChatManager::Logout() begin");

	// 设置不自动重登录
	mIsAutoRelogin = false;
	bool result =  LiveChatClient.Logout();

	FileLog("livechat", "LiveChatManager::Logout() end, result:%b", result);

	return result;
}

/**
 * 是否已经登录
 * @return
 */
public bool IsLogin()
{
	return mIsLogin;
}

/**
 * 是否处理发送操作
 * @return
 */
private bool IsHandleSendOpt()
{
	bool result = false;
	if (!mRiskControl
		 && mIsAutoRelogin)
	{
		// 没有风控且自动重连
		result = true;
	}
	return result;
}

/**
 * 是否立即发送消息给用户
 * @param userItem	用户item
 * @return
 */
private bool IsSendMessageNow(LCUserItem userItem)
{
	bool result = false;
	if (null != userItem)
	{
		// 已经登录及聊天状态为inchat或男士邀请
		result = IsLogin() && (userItem.chatType == ChatType.InChatCharge
				|| userItem.chatType == ChatType.InChatUseTryTicket
				|| userItem.chatType == ChatType.ManInvite);
	}
	return result;
}

/**
 * 是否等待登录后发送消息给用户
 * @param userItem	用户item
 * @return
 */
private bool IsWaitForLoginToSendMessage(LCUserItem userItem)
{
	bool result = false;
	if (null != userItem)
	{
		// 已经登录及聊天状态为inchat或男士邀请
		result = !IsLogin() && (userItem.chatType == ChatType.InChatCharge
				|| userItem.chatType == ChatType.InChatUseTryTicket
				|| userItem.chatType == ChatType.ManInvite);
	}
	return result;
}

/**
 * 设置在线状态
 * @param statusType	在线状态
 * @return
 */
public bool SetStatus(UserStatusType statusType)
{
	return LiveChatClient.SetStatus(statusType);
}

/**
 * 获取用户状态(多个)
 * @param userIds	用户ID数组
 * @return
 */
public bool GetUserStatus(string[] userIds)
{
	return LiveChatClient.GetUserStatus(userIds);
}

/**
 * 获取在聊及邀请的用户列表
 * @return
 */
private bool GetTalkList()
{
	int talkList = LiveChatClient.TalkListChating | LiveChatClient.TalkListPause | LiveChatClient.TalkListWomanInvite;
	return LiveChatClient.GetTalkList(talkList);
}

/**
 * 获取邀请的用户列表（使用前需要先完成GetTalkList()调用）
 * @return
 */
public ArrayList<LCUserItem> GetInviteUsers()
{
	return m_userMgr->getInviteUsers();
}

/**
 * 获取最后一个邀请用户
 * @return
 */
public LCUserItem GetLastInviteUser()
{
	LCUserItem userItem = null;
	ArrayList<LCUserItem> inviteUsers = m_userMgr->getInviteUsers();
	if (inviteUsers.size() > 0) {
		userItem = inviteUsers.get(0);
	}
	return userItem;
}

/**
 * 设置用户在线状态，若用户在线状态改变则callback通知listener
 * @param userItem
 * @param statusType
 */
private void SetUserOnlineStatus(LCUserItem userItem, UserStatusType statusType)
{
	if (userItem.statusType != statusType)
	{
		userItem.statusType = statusType;
		mCallbackHandler.OnChangeOnlineStatus(userItem);
	}
}

/**
 * 获取在聊用户列表（使用前需要先完成GetTalkList()调用）
 * @return
 */
public ArrayList<LCUserItem> GetChatingUsers()
{
	return m_userMgr->getChatingUsers();
}

/**
 * 检测用户帐号是否有足够点数
 * @return
 */
private bool CheckMoney(final string userId)
{
	ConfigManager.getInstance().GetOtherSynConfigItem(new OnConfigManagerCallback() {

		@Override
		public void OnGetOtherSynConfigItem(bool isSuccess, string errno,
				string errmsg, OtherSynConfigItem item) {
			// TODO Auto-generated method stub
			double money = 0.5;
			if (isSuccess) {
				if (WebSiteManager.getInstance().GetWebSite().getSiteKey().compareTo(WebSiteType.ChnLove.name()) == 0) {
					money = item.cl.minChat;
				}
				else if (WebSiteManager.getInstance().GetWebSite().getSiteKey().compareTo(WebSiteType.CharmDate.name()) == 0) {
					money = item.ch.minChat;
				}
				else if (WebSiteManager.getInstance().GetWebSite().getSiteKey().compareTo(WebSiteType.IDateAsia.name()) == 0) {
					money = item.ida.minChat;
				}
				else if (WebSiteManager.getInstance().GetWebSite().getSiteKey().compareTo(WebSiteType.LatamDate.name()) == 0) {
					money = item.la.minChat;
				}
			}
			final double minMoney = money;

			long reuqestId = RequestJniOther.GetCount(true, false, false, false, false, false, new OnOtherGetCountCallback() {

				@Override
				public void OnOtherGetCount(bool isSuccess, string errno, string errmsg,
						OtherGetCountItem item) {
					// TODO Auto-generated method stub
					if (isSuccess) {
						if (item.money >= minMoney) {
							// 若当前状态为Other，则标记为ManInvite(男士邀请)状态
							LCUserItem userItem = m_userMgr->getUserItem(userId);
							if (null != userItem) {
								if (userItem.chatType == ChatType.Other
									|| userItem.chatType == ChatType.Invite)
								{
									userItem.chatType = ChatType.ManInvite;
								}
							}
							// 余额足够，发送待发消息
							Message msg = Message.obtain();
							msg.what = LiveChatRequestOptType.SendMessageList.ordinal();
							msg.obj = userId;
							mHandler.sendMessage(msg);
						}
						else {
							// 返回发送消息余额不足，发送失败
							Message msgSendMsg = Message.obtain();
							msgSendMsg.what = LiveChatRequestOptType.SendMessageListNoMoneyFail.ordinal();
							msgSendMsg.obj = userId;
							mHandler.sendMessage(msgSendMsg);
						}
					}
					else {
						// 请求失败
						Message msgSendMsg = Message.obtain();
						msgSendMsg.what = LiveChatRequestOptType.SendMessageListConnectFail.ordinal();
						msgSendMsg.obj = userId;
						mHandler.sendMessage(msgSendMsg);
					}
				}
			});

			if (RequestJni.InvalidRequestId == reuqestId) {
				// 请求失败，返回发送消息余额不足失败
				Message msgSendMsg = Message.obtain();
				msgSendMsg.what = LiveChatRequestOptType.SendMessageListConnectFail.ordinal();
				msgSendMsg.obj = userId;
				mHandler.sendMessage(msgSendMsg);
			}
		}
	});
	return true;
}

/**
 * 查询是否能使用试聊
 * @param userId	对方用户ID
 * @return
 */
public bool CheckCoupon(string userId) {
	long requestId = RequestOperator.getInstance().CheckCoupon(userId, new OnCheckCouponCallCallback() {

		@Override
		public void OnCheckCoupon(long requestId, bool isSuccess, string errno, string errmsg,
				Coupon item) {
			// TODO Auto-generated method stub
			mCallbackHandler.OnCheckCoupon(isSuccess, errno, errmsg, item);
		}
	});
	return requestId != RequestJni.InvalidRequestId;
}

/**
 * 查询是否能使用试聊
 * @param userId	对方用户ID
 * @return
 */
private bool CheckCouponProc(LCUserItem userItem) {
	// 执行尝试使用试聊券流程
	if (!userItem.tryingSend) {
		userItem.tryingSend = true;
		long requestId = RequestOperator.getInstance().CheckCoupon(userItem.userId, new OnCheckCouponCallCallback() {

			@Override
			public void OnCheckCoupon(long requestId, bool isSuccess, string errno, string errmsg,
					Coupon item) {
				// TODO Auto-generated method stub
				if (isSuccess && item.status == CouponStatus.Yes)
				{
					// 尝试使用试聊券
					Message msg = Message.obtain();
					msg.what = LiveChatRequestOptType.TryUseTicket.ordinal();
					msg.obj = item;
					mHandler.sendMessage(msg);
				}
				else {
					// 检测是否有点
					Message msg = Message.obtain();
					msg.what = LiveChatRequestOptType.GetCount.ordinal();
					msg.obj = item;
					mHandler.sendMessage(msg);
				}
			}
		});
		return requestId != RequestJni.InvalidRequestId;
	}
	else {
		return true;
	}
}

/**
 * 使用试聊券
 * @param userId	对方用户ID
 * @return
 */
private bool UseTryTicket(final string userId) {
	bool result = false;
	if (!userId.empty()) {
		long requestId = RequestOperator.getInstance().UseCoupon(userId, new OnLCUseCouponCallback() {

			@Override
			public void OnLCUseCoupon(long requestId, bool isSuccess, string errno, string errmsg, string userId) {
				// TODO Auto-generated method stub
				if (isSuccess) {
					LiveChatClient.UseTryTicket(userId);
				}
				else {
					// 试聊券使用不成功
					mCallbackHandler.OnUseTryTicket(LiveChatErrType.Fail, errno, errmsg, userId, TryTicketEventType.Unknow);
					// 检测帐号是否有足够的点数
					Message msg = Message.obtain();
					msg.what = LiveChatRequestOptType.CheckMoney.ordinal();
					msg.obj = userId;
					mHandler.sendMessage(msg);
				}
			}
		});
		result = (requestId != RequestJni.InvalidRequestId);
	}
	return result;
}

/**
 * 结束对话
 * @param userId	对方的用户ID
 * @return
 */
public bool EndTalk(string userId)
{
	return LiveChatClient.EndTalk(userId);
}

/**
 * 获取单个用户历史聊天记录（包括文本、高级表情、语音、图片）
 * @param userId	用户ID
 * @return
 */
public bool GetHistoryMessage(string userId)
{
	bool result = false;
	LCUserItem userItem = m_userMgr->getUserItem(userId);
	if (null != userItem) {
		if (userItem.getMsgList().size() > 0
			&& mGetUsersHistoryMsgRequestId == RequestJni.InvalidRequestId) // 未完成获取多个用户历史聊天记录的请求
		{
			result = true;
			mCallbackHandler.OnGetHistoryMessage(true, "", "", userItem);
		}
		else if (!userItem.inviteId.empty())
		{
			long requestId = RequestOperator.getInstance().QueryChatRecord(userItem.inviteId, new OnQueryChatRecordCallback() {

				@Override
				public void OnQueryChatRecord(bool isSuccess, string errno,
						string errmsg, int dbTime, Record[] recordList, string inviteId)
				{
					// TODO Auto-generated method stub

					// 设置服务器当前数据库时间
					LCMessageItem.SetDbTime(dbTime);

					// 插入聊天记录
					LCUserItem userItem = m_userMgr->getUserItemWithInviteId(inviteId);
					if (isSuccess && userItem != null) {
						// 清除已完成的记录（保留未完成发送的记录）
						userItem.clearFinishedMsgList();
						// 插入历史记录
						for (int i = 0; i < recordList.length; i++)
						{
							LCMessageItem item = new LCMessageItem();
							if (item.InitWithRecord(
									mMsgIdIndex.getAndIncrement(),
									mUserId,
									userItem.userId,
									userItem.inviteId,
									recordList[i],
									mEmotionMgr,
									mVoiceMgr,
									mPhotoMgr,
									mVideoMgr))
							{
								userItem.insertSortMsgList(item);
							}
						}
						// 合并图片聊天记录
						m_photoMgr->combineMessageItem(userItem.msgList);
						// 合并视频聊天记录
						mVideoMgr.combineMessageItem(userItem.msgList);
					}
					mCallbackHandler.OnGetHistoryMessage(isSuccess, errno, errmsg, userItem);
				}
			});
			result = (requestId != RequestJni.InvalidRequestId);
		}
	}

	return result;
}

/**
 * 获取多个用户历史聊天记录（包括文本、高级表情、语音、图片）
 * @param userIds	用户ID数组
 * @return
 */
public bool GetUsersHistoryMessage(string[] userIds)
{
	bool result = false;
	ArrayList<string> inviteIds = new ArrayList<string>();
	for (int i = 0; i < userIds.length; i++) {
		if (!userIds[i].empty()) {
			LCUserItem userItem = m_userMgr->getUserItem(userIds[i]);
			if (null != userItem)
			{
				if (!userItem.inviteId.empty())
				{
					inviteIds.add(userItem.inviteId);
				}
			}
		}
	}
	if (inviteIds.size() > 0) {
		string[] inviteArray = new string[inviteIds.size()];
		inviteIds.toArray(inviteArray);
		mGetUsersHistoryMsgRequestId = RequestOperator.getInstance().QueryChatRecordMutiple(inviteArray, new OnQueryChatRecordMutipleCallback() {

			@Override
			public void OnQueryChatRecordMutiple(bool isSuccess, string errno,
					string errmsg, int dbTime, RecordMutiple[] recordMutipleList)
			{
				// TODO Auto-generated method stub
				LCUserItem[] userArray = null;
				ArrayList<LCUserItem> userList = new ArrayList<LCUserItem>();
				if (isSuccess
					&& null != recordMutipleList)
				{
					// 设置服务器当前数据库时间
					LCMessageItem.SetDbTime(dbTime);

					// 插入聊天记录
					for (int i = 0; i < recordMutipleList.length; i++)
					{
						RecordMutiple record = recordMutipleList[i];
						LCUserItem userItem = m_userMgr->getUserItemWithInviteId(record.inviteId);
						if (null != record.recordList
							&& userItem != null)
						{
							// 清除已完成的记录（保留未完成发送的记录）
							userItem.clearFinishedMsgList();
							// 服务器返回的历史消息是倒序排列的
							for (int k = record.recordList.length - 1; k >= 0; k--)
							{
								LCMessageItem item = new LCMessageItem();
								if (item.InitWithRecord(
										mMsgIdIndex.getAndIncrement(),
										mUserId,
										userItem.userId,
										userItem.inviteId,
										record.recordList[k],
										mEmotionMgr,
										mVoiceMgr,
										mPhotoMgr,
										mVideoMgr))
								{
									userItem.insertSortMsgList(item);
								}
							}

							// 合并图片聊天记录
							m_photoMgr->combineMessageItem(userItem.msgList);
							// 合并视频聊天记录
							mVideoMgr.combineMessageItem(userItem.msgList);
							// 添加到用户数组
							userList.add(userItem);
						}
					}

					userArray = new LCUserItem[userList.size()];
					userList.toArray(userArray);
				}
				mCallbackHandler.OnGetUsersHistoryMessage(isSuccess, errno, errmsg, userArray);

				// 重置ReuqestId
				mGetUsersHistoryMsgRequestId = RequestJni.InvalidRequestId;
			}
		});
		result = (mGetUsersHistoryMsgRequestId != RequestJni.InvalidRequestId);
	}

	return result;
}

/**
 * 插入历史聊天记录（msgId及createTime由LiveChatManager生成）
 * @param userId	对方用户ID
 * @param item		消息item
 * @return
 */
public bool InsertHistoryMessage(string userId, LCMessageItem item) {
	bool result = false;
	LCUserItem userItem = m_userMgr->getUserItem(userId);
	if (null != userItem) {
		result = userItem.insertSortMsgList(item);
		item.msgId = mMsgIdIndex.getAndIncrement();
		item.createTime = (int)(System.currentTimeMillis() / 1000);
	}
	else {
		Log.e("livechat", string.format("%s::%s() userId:%s is not exist", "LiveChatManager", "InsertHistoryMessage", userId));
	}

	if (!result) {
		Log.e("livechat", string.format("%s::%s() fail, userId:%s, msgId:%d", "LiveChatManager", "InsertHistoryMessage", userId, item.msgId));
	}
	return result;
}

/**
 * 删除历史消息记录
 * @param item	消息item
 */
public bool RemoveHistoryMessage(LCMessageItem item)
{
	bool result = false;
	if (null != item && null != item.getUserItem())
	{
		LCUserItem userItem = item.getUserItem();
		result = userItem.removeSortMsgList(item);
	}
	return result;
}

/**
 * 获取指定消息Id的消息Item
 * @param userId	用户ID
 * @param msgId		消息ID
 * @return
 */
public LCMessageItem GetMessageWithMsgId(string userId, int msgId) {
	LCMessageItem item = null;
	LCUserItem userItem = m_userMgr->getUserItem(userId);
	if (userItem != null) {
		item = userItem.getMsgItemWithId(msgId);
	}
	return item;
}

/**
 * 获取指定用户Id的用户item
 * @param userId	用户ID
 * @return
 */
public LCUserItem GetUserWithId(string userId) {
	LCUserItem userItem = m_userMgr->getUserItem(userId);
	return userItem;
}

/**
 * 获取消息处理状态
 * @param userId	用户ID
 * @param msgId		消息ID
 * @return
 */
public StatusType GetMessageItemStatus(string userId, int msgId)
{
	StatusType statusType = StatusType.Unprocess;
	LCMessageItem item = GetMessageWithMsgId(userId, msgId);
	if (null != item) {
		statusType = item.statusType;
	}
	return statusType;
}

/**
 * 发送待发消息列表
 * @param usreId
 */
private void SendMessageList(string userId)
{
	LCUserItem userItem = m_userMgr->getUserItem(userId);
	synchronized (userItem.sendMsgList)
	{
		userItem.tryingSend = false;
		for (LCMessageItem item : userItem.sendMsgList) {
			// 发送消息item
			SendMessageItem(item);
		}
		userItem.sendMsgList.clear();
	}
}

/**
 * 返回待发送消息错误
 * @param errType
 */
private void SendMessageListFail(final string userId, final LiveChatErrType errType)
{
	LCUserItem userItem = m_userMgr->getUserItem(userId);
	if (null != userItem) {
		synchronized (userItem.sendMsgList)
		{
			userItem.tryingSend = false;
			for (LCMessageItem item : userItem.sendMsgList)
			{
				item.statusType = StatusType.Fail;
			}

			@SuppressWarnings("unchecked")
			ArrayList<LCMessageItem> cloneMsgList = (ArrayList<LCMessageItem>)userItem.sendMsgList.clone();
			userItem.sendMsgList.clear();

			mCallbackHandler.OnSendMessageListFail(errType, cloneMsgList);
		}
	}
	else {
		Log.e("livechat", "LiveChatManager::SendMessageListFail() get user item fail");
	}
}

/**
 * 发送消息item
 * @param item	消息item
 */
private void SendMessageItem(LCMessageItem item)
{
	// 发送消息
	switch (item.msgType)
	{
	case Text:
		SendMessageProc(item);
		break;
	case Emotion:
		SendEmotionProc(item);
		break;
	case Photo:
		SendPhotoProc(item);
		break;
	case Voice:
		SendVoiceProc(item);
		break;
	default:
		Log.e("livechat", "LiveChatManager::SendMessageList() msgType error, msgType:%s", item.msgType.name());
		break;
	}
}

// ---------------- 文字聊天操作函数(message) ----------------
/**
 * 发送文本聊天消息
 * @param userId	对方的用户ID
 * @param message	消息内容
 * @param illegal	消息内容是否合法
 * @return
 */
public LCMessageItem SendMessage(string userId, string message)
{
	// 判断是否处理发送操作
	if (!IsHandleSendOpt()) {
		Log.e("livechat", "LiveChatManager::SendMessage() IsHandleSendOpt()==false");
		return null;
	}

	// 获取用户item
	LCUserItem userItem = m_userMgr->getUserItem(userId);
	if (null == userItem) {
		Log.e("livechat", string.format("%s::%s() getUserItem fail, userId:%s", "LiveChatManager", "SendPhoto", userId));
		return null;
	}

	// 构造消息item
	LCMessageItem item = null;
	if (!message.empty()) {
		// 生成MessageItem
		item = new LCMessageItem();
		item.init(mMsgIdIndex.getAndIncrement()
				, SendType.Send
				, mUserId
				, userId
				, userItem.inviteId
				, StatusType.Processing);
		// 生成TextItem
		LCTextItem textItem = new LCTextItem();
		textItem.init(message);
		// 把TextItem加到MessageItem
		item.setTextItem(textItem);
		// 添加到历史记录
		userItem.insertSortMsgList(item);

		if (IsSendMessageNow(userItem))
		{
			// 发送消息
			SendMessageProc(item);
		}
		else if (IsWaitForLoginToSendMessage(userItem))
		{
			// 登录未成功，添加到待发送列表
			userItem.sendMsgList.add(item);
		}
		else
		{
			// 正在使用试聊券，消息添加到待发列表
			userItem.sendMsgList.add(item);
			if (IsLogin()) {
				// 执行尝试使用试聊券流程
				CheckCouponProc(userItem);
			}
		}
	}
	else {
		Log.e("livechat", string.format("%s::%s() param error, userId:%s, message:%s", "LiveChatManager", "SendMessage", userId, message));
	}
	return item;
}

/**
 * 发送文本消息处理
 * @param item
 */
private void SendMessageProc(LCMessageItem item)
{
	if (LiveChatClient.SendMessage(item.toId, item.getTextItem().message, item.getTextItem().illegal, item.msgId)) {
		m_textMgr->addSendingItem(item);
	}
	else {
		item.statusType = StatusType.Fail;
		mCallbackHandler.OnSendMessage(LiveChatErrType.Fail, "", item);
	}
}

/**
 * 根据错误类型生成警告消息
 * @param userItem	用户item
 * @param errType	服务器返回错误类型
 */
private void BuildAndInsertWarningWithErrType(LCUserItem userItem, LiveChatErrType errType)
{
	if (errType == LiveChatErrType.NoMoney)
	{
		// 获取消息内容
		string message = mContext.getstring(R.string.livechat_msg_no_credit_warning);
		string linkMsg = mContext.getstring(R.string.livechat_msg_no_credit_warning_link);
		// 生成余额不足的警告消息
		BuildAndInsertWarning(userItem, message, linkMsg);
	}
}

/**
 * 生成警告消息
 * @param userItem	用户item
 * @param message	警告消息内容
 * @param linkMsg	链接内容
 */
private void BuildAndInsertWarning(LCUserItem userItem, string message, string linkMsg) {
	if (!message.empty()) {
		// 生成warning消息
		LCWarningItem warningItem = new LCWarningItem();
		if (!linkMsg.empty()) {
			LCWarningLinkItem linkItem = new LCWarningLinkItem();
			linkItem.init(linkMsg, LinkOptType.Rechange);
			warningItem.initWithLinkMsg(message, linkItem);
			warningItem.linkItem = linkItem;
		}
		// 生成message消息
		LCMessageItem item = new LCMessageItem();
		item.init(mMsgIdIndex.getAndIncrement(), SendType.System, userItem.userId, mUserId, userItem.inviteId, StatusType.Finish);
		item.setWarningItem(warningItem);
		// 插入到聊天记录列表中
		userItem.insertSortMsgList(item);
		// 回调
		mCallbackHandler.OnRecvWarning(item);
	}
}

/**
 *
 * @param userId
 * @param message
 * @return
 */
public bool BuildAndInsertSystemMsg(string userId, string message)
{
	bool result = false;
	LCUserItem userItem = m_userMgr->getUserItem(userId);
	if (null != userItem) {
		// 生成系统消息并回调
		LCSystemItem systemItem = new LCSystemItem();
		systemItem.message = message;
		LCMessageItem item = new LCMessageItem();
		item.init(mMsgIdIndex.getAndIncrement(), SendType.System, userId, mUserId, userItem.inviteId, StatusType.Finish);
		item.setSystemItem(systemItem);
		userItem.insertSortMsgList(item);
		mCallbackHandler.OnRecvSystemMsg(item);

		result = true;
	}
	return result;
}

// ---------------- 高级表情操作函数(Emotion) ----------------
/**
 * 获取高级表情配置
 */
public synchronized bool GetEmotionConfig()
{
	if (m_emotionMgr->mEmotionConfigReqId != RequestJni.InvalidRequestId) {
		return true;
	}

	m_emotionMgr->mEmotionConfigReqId = RequestOperator.getInstance().EmotionConfig(new OnOtherEmotionConfigCallback() {

		@Override
		public void OnOtherEmotionConfig(bool isSuccess, string errno,
				string errmsg, OtherEmotionConfigItem item) {
			// TODO Auto-generated method stub
			FileLog("LiveChatManager", "GetEmotionConfig() OnOtherEmotionConfig begin");
			bool success = isSuccess;
			OtherEmotionConfigItem configItem = item;
			if (isSuccess) {
				// 请求成功
				if (m_emotionMgr->IsVerNewThenConfigItem(item.version)) {
					// 配置版本更新
					success = m_emotionMgr->UpdateConfigItem(item);
				}
				else {
					// 使用旧配置
					configItem = m_emotionMgr->GetConfigItem();
				}
			}
			FileLog("LiveChatManager", "GetEmotionConfig() OnOtherEmotionConfig callback");
			mCallbackHandler.OnGetEmotionConfig(success, errno, errmsg, configItem);
			m_emotionMgr->mEmotionConfigReqId = RequestJni.InvalidRequestId;
			FileLog("LiveChatManager", "GetEmotionConfig() OnOtherEmotionConfig end");
		}
	});
	return m_emotionMgr->mEmotionConfigReqId != RequestJni.InvalidRequestId;
}

/**
 * 获取配置item（PS：本次获取可能是旧的，当收到OnGetEmotionConfig()回调时，需要重新调用本函数获取）
 * @return
 */
public OtherEmotionConfigItem GetEmotionConfigItem() {
	return m_emotionMgr->GetConfigItem();
}

/**
 * 获取高级表情item
 * @param emotionId	高级表情ID
 * @return
 */
public LCEmotionItem GetEmotionInfo(string emotionId)
{
	return m_emotionMgr->getEmotion(emotionId);
}

/**
 * 发送高级表情
 * @param userId	对方的用户ID
 * @param emotionId	高级表情ID
 * @param ticket	票根
 * @return
 */
public LCMessageItem SendEmotion(string userId, string emotionId)
{
	// 判断是否处理发送操作
	if (!IsHandleSendOpt()) {
		Log.e("livechat", "LiveChatManager::SendEmotion() IsHandleSendOpt()==false");
		return null;
	}

	// 获取用户item
	LCUserItem userItem = m_userMgr->getUserItem(userId);
	if (null == userItem) {
		Log.e("livechat", string.format("%s::%s() getUserItem fail, userId:%s", "LiveChatManager", "SendPhoto", userId));
		return null;
	}

	LCMessageItem item = null;
	if (!emotionId.empty()) {
		// 生成MessageItem
		item = new LCMessageItem();
		item.init(mMsgIdIndex.getAndIncrement()
				, SendType.Send
				, mUserId
				, userId
				, userItem.inviteId
				, StatusType.Processing);
		// 获取EmotionItem
		LCEmotionItem emotionItem = m_emotionMgr->getEmotion(emotionId);
		// 把EmotionItem添加到MessageItem
		item.setEmotionItem(emotionItem);
		// 添加到历史记录
		userItem.insertSortMsgList(item);

		if (IsSendMessageNow(userItem))
		{
			// 发送消息
			SendEmotionProc(item);
		}
		else if (IsWaitForLoginToSendMessage(userItem))
		{
			// 登录未成功，添加到待发送列表
			userItem.sendMsgList.add(item);
		}
		else
		{
			// 正在使用试聊券，消息添加到待发列表
			userItem.sendMsgList.add(item);
			// 执行尝试使用试聊券流程
			CheckCouponProc(userItem);
		}
	}
	else {
		Log.e("livechat", string.format("%s::%s() param error, userId:%s, emotionId:%s", "LiveChatManager", "SendEmotion", userId, emotionId));
	}
	return item;
}

/**
 * 发送高级表情处理
 * @param item
 */
private void SendEmotionProc(LCMessageItem item)
{
	if (LiveChatClient.SendEmotion(item.toId, item.getEmotionItem().emotionId, item.msgId)) {
		m_emotionMgr->addSendingItem(item);
	}
	else {
		item.statusType = StatusType.Fail;
		mCallbackHandler.OnSendEmotion(LiveChatErrType.Fail, "", item);
	}
}

/**
 * 手动下载/更新高级表情图片文件
 * @param emotionId	高级表情ID
 * @return
 */
public bool GetEmotionImage(string emotionId)
{
	LCEmotionItem emotionItem = m_emotionMgr->getEmotion(emotionId);

	bool result = false;
	// 判断文件是否存在，若不存在则下载
	if (!emotionItem.imagePath.empty()) {
		File file  = new File(emotionItem.imagePath);
		if (file.exists() && file.isFile()) {
			mCallbackHandler.OnGetEmotionImage(true, emotionItem);
			result = true;
		}
	}

	// 文件不存在，需要下载
	if (!result) {
		result = m_emotionMgr->StartDownloadImage(emotionItem);
	}
	return result;
}

/**
 * 手动下载/更新高级表情图片文件
 * @param emotionId	高级表情ID
 * @return
 */
public bool GetEmotionPlayImage(string emotionId)
{
	LCEmotionItem emotionItem = m_emotionMgr->getEmotion(emotionId);

	bool result = false;
	// 判断文件是否存在，若不存在则下载
	if (!emotionItem.playBigPath.empty()) {
		File file  = new File(emotionItem.playBigPath);
		if (file.exists() && file.isFile()) {
			if (emotionItem.playBigImages.size() > 0) {
				result = true;
				for (string filePath : emotionItem.playBigImages)
				{
					File subFile = new File(filePath);
					if (!subFile.exists() || !subFile.isFile()) {
						result = false;
						break;
					}
				}
			}

			// 所有文件都存在
			if (result) {
				mCallbackHandler.OnGetEmotionPlayImage(true, emotionItem);
			}
		}
	}

	// 有文件不存在，需要下载
	if (!result) {
		result = m_emotionMgr->StartDownloadPlayImage(emotionItem);
	}
	return result;
}

/**
 * 手动下载/更新高级表情播放文件
 * @param emotionId	高级表情ID
 * @return
 */
public bool GetEmotion3gp(string emotionId)
{
	LCEmotionItem emotionItem = m_emotionMgr->getEmotion(emotionId);

	bool result = false;
	// 判断文件是否存在，若不存在则下载
	if (!emotionItem.f3gpPath.empty()) {
		File file  = new File(emotionItem.f3gpPath);
		if (file.exists() && file.isFile()) {
			mCallbackHandler.OnGetEmotion3gp(true, emotionItem);
			result = true;
		}
	}

	// 文件不存在，下载文件
	if (!result) {
		result = m_emotionMgr->StartDownload3gp(emotionItem);
	}
	return result;
}

// ---------------- 图片操作函数(Private Album) ----------------
/**
 * 发送图片（包括上传图片文件(php)、发送图片(livechat)）
 * @param userId	对方的用户ID
 * @param photoPath	图片本地路径
 * @return
 */
public LCMessageItem SendPhoto(
		final string userId
		, final string photoPath)
{
	// 判断是否处理发送操作
	if (!IsHandleSendOpt()) {
		Log.e("livechat", "LiveChatManager::SendPhoto() IsHandleSendOpt()==false");
		return null;
	}

	// 获取用户item
	LCUserItem userItem = m_userMgr->getUserItem(userId);
	if (null == userItem) {
		Log.e("livechat", string.format("%s::%s() getUserItem fail, userId:%s", "LiveChatManager", "SendPhoto", userId));
		return null;
	}

	// 生成MessageItem
	LCMessageItem item = new LCMessageItem();
	item.init(mMsgIdIndex.getAndIncrement()
			, SendType.Send
			, mUserId
			, userId
			, userItem.inviteId
			, StatusType.Processing);
	// 生成PhotoItem
	LCPhotoItem photoItem = new LCPhotoItem();
	photoItem.init(
			""
			, ""
			, ""
			, ""
			, ""
			, photoPath
			, ""
			, ""
			, true);
	// 把PhotoItem添加到MessageItem
	item.setPhotoItem(photoItem);
	// 添加到历史记录
	userItem.insertSortMsgList(item);

	if (IsSendMessageNow(userItem))
	{
		// 发送消息
		SendPhotoProc(item);
	}
	else if (IsWaitForLoginToSendMessage(userItem))
	{
		// 登录未成功，添加到待发送列表
		userItem.sendMsgList.add(item);
	}
	else
	{
		// 正在使用试聊券，消息添加到待发列表
		userItem.sendMsgList.add(item);
		// 执行尝试使用试聊券流程
		CheckCouponProc(userItem);
	}

	return item;
}

/**
 * 发送图片处理
 * @param userId
 * @param inviteId
 * @param photoPath
 * @return
 */
private void SendPhotoProc(LCMessageItem item)
{
	LCUserItem userItem = item.getUserItem();
	final LCPhotoItem photoItem = item.getPhotoItem();
	// 请求上传文件
	long requestId = RequestOperator.getInstance().SendPhoto(userItem.userId, userItem.inviteId, mUserId, mSid, photoItem.srcFilePath, new OnLCSendPhotoCallback() {
		@Override
		public void OnLCSendPhoto(long requestId, bool isSuccess, string errno, string errmsg,
				LCSendPhotoItem item)
		{
			LCMessageItem msgItem = m_photoMgr->getAndRemoveRequestItem(requestId);
			if (null == msgItem) {
				Log.e("livechat", string.format("%s::%s() OnLCSendPhoto() get request item fail, requestId:%d", "LiveChatManager", "SendPhoto", requestId));
				return;
			}

			if (isSuccess) {
				LCPhotoItem photoItem = msgItem.getPhotoItem();
				photoItem.photoId = item.photoId;
				photoItem.sendId = item.sendId;

				// 把源文件copy到LiveChat目录下
				m_photoMgr->copyPhotoFileToDir(photoItem, photoItem.srcFilePath);

				if (LiveChatClient.SendPhoto(msgItem.toId
						, msgItem.getUserItem().inviteId
						, photoItem.photoId
						, photoItem.sendId
						, false
						, photoItem.photoDesc
						, msgItem.msgId))
				{
					// 添加到发送map
					m_photoMgr->addSendingItem(msgItem);
				}
				else {
					// LiveChatClient发送不成功
					msgItem.statusType = StatusType.Fail;
					mCallbackHandler.OnSendPhoto(LiveChatErrType.Fail, "", "", msgItem);
				}
			}
			else {
				// 上传文件不成功
				msgItem.statusType = StatusType.Fail;
				mCallbackHandler.OnSendPhoto(LiveChatErrType.Fail, errno, errmsg, msgItem);
			}
		}
	});

	if (RequestJni.InvalidRequestId != requestId) {
		if (!m_photoMgr->addRequestItem(requestId, item)) {
			Log.e("livechat", string.format("%s::%s() add request item fail, requestId:%d", "LiveChatManager", "SendPhoto", requestId));
		}
	}
	else {
		item.statusType = StatusType.Fail;
		mCallbackHandler.OnSendPhoto(LiveChatErrType.Fail, "", "", item);
	}
}

/**
 * 购买图片（包括付费购买图片(php)）
 * @param item
 * @return
 */
public bool PhotoFee(LCMessageItem item)
{
	// 判断参数是否有效
	if (item.msgType != MessageType.Photo
		|| item.fromId.empty()
		|| item.inviteId.empty()
		|| item.getPhotoItem().photoId.empty()
		|| item.statusType != StatusType.Finish)
	{
		Log.e("livechat", string.format("%s::%s() param error, msgType:%s, fromId:%s, inviteId%s, photoId:%s, statusType:%s", "LiveChatManager", "PhotoFee"
				, item.msgType.name(), item.fromId, item.inviteId, item.getPhotoItem().photoId, item.statusType.name()));
		return false;
	}

	// 请求付费获取图片
	long requestId = RequestOperator.getInstance().PhotoFee(
			item.fromId
			, item.inviteId
			, mUserId
			, mSid, item.getPhotoItem().photoId
			, new OnLCPhotoFeeCallback() {
		@Override
		public void OnLCPhotoFee(long requestId, bool isSuccess, string errno,
				string errmsg) {
			// TODO Auto-generated method stub
			LCMessageItem item = m_photoMgr->getAndRemoveRequestItem(requestId);
			LCPhotoItem photoItem = item.getPhotoItem();
			if (null == item || null == photoItem) {
				Log.e("livechat", string.format("%s::%s() OnLCPhotoFee() get request item fail, requestId:%d", "LiveChatManager", "PhotoFee", requestId));
				return;
			}

			item.statusType = isSuccess ? StatusType.Finish : StatusType.Fail;
			photoItem.charge = isSuccess;
			photoItem.statusType = LCPhotoItem.StatusType.Finish;

			if (isSuccess) {
				// 通知LiveChat服务器已经购买图片
				LiveChatClient.ShowPhoto(
					item.getUserItem().userId
					, item.getUserItem().inviteId
					, item.getPhotoItem().photoId
					, item.getPhotoItem().sendId
					, item.getPhotoItem().charge
					, item.getPhotoItem().photoDesc
					, item.msgId);

				// 清除未付费图片
//					m_photoMgr->removeFuzzyPhotoFile(photoItem);
			}
			mCallbackHandler.OnPhotoFee(isSuccess, errno, errmsg, item);
		}
	});

	bool result = false;
	if (requestId != RequestJni.InvalidRequestId) {
		item.statusType = StatusType.Processing;
		LCPhotoItem photoItem = item.getPhotoItem();
		photoItem.statusType = LCPhotoItem.StatusType.PhotoFee;

		result = m_photoMgr->addRequestItem(requestId, item);
		if (!result) {
			Log.e("livechat", string.format("%s::%s() requestId:%d addRequestItem fail", "LiveChatManager", "PhotoFee", requestId));
		}
	}
	else {
		item.statusType = StatusType.Fail;
		mCallbackHandler.OnPhotoFee(false, "request fail", "", item);
	}

	return result;
}

/**
 * 根据消息ID获取图片(模糊或清晰)（包括获取/下载对方私密照片(php)、显示图片(livechat)）
 * @param msgId		消息ID
 * @param sizeType	下载的照片尺寸
 * @return
 */
public bool GetPhoto(string userId, int msgId, final PhotoSizeType sizeType)
{
	LCUserItem userItem = m_userMgr->getUserItem(userId);
	if (null == userItem) {
		Log.e("livechat", "LiveChatManager::GetPhoto() get user item fail, userId:%s", userId);
		return false;
	}

	LCMessageItem item = userItem.getMsgItemWithId(msgId);
	if (null == item) {
		Log.e("livechat", "LiveChatManager::GetPhoto() get message item fail, msgId:%d", msgId);
		return false;
	}

	return GetPhoto(item, sizeType);
}

/**
 * 获取图片(模糊或清晰)（包括获取/下载对方私密照片(php)、显示图片(livechat)）
 * @param item		消息item
 * @param sizeType	下载的照片尺寸
 * @return
 */
private bool GetPhoto(LCMessageItem item, final PhotoSizeType sizeType)
{
	if (item.msgType != MessageType.Photo
		|| item.fromId.empty()
		|| item.getPhotoItem().photoId.empty())
	{
		Log.e("livechat", string.format("%s::%s() param error, msgType:%s, fromId:%s, photoId:%s, statusType:%s", "LiveChatManager", "GetPhoto"
				, item.msgType.name(), item.fromId, item.getPhotoItem().photoId, item.statusType.name()));
		return false;
	}

	if (item.statusType == StatusType.Processing
		&& RequestJni.InvalidRequestId != m_photoMgr->getRequestIdWithItem(item))
	{
		// 正在下载
		return true;
	}

	// 请求下载图片
	final PhotoModeType modeType;
	if (item.sendType == SendType.Send) {
		// 男士发送（直接获取清晰图片）
		modeType = PhotoModeType.Clear;
	}
	else  {
		// 女士发送（判断是否已购买）
		modeType = (item.getPhotoItem().charge ? PhotoModeType.Clear : PhotoModeType.Fuzzy);
	}
	long requestId = RequestOperator.getInstance().GetPhoto(
			RequestJniLiveChat.ToFlagType.ManGetWoman
			, item.getUserItem().userId
			, mUserId
			, mSid
			, item.getPhotoItem().photoId
			, sizeType, modeType
			, m_photoMgr->getTempPhotoPath(item, modeType, sizeType)
			, new OnLCGetPhotoCallback() {

		@Override
		public void OnLCGetPhoto(long requestId, bool isSuccess, string errno,
				string errmsg, string filePath) {
			// TODO Auto-generated method stub
			LCMessageItem item = m_photoMgr->getAndRemoveRequestItem(requestId);
			if (null == item) {
				Log.e("livechat", string.format("%s::%s() OnLCGetPhoto() get request item fail, requestId:%d", "LiveChatManager", "GetPhoto", requestId));
				return;
			}

			if (isSuccess) {
				string tempPath = m_photoMgr->getTempPhotoPath(item, modeType, sizeType);
				m_photoMgr->tempToPhoto(item, tempPath, modeType, sizeType);

				item.getPhotoItem().statusType = LCPhotoItem.StatusType.Finish;
				item.statusType = StatusType.Finish;
				mCallbackHandler.OnGetPhoto(LiveChatErrType.Success, "", "", item);
			}
			else {
				// 获取图片失败
				item.getPhotoItem().statusType = LCPhotoItem.StatusType.Finish;
				item.statusType = StatusType.Fail;
				mCallbackHandler.OnGetPhoto(LiveChatErrType.Fail, errno, errmsg, item);
			}
		}
	});

	bool result = false;
	if (requestId != RequestJni.InvalidRequestId) {
		item.statusType = StatusType.Processing;
		LCPhotoItem photoItem = item.getPhotoItem();
		photoItem.SetStatusType(modeType, sizeType);

		result = m_photoMgr->addRequestItem(requestId, item);
		if (!result) {
			Log.e("livechat", string.format("%s::%s() requestId:%d addRequestItem fail", "LiveChatManager", "GetPhoto", requestId));
		}
	}

	return result;
}

/**
 * 获取图片发送/下载进度
 * @param item	消息item
 * @return
 */
public int GetPhotoProcessRate(LCMessageItem item) {
	int percent = 0;
	long requestId = m_photoMgr->getRequestIdWithItem(item);
	if (requestId != RequestJni.InvalidRequestId) {
		int total = RequestJni.GetDownloadContentLength(requestId);
		int recv = RequestJni.GetRecvLength(requestId);

		if (total > 0) {
			recv = recv * 100;
			percent = recv / total;
		}
	}
	return percent;
}

// ---------------- 语音操作函数(Voice) ----------------
/**
 * 发送语音（包括获取语音验证码(livechat)、上传语音文件(livechat)、发送语音(livechat)）
 * @param userId	对方的用户ID
 * @param voicePath	语音文件本地路径
 * @return
 */
public LCMessageItem SendVoice(string userId, string voicePath, string fileType, int timeLength)
{
	// 判断是否处理发送操作
	if (!IsHandleSendOpt()) {
		Log.e("livechat", "LiveChatManager::SendVoice() IsHandleSendOpt()==false");
		return null;
	}

	// 获取用户item
	LCUserItem userItem = m_userMgr->getUserItem(userId);
	if (null == userItem) {
		Log.e("livechat", string.format("%s::%s() getUserItem fail, userId:%s", "LiveChatManager", "SendVoice", userId));
		return null;
	}

	// 生成MessageItem
	LCMessageItem item = new LCMessageItem();
	item.init(mMsgIdIndex.getAndIncrement()
			, SendType.Send
			, mUserId
			, userId
			, userItem.inviteId
			, StatusType.Processing);
	// 生成VoiceItem
	LCVoiceItem voiceItem = new LCVoiceItem();
	voiceItem.init(""
			, voicePath
			, timeLength
			, fileType
			, ""
			, true);
	// 把VoiceItem添加到MessageItem
	item.setVoiceItem(voiceItem);
	// 添加到聊天记录中
	userItem.insertSortMsgList(item);

	if (IsSendMessageNow(userItem))
	{
		// 发送消息
		SendVoiceProc(item);
	}
	else if (IsWaitForLoginToSendMessage(userItem))
	{
		// 登录未成功，添加到待发送列表
		userItem.sendMsgList.add(item);
	}
	else
	{
		// 正在使用试聊券，消息添加到待发列表
		userItem.sendMsgList.add(item);
		// 执行尝试使用试聊券流程
		CheckCouponProc(userItem);
	}
	return item;
}

/**
 * 发送语音处理
 * @param item
 */
private void SendVoiceProc(LCMessageItem item)
{
	if (LiveChatClient.GetVoiceCode(item.toId, item.msgId))
	{
		m_voiceMgr->addSendingItem(item.msgId, item);
	}
	else {
		item.statusType = StatusType.Fail;
		mCallbackHandler.OnSendVoice(LiveChatErrType.Fail, "", "", item);
	}
}

/**
 * 获取语音（包括下载语音(livechat)）
 * @param item		消息ID
 * @return
 */
public bool GetVoice(LCMessageItem item)
{
	if (item.msgType != MessageType.Voice
		&& null == item.getVoiceItem())
	{
		Log.e("livechat", string.format("%s::%s() param error.", "LiveChatManager", "GetVoice"));
		return false;
	}

	bool result = false;
	LCVoiceItem voiceItem = item.getVoiceItem();
	voiceItem.filePath = m_voiceMgr->getVoicePath(item);
	int siteType = WebSiteManager.newInstance(null).GetWebSite().getSiteId();
	long requestId = RequestOperator.getInstance().PlayVoice(voiceItem.voiceId, siteType, voiceItem.filePath, new OnLCPlayVoiceCallback() {

		@Override
		public void OnLCPlayVoice(long requestId, bool isSuccess, string errno,
				string errmsg, string filePath) {
			// TODO Auto-generated method stub
			LCMessageItem item = m_voiceMgr->getAndRemoveRquestItem(requestId);
			if (null != item) {
				item.statusType = isSuccess ? StatusType.Finish : StatusType.Fail;
				LiveChatErrType errType = isSuccess ? LiveChatErrType.Success : LiveChatErrType.Fail;
				mCallbackHandler.OnGetVoice(errType, errmsg, item);
			}
			else {
				Log.e("livechat", string.format("%s::%s() item is null, requestId:%d, isSuccess:%b, errno:%s, errmsg:%s, filePath:%s"
						, "LiveChatManager", "OnLCPlayVoice", requestId, isSuccess, errno, errmsg, filePath));
			}
		}
	});

	if (requestId != RequestJni.InvalidRequestId) {
		// 添加至请求map
		item.statusType = StatusType.Processing;
		m_voiceMgr->addRequestItem(requestId, item);
		result = true;

		Log.e("livechat", string.format("%s::%s() requestId:%d", "LiveChatManager", "OnLCPlayVoice", requestId));
	}
	else {
		FileLog("livechat", string.format("%s::%s() RequestOperator.getInstance().PlayVoice fail, voiceId:%s, siteType:%d, filePath:%s"
				, "LiveChatManager"
				, "GetVoice"
				, voiceItem.voiceId, siteType, voiceItem.filePath));
		item.statusType = StatusType.Fail;
		mCallbackHandler.OnGetVoice(LiveChatErrType.Fail, "", item);
		result = false;
	}
	return result;
}

/**
 * 获取语音发送/下载进度
 * @param item	消息item
 * @return
 */
public int GetVoiceProcessRate(LCMessageItem item) {
	int percent = 0;
	long requestId = m_voiceMgr->getRequestIdWithItem(item);
	if (requestId != RequestJni.InvalidRequestId) {
		int total = RequestJni.GetDownloadContentLength(requestId);
		int recv = RequestJni.GetRecvLength(requestId);

		if (total > 0) {
			recv = recv * 100;
			percent = recv / total;
		}
	}
	return percent;
}

// ---------------- 视频操作函数(Video) ----------------
/**
 * 获取微视频图片
 * @param item	消息item
 * @param type	视频图片类型
 * @return
 */
public bool GetVideoPhoto(final LCMessageItem item, final VideoPhotoType type)
{
	bool result = false;
	LCUserItem userItem = item.getUserItem();
	LCVideoItem videoItem = item.getVideoItem();
	if (null != userItem
		&& !stringUtil.empty(userItem.userId)
		&& !stringUtil.empty(item.inviteId)
		&& null != videoItem
		&& !stringUtil.empty(videoItem.videoId))
	{
		result = GetVideoPhoto(userItem.userId, videoItem.videoId, userItem.inviteId, type);
	}
	return result;
}

/**
 * 获取微视频图片
 * @param userId	女士ID
 * @param videoItem	视频列表item
 * @param type		视频图片类型
 * @return
 */
public bool GetVideoPhoto(final string userId, final com.qpidnetwork.request.item.LCVideoItem videoItem, final VideoPhotoType type)
{
	bool result = false;
	if (!stringUtil.empty(userId)
		&& null != videoItem
		&& !stringUtil.empty(videoItem.videoid)
		&& !stringUtil.empty(videoItem.inviteid))
	{
		result = GetVideoPhoto(userId, videoItem.videoid, videoItem.inviteid, type);
	}
	return result;
}

/**
 * 获取微视频图片
 * @param userId	用户ID
 * @param videoId	视频ID
 * @param inviteId	邀请ID
 * @param type		视频图片类型
 * @return
 */
private bool GetVideoPhoto(final string userId, final string videoId, final string inviteId, final VideoPhotoType type)
{
	bool result = false;

	// 判断是否已经在下载
	if (mVideoMgr.isVideoPhotoRequest(videoId)) {
		result = true;
	}
	else {
		// 还没请求下载，现在下载
		string filePath = mVideoMgr.getVideoPhotoTempPath(userId, videoId, inviteId, type);
		long requestId = RequestOperator.getInstance().GetVideoPhoto(mSid, mUserId, userId, videoId, type, filePath, new OnRequestFileCallback() {

			@Override
			public void OnRequestFile(long requestId, bool isSuccess, string errno,
					string errmsg, string filePath)
			{
				bool result = false;
				ArrayList<LCMessageItem> videoMsgList = null;
				string desPath = "";

				// 下载成功
				if (isSuccess) {
					// 把临时文件复制到正式文件目录
					desPath = mVideoMgr.getVideoPhotoPath(userId, videoId, inviteId, type);
					result = mVideoMgr.tempFileToDesFile(filePath, desPath);
				}

				// 更新视频消息
				LCUserItem userItem = m_userMgr->getUserItem(userId);
				if (null != userItem) {
					videoMsgList = mVideoMgr.getMessageItem(videoId, userItem.getMsgList());
					for (LCMessageItem item : videoMsgList) {
						// 更新视频图片路径
						if (result) {
							item.getVideoItem().updatePhotoPathWithType(desPath, type);
						}
						// 重置下载标志
						item.getVideoItem().updatePhotoDownloadSign(type, false);
					}
				}

				// 从正在下载列表中移除
				mVideoMgr.getAndRemoveRequestVideoPhoto(requestId);

				// callback
				LiveChatErrType errType = result ? LiveChatErrType.Success : LiveChatErrType.Fail;
				mCallbackHandler.OnGetVideoPhoto(errType
												, errno
												, errmsg
												, userId
												, inviteId
												, videoId
												, type
												, desPath
												, videoMsgList);
			}
		});

		// 添加到requestId对照表
		if (requestId != RequestJni.InvalidRequestId)
		{
			result = true;

			// 设置视频相关的所有聊天消息下载标志位为true
			LCUserItem userItem = m_userMgr->getUserItem(userId);
			if (null != userItem) {
				ArrayList<LCMessageItem> videoMsgList = mVideoMgr.getMessageItem(videoId, userItem.getMsgList());
				for (LCMessageItem item : videoMsgList) {
					item.getVideoItem().updatePhotoDownloadSign(type, true);
				}
			}

			// 添加到对照表
			mVideoMgr.addRequestVideoPhoto(requestId, videoId);
		}
	}

	return result;
}

/**
 * 购买微视频
 * @param item	消息item
 * @return
 */
public bool VideoFee(final LCMessageItem item)
{
	bool result = false;
	if (null != item
		&& null != item.getUserItem()
		&& item.msgType == MessageType.Video
		&& null != item.getVideoItem())
	{
		final LCUserItem userItem = item.getUserItem();
		final LCVideoItem videoItem = item.getVideoItem();

		if (videoItem.isVideoFeeding) {
			// 已在下载
			result = true;
		}
		else {
			long requestId = RequestOperator.getInstance().GetVideo(
									mSid
									, mUserId
									, userItem.userId
									, videoItem.videoId
									, item.inviteId
									, VideoToFlagType.Man
									, videoItem.sendId
									, new OnGetVideoCallback() {

				@Override
				public void OnLCGetVideo(long requestId, bool isSuccess, string errno,
						string errmsg, string url) {
					// TODO Auto-generated method stub
					videoItem.isVideoFeeding = false;
					if (isSuccess) {
						videoItem.charget = true;
						videoItem.videoUrl = url;

						// 通知LiveChat服务器已经购买视频
						LiveChatClient.PlayVideo(
								userItem.userId
								, item.inviteId
								, videoItem.videoId
								, videoItem.sendId
								, videoItem.charget
								, videoItem.videoDesc
								, item.msgId);
					}

					// callback
					mCallbackHandler.OnVideoFee(isSuccess, errno, errmsg, item);
				}
			});

			if (RequestJni.InvalidRequestId != requestId)
			{
				videoItem.isVideoFeeding = true;
				result = true;
			}
		}
	}
	return result;
}

/**
 * 获取微视频播放文件
 * @param item	消息item
 * @return
 */
public bool GetVideo(LCMessageItem item)
{
	bool result = false;
	LCUserItem userItem = item.getUserItem();
	LCVideoItem videoItem = item.getVideoItem();
	if (null != userItem
		&& !stringUtil.empty(userItem.userId)
		&& !stringUtil.empty(item.inviteId)
		&& null != videoItem
		&& !stringUtil.empty(videoItem.videoId)
		&& videoItem.charget
		&& !stringUtil.empty(videoItem.videoUrl))
	{
		result = GetVideo(userItem.userId, videoItem.videoId, userItem.inviteId, videoItem.videoUrl);
	}
	return result;
}

/**
 * 获取微视频播放文件
 * @param userId	用户ID
 * @param videoItem	视频列表item
 * @return
 */
public bool GetVideo(final string userId, final com.qpidnetwork.request.item.LCVideoItem videoItem)
{
	bool result = false;
	if (!stringUtil.empty(userId)
		&& null != videoItem
		&& !stringUtil.empty(videoItem.videoid)
		&& !stringUtil.empty(videoItem.inviteid)
		&& !stringUtil.empty(videoItem.video_url))
	{
		result = GetVideo(userId, videoItem.videoid, videoItem.inviteid, videoItem.video_url);
	}
	return result;
}

/**
 * 获取微视频播放文件
 * @param userId	用户ID
 * @param videoId	视频ID
 * @param inviteId	邀请ID
 * @param videoUrl	视频文件下载URL
 * @return
 */
private bool GetVideo(final string userId, final string videoId, final string inviteId, final string videoUrl)
{
	bool result = false;

	// 判断是否已在下载
	if (mVideoMgr.isVideoDownload(videoId)) {
		result = true;
	}
	else {
		// 还没请求下载，现在下载
		final string tempPath = mVideoMgr.getVideoTempPath(userId, videoId, inviteId);
		FileDownloader fileDownloader = new FileDownloader();
		fileDownloader.SetBigFile(true);
		fileDownloader.SetUseCache(false);
		fileDownloader.StartDownload(videoUrl, tempPath, new FileDownloader.FileDownloaderCallback() {

			@Override
			public void onUpdate(FileDownloader loader, int progress) {
				// TODO Auto-generated method stub
				// 获取所有与该视频相关的所有聊天消息
				LCUserItem userItem = m_userMgr->getUserItem(userId);
				if (null != userItem) {
					ArrayList<LCMessageItem> videoMsgList = mVideoMgr.getMessageItem(videoId, userItem.getMsgList());
					for (LCMessageItem item : videoMsgList) {
						// 更新视频下载进度
						item.getVideoItem().videoDownloadProgress = progress;
					}
				}
			}

			@Override
			public void onSuccess(FileDownloader loader) {
				// TODO Auto-generated method stub
				bool result = false;
				// 把临时文件复制到正式文件目录
				string videoPath = mVideoMgr.getVideoPath(userId, videoId, inviteId);
				result = mVideoMgr.tempFileToDesFile(tempPath, videoPath);

				ArrayList<LCMessageItem> videoMsgList = null;
				if (result) {
					// 获取所有与该视频相关的所有聊天消息
					LCUserItem userItem = m_userMgr->getUserItem(userId);
					if (null != userItem) {
						videoMsgList = mVideoMgr.getMessageItem(videoId, userItem.getMsgList());
						for (LCMessageItem item : videoMsgList) {
							// 更新视频消息的图片路径
							item.getVideoItem().videoFilePath = videoPath;
							// 重置下载标志位
							item.getVideoItem().isVideoDownloading = false;
							item.getVideoItem().videoDownloadProgress = 0;
						}
					}
				}

				// 移除下载对照表
				mVideoMgr.getAndRemoveDownloadVideo(loader);

				// callback
				LiveChatErrType errType = result ? LiveChatErrType.Success : LiveChatErrType.Fail;
				mCallbackHandler.OnGetVideo(errType
											, userId
											, videoId
											, inviteId
											, videoPath
											, videoMsgList);
			}

			@Override
			public void onFail(FileDownloader loader) {
				// TODO Auto-generated method stub
				ArrayList<LCMessageItem> videoMsgList = null;
				// 获取所有与该视频相关的聊天消息
				LCUserItem userItem = m_userMgr->getUserItem(userId);
				if (null != userItem) {
					videoMsgList = mVideoMgr.getMessageItem(videoId, userItem.getMsgList());
					for (LCMessageItem item : videoMsgList) {
						// 重置下载标志位
						item.getVideoItem().isVideoDownloading = false;
						item.getVideoItem().videoDownloadProgress = 0;
					}
				}

				// 移除下载对照表
				mVideoMgr.getAndRemoveDownloadVideo(loader);

				// callback
				mCallbackHandler.OnGetVideo(LiveChatErrType.Fail
											, userId
											, videoId
											, inviteId
											, ""
											, videoMsgList);
			}
		});

		result = true;

		ArrayList<LCMessageItem> videoMsgList = null;
		// 设置视频相关的所有聊天消息下载标志位为true
		LCUserItem userItem = m_userMgr->getUserItem(userId);
		if (null != userItem) {
			videoMsgList = mVideoMgr.getMessageItem(videoId, userItem.getMsgList());
			for (LCMessageItem item : videoMsgList) {
				item.getVideoItem().isVideoDownloading = true;
				item.getVideoItem().videoDownloadProgress = 0;
			}
		}

		// 添加到下载对照表
		mVideoMgr.addDownloadVideo(fileDownloader, videoId);

		if (null != videoMsgList) {
			// 通知回调开始下载视频
			mCallbackHandler.OnStartGetVideo(userId
										, videoId
										, inviteId
										, ""
										, videoMsgList);
		}
	}

	return result;
}

/**
 * 获取视频下载状态
 * @param videoId	视频ID
 * @return true为正在下载，否则为不在下载状态
 */
public bool isGetVideoNow(final string videoId)
{
	return mVideoMgr.isVideoDownload(videoId);
}

/**
 * 获取视频图片文件路径，若文件已存在
 * @param userId	用户ID
 * @param videoId	视频ID
 * @param inviteId	邀请ID
 * @param type		图片类型
 * @return
 */
public string GetVideoPhotoPathWithExist(final string userId, final string inviteId, final string videoId, final VideoPhotoType type)
{
	string path = "";
	string localPath = mVideoMgr.getVideoPhotoPath(userId, videoId, inviteId, type);
	if (!localPath.empty())
	{
		File file = new File(localPath);
		if (file.exists())
		{
			path = localPath;
		}
	}
	return path;
}

/**
 * 获取视频文件路径，若文件已存在
 * @param userId	用户ID
 * @param inviteId	邀请ID
 * @param videoId	视频ID
 * @return
 */
public string GetVideoPathWithExist(final string userId, final string inviteId, final string videoId)
{
	string path = "";
	string localPath = mVideoMgr.getVideoPath(userId, videoId, inviteId);
	if (!localPath.empty())
	{
		File file = new File(localPath);
		if (file.exists())
		{
			path = localPath;
		}
	}
	return path;
}

// ------------- LiveChatClientListener abstract function -------------
/**
 * 登录回调
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 */
@Override
public void OnLogin(LiveChatErrType errType, string errmsg)
{
	FileLog("livechat", string.format("OnLogin() begin errType:%s", errType.name()));
	bool isAutoLogin = false;
	if (errType == LiveChatErrType.Success) {
		mIsLogin = true;

		// 上传客户端内部版本号
		Message msgUploadVer = Message.obtain();
		msgUploadVer.what = LiveChatRequestOptType.UploadClientVersion.ordinal();
		mHandler.sendMessage(msgUploadVer);

		// 获取黑名单列表
		Message msgBlockList = Message.obtain();
		msgBlockList.what = LiveChatRequestOptType.GetBlockList.ordinal();
		mHandler.sendMessage(msgBlockList);

		// 获取被屏蔽女士列表
		Message msgBlockUsers = Message.obtain();
		msgBlockUsers.what = LiveChatRequestOptType.GetBlockUsers.ordinal();
		mHandler.sendMessage(msgBlockUsers);

		// 获取联系人列表
		Message msgContactList = Message.obtain();
		msgContactList.what = LiveChatRequestOptType.GetContactList.ordinal();
		mHandler.sendMessage(msgContactList);

		// 获取在聊/邀请用户列表
		Message msgGetTalkList = Message.obtain();
		msgGetTalkList.what = LiveChatRequestOptType.GetTalkList.ordinal();
		mHandler.sendMessage(msgGetTalkList);

		// 获取高级表情配置
		Message msgGetEmotionConfig = Message.obtain();
		msgGetEmotionConfig.what = LiveChatRequestOptType.GetEmotionConfig.ordinal();
		mHandler.sendMessage(msgGetEmotionConfig);

		// 使用试聊券，发送待发消息
		Message msg = Message.obtain();
		msg.what = LiveChatRequestOptType.CheckCouponWithToSendUser.ordinal();
		mHandler.sendMessage(msg);
	}
	else if (IsAutoRelogin(errType)) {
		FileLog("livechat", "OnLogin() AutoRelogin() begin");
		// 自动重登录
		isAutoLogin = true;
		Message msgAutoRelogin = Message.obtain();
		msgAutoRelogin.what = LiveChatRequestOptType.AutoRelogin.ordinal();
		mHandler.sendMessageDelayed(msgAutoRelogin, mAutoReloginTime);
		FileLog("livechat", "OnLogin() AutoRelogin() end");
	}
	else {
		mUserId = null;
		mSid = null;
		mDeviceId = null;
	}

	FileLog("livechat", "OnLogin() callback");
	mCallbackHandler.OnLogin(errType, errmsg, isAutoLogin);
	FileLog("livechat", "OnLogin() end");
}

/**
 * 注销/断线回调
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 * @param isAutoLogin	是否自动登录
 */
@Override
public void OnLogout(LiveChatErrType errType, string errmsg)
{
	FileLog("livechat", "OnLogout() begin, errType:%s", errType.name());

	// 重置参数
	mIsLogin = false;

	// callback
	bool isAutoLogin = IsAutoRelogin(errType);
	FileLog("livechat", "OnLogout(bool bActive) callback OnLogout");
	mCallbackHandler.OnLogout(errType, errmsg, isAutoLogin);
	if (isAutoLogin) {
		// 自动重登录
		FileLog("livechat", "OnLogout(bool bActive) AutoRelogin");
		Message msgAutoRelogin = Message.obtain();
		msgAutoRelogin.what = LiveChatRequestOptType.AutoRelogin.ordinal();
		mHandler.sendMessageDelayed(msgAutoRelogin, mAutoReloginTime);
	}
	else {
		// 重置参数
		ResetParam();
	}

	FileLog("livechat", "OnLogout(bool bActive) end");
}

/**
 * 设置在线状态回调
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 */
@Override
public void OnSetStatus(LiveChatErrType errType, string errmsg)
{
	mCallbackHandler.OnSetStatus(errType, errmsg);
}

/**
 * 结束聊天会话回调
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 * @param userId	用户ID
 */
@Override
public void OnEndTalk(LiveChatErrType errType, string errmsg, string userId)
{
	LCUserItem userItem = m_userMgr->getUserItem(userId);
	if (errType == LiveChatErrType.Success) {
		if (null != userItem) {
			userItem.endTalk();
		}
		else {
			Log.e("livechat", string.format("%s::%s() getUserItem is null, userId:%s", "LiveChatManager", "OnEndTalk", userId));
		}
	}
	mCallbackHandler.OnEndTalk(errType, errmsg, userItem);

	// 生成警告消息
	if (errType != LiveChatErrType.Success) {
		if (null != userItem) {
			BuildAndInsertWarningWithErrType(userItem, errType);
		}
	}
}

/**
 * 获取用户在线状态回调
 * @param errType			处理结果类型
 * @param errmsg			处理结果描述
 * @param userStatusArray	用户在线状态数组
 */
@Override
public void OnGetUserStatus(
		LiveChatErrType errType
		, string errmsg
		, LiveChatUserStatus[] userStatusArray)
{
	ArrayList<LCUserItem> userArrayList = new ArrayList<LCUserItem>();
	if (null != userStatusArray)
	{
		for (int i = 0; i < userStatusArray.length; i++)
		{
			LiveChatUserStatus userStatusItem = userStatusArray[i];
			LCUserItem userItem = m_userMgr->getUserItem(userStatusItem.userId);
			if (null != userItem) {
				SetUserOnlineStatus(userItem, userStatusItem.statusType);
				userArrayList.add(userItem);
			}
		}
	}

	LCUserItem[] userList = new LCUserItem[userArrayList.size()];
	userArrayList.toArray(userList);
	mCallbackHandler.OnGetUserStatus(errType, errmsg, userList);
}

/**
 * 获取聊天会话信息回调
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 * @param userId	用户ID
 * @param invitedId	邀请ID
 * @param charget	是否已付费
 * @param chatTime	聊天时长
 */
@Override
public void OnGetTalkInfo(
		LiveChatErrType errType
		, string errmsg
		, string userId
		, string invitedId
		, bool charget
		, int chatTime)
{
	// 暂时没用
}

/**
 * 发送聊天文本消息回调
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 * @param userId	用户ID
 * @param message	消息内容
 * @param ticket	票根
 */
@Override
public void OnSendMessage(
		LiveChatErrType errType
		, string errmsg
		, string userId
		, string message
		, int ticket)
{
	LCMessageItem item = m_textMgr->getAndRemoveSendingItem(ticket);
	if (null != item) {
		item.statusType = (errType==LiveChatErrType.Success ? StatusType.Finish : StatusType.Fail);
		mCallbackHandler.OnSendMessage(errType, errmsg, item);
	}
	else {
		Log.e("livechat", string.format("%s::%s() get sending item fail, ticket:%d", "LiveChatManager", "OnSendMessage", ticket));
	}

	// 生成警告消息
	if (errType != LiveChatErrType.Success) {
		if (null != item && null != item.getUserItem()) {
			BuildAndInsertWarningWithErrType(item.getUserItem(), errType);
		}
	}

	FileLog("livechat", "OnSendMessage() errType:%s, userId:%s, message:%s", errType.name(), userId, message);
}

/**
 * 发送高级表情消息回调
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 * @param userId	用户ID
 * @param emotionId	高级表情ID
 * @param ticket	票根
 */
@Override
public void OnSendEmotion(
		LiveChatErrType errType
		, string errmsg
		, string userId
		, string emotionId
		, int ticket)
{
	LCMessageItem item = m_emotionMgr->getAndRemoveSendingItem(ticket);
	if (null != item) {
		item.statusType = (errType==LiveChatErrType.Success ? StatusType.Finish : StatusType.Fail);
		mCallbackHandler.OnSendMessage(errType, errmsg, item);
	}
	else {
		Log.e("livechat", string.format("%s::%s() get sending item fail, ticket:%d", "LiveChatManager", "OnSendEmotion", ticket));
	}

	// 生成警告消息
	if (errType != LiveChatErrType.Success) {
		if (null != item && null != item.getUserItem()) {
			BuildAndInsertWarningWithErrType(item.getUserItem(), errType);
		}
	}
}

/**
 * 发送虚拟礼物回调
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 * @param userId	用户ID
 * @param giftId	虚拟礼物ID
 */
@Override
public void OnSendVGift(
		LiveChatErrType errType
		, string errmsg
		, string userId
		, string giftId
		, int ticket)
{
	// 暂时没用（本版本暂时未实现本功能）
}

/**
 * 获取发送语音验证码回调
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 * @param userId	用户ID
 * @param ticket	票根
 * @param voiceCode	语音ID
 */
@Override
public void OnGetVoiceCode(
		LiveChatErrType errType
		, string errmsg
		, string userId
		, int ticket
		, string voiceCode)
{
	LCMessageItem item = m_voiceMgr->getAndRemoveSendingItem(ticket);
	if (errType == LiveChatErrType.Success) {
		if (null != item
			&& item.msgType == MessageType.Voice
			&& null != item.getVoiceItem())
		{
			LCVoiceItem voiceItem = item.getVoiceItem();
			voiceItem.checkCode = voiceCode;
			LCUserItem userItem = item.getUserItem();

			int siteType = WebSiteManager.newInstance(null).GetWebSite().getSiteId();

			// 请求上传语音文件
			long requestId = RequestOperator.getInstance().UploadVoice(
									voiceItem.checkCode
									, userItem.inviteId
									, mUserId
									, true
									, userItem.userId
									, siteType
									, voiceItem.fileType
									, voiceItem.timeLength
									, voiceItem.filePath
									, new OnLCUploadVoiceCallback()
			{
				@Override
				public void OnLCUploadVoice(long requestId, bool isSuccess, string errno,
						string errmsg, string voiceId) {
					// TODO Auto-generated method stub
					LCMessageItem item = m_voiceMgr->getAndRemoveRquestItem(requestId);
					LCVoiceItem voiceItem = item.getVoiceItem();
					if (null == voiceItem) {
						Log.e("livechat", string.format("%s::%s() param fail. voiceItem is null.", "LiveChatManager", "OnGetVoiceCode"));
						mCallbackHandler.OnSendVoice(LiveChatErrType.Fail, "", "", item);
					}

					if (isSuccess) {
						voiceItem.voiceId = voiceId;
						if (LiveChatClient.SendVoice(item.toId, voiceItem.voiceId, voiceItem.timeLength, item.msgId)) {
							m_voiceMgr->addSendingItem(item.msgId, item);
						}
						else {
							mCallbackHandler.OnSendVoice(LiveChatErrType.Fail, "", "", item);
						}
					}
					else {
						item.statusType = StatusType.Fail;
						mCallbackHandler.OnSendVoice(LiveChatErrType.Fail, errno, errmsg, item);
					}
				}
			});

			if (requestId != RequestJni.InvalidRequestId) {
				// 添加item到请求map
				m_voiceMgr->addRequestItem(requestId, item);
			}
			else {
				item.statusType = StatusType.Fail;
				mCallbackHandler.OnSendVoice(LiveChatErrType.Fail, "", "", item);
			}
		}
		else {
			Log.e("livechat", string.format("%s::%s() param fail.", "LiveChatManager", "OnGetVoiceCode"));
		}
	}
	else {
		item.statusType = StatusType.Fail;
		mCallbackHandler.OnSendVoice(errType, "", errmsg, item);
	}

	// 生成警告消息
	if (errType != LiveChatErrType.Success) {
		if (null != item && null != item.getUserItem()) {
			BuildAndInsertWarningWithErrType(item.getUserItem(), errType);
		}
	}
}

/**
 * 发送语音回调
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 * @param userId	用户ID
 * @param voiceId	语音ID
 */
@Override
public void OnSendVoice(
		LiveChatErrType errType
		, string errmsg
		, string userId
		, string voiceId
		, int ticket)
{
	LCMessageItem item = m_voiceMgr->getAndRemoveSendingItem(ticket);
	if (null == item
		|| item.msgType != MessageType.Voice
		|| null == item.getVoiceItem())
	{
		Log.e("livechat", string.format("%s::%s() param fail.", "LiveChatManager", "OnSendVoice"));
		mCallbackHandler.OnSendVoice(LiveChatErrType.Fail, "", "", item);
		return;
	}

	// 发送成功
	if (errType == LiveChatErrType.Success) {
		m_voiceMgr->copyVoiceFileToDir(item);
	}

	// 回调
	item.statusType = (LiveChatErrType.Success==errType ? StatusType.Finish : StatusType.Fail);
	mCallbackHandler.OnSendVoice(errType, "", "", item);

	// 生成警告消息
	if (errType != LiveChatErrType.Success) {
		if (null != item && null != item.getUserItem()) {
			BuildAndInsertWarningWithErrType(item.getUserItem(), errType);
		}
	}
}

/**
 * 使用试聊券回调
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 * @param userId	用户ID
 * @param eventType	试聊券使用情况
 */
@Override
public void OnUseTryTicket(
		LiveChatErrType errType
		, string errmsg
		, string userId
		, TryTicketEventType eventType)
{
	mCallbackHandler.OnUseTryTicket(errType, "", errmsg, userId, eventType);

	if (errType != LiveChatErrType.Success) {
		// 使用不成功，生成警告消息
		LCUserItem userItem = m_userMgr->getUserItem(userId);
		if (null != userItem) {
			BuildAndInsertWarningWithErrType(userItem, errType);
		}
		// 检测是否有足够余额
		Message msg = Message.obtain();
		msg.what = LiveChatRequestOptType.CheckMoney.ordinal();
		msg.obj = userId;
		mHandler.sendMessage(msg);
	}
	else {
		// 若当前状态为Other，则标记为ManInvite(男士邀请)状态
		LCUserItem userItem = m_userMgr->getUserItem(userId);
		if (null != userItem) {
			if (userItem.chatType == ChatType.Other
				|| userItem.chatType == ChatType.Invite)
			{
				userItem.chatType = ChatType.ManInvite;
			}
		}
		// 使用成功，发送待发消息
		Message msg = Message.obtain();
		msg.what = LiveChatRequestOptType.SendMessageList.ordinal();
		msg.obj = userId;
		mHandler.sendMessage(msg);
	}
}

/**
 * 获取邀请/在聊列表回调
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 * @param listType	请求列表类型
 * @param info		请求结果
 */
@Override
public void OnGetTalkList(
		LiveChatErrType errType
		, string errmsg
		, int listType
		, LiveChatTalkListInfo info)
{
	if (errType == LiveChatErrType.Success)
	{
		int i = 0;
		// 在聊列表
		for (i = 0; i < info.chatingArray.length; i++) {
			LiveChatTalkUserListItem item = info.chatingArray[i];
			LCUserItem userItem = m_userMgr->getUserItem(item.userId);
			if (null != userItem) {
				userItem.userName = item.userName;
				userItem.sexType = UserSexType.USER_SEX_FEMALE;
//					userItem.statusType = UserStatusType.USTATUS_ONLINE;
				SetUserOnlineStatus(userItem, UserStatusType.USTATUS_ONLINE);
				userItem.chatType = ChatType.InChatCharge;
				userItem.clientType = item.clientType;
				userItem.order = item.orderValue;

				FileLog("livechat", string.format("OnGetTalkList() chatingArray, userId:%s, status:%s", item.userId, item.statusType.name()));
			}
		}
		// 在聊session列表
		for (i = 0; i < info.chatingSessionArray.length; i++) {
			LiveChatTalkSessionListItem item = info.chatingSessionArray[i];
			LCUserItem userItem = m_userMgr->getUserItem(item.userId);
			if (null != userItem) {
				userItem.inviteId = item.invitedId;
			}
		}

		// 在聊暂停列表
		for (i = 0; i < info.pauseArray.length; i++) {
			LiveChatTalkUserListItem item = info.pauseArray[i];
			LCUserItem userItem = m_userMgr->getUserItem(item.userId);
			if (null != userItem) {
				userItem.userName = item.userName;
				userItem.sexType = UserSexType.USER_SEX_FEMALE;
//					userItem.statusType = UserStatusType.USTATUS_ONLINE;
				SetUserOnlineStatus(userItem, UserStatusType.USTATUS_ONLINE);
				userItem.chatType = ChatType.InChatCharge;
				userItem.clientType = item.clientType;
				userItem.order = item.orderValue;

				FileLog("livechat", string.format("OnGetTalkList() pauseArray, userId:%s, status:%s", item.userId, item.statusType.name()));
			}
		}
		// 在聊暂停session列表
		for (i = 0; i < info.pauseSessionArray.length; i++) {
			LiveChatTalkSessionListItem item = info.pauseSessionArray[i];
			LCUserItem userItem = m_userMgr->getUserItem(item.userId);
			if (null != userItem) {
				userItem.inviteId = item.invitedId;
			}
		}

		// 邀请列表
		for (i = 0; i < info.inviteArray.length; i++) {
			LiveChatTalkUserListItem item = info.inviteArray[i];
			LCUserItem userItem = m_userMgr->getUserItem(item.userId);
			if (null != userItem) {
				userItem.userName = item.userName;
				userItem.sexType = UserSexType.USER_SEX_FEMALE;
//					userItem.statusType = UserStatusType.USTATUS_ONLINE;
				SetUserOnlineStatus(userItem, UserStatusType.USTATUS_ONLINE);
				userItem.chatType = ChatType.Invite;
				userItem.clientType = item.clientType;
				userItem.order = item.orderValue;

				FileLog("livechat", string.format("OnGetTalkList() inviteArray, userId:%s, status:%s", item.userId, item.statusType.name()));
			}
		}
		// 邀请session列表
		for (i = 0; i < info.inviteSessionArray.length; i++) {
			LiveChatTalkSessionListItem item = info.inviteSessionArray[i];
			LCUserItem userItem = m_userMgr->getUserItem(item.userId);
			if (null != userItem) {
				userItem.inviteId = item.invitedId;
			}
		}

		// 获取 inchat 用户历史聊天记录
		Message msgGetUsersHistoryMessage = Message.obtain();
		msgGetUsersHistoryMessage.what = LiveChatRequestOptType.GetUsersHistoryMessage.ordinal();
		mHandler.sendMessage(msgGetUsersHistoryMessage);
	}

	mCallbackHandler.OnGetTalkList(errType, errmsg);
}

/**
 * 发送图片回调
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 */
@Override
public void OnSendPhoto(LiveChatErrType errType, string errmsg, int ticket)
{
	LCMessageItem item = m_photoMgr->getAndRemoveSendingItem(ticket);
	if (null != item) {
		item.statusType = (errType==LiveChatErrType.Success ? StatusType.Finish : StatusType.Fail);
		mCallbackHandler.OnSendPhoto(errType, "", errmsg, item);
	}
	else {
		Log.e("livechat", string.format("%s::%s() get sending item fail, ticket:%d", "LiveChatManager", "OnSendPhoto", ticket));
	}

	// 生成警告消息
	if (errType != LiveChatErrType.Success) {
		if (null != item && null != item.getUserItem()) {
			BuildAndInsertWarningWithErrType(item.getUserItem(), errType);
		}
	}
}

/**
 * 显示图片回调
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 */
@Override
public void OnShowPhoto(LiveChatErrType errType, string errmsg, int ticket)
{
//		LCMessageItem item = m_photoMgr->getAndRemoveSendingItem(ticket);
//		if (null != item) {
//			item.statusType = (errType==LiveChatErrType.Success ? StatusType.Finish : StatusType.Fail);
//			mCallbackHandler.OnGetPhoto(errType, "", errmsg, item);
//		}
//		else {
//			Log.e("livechat", string.format("%s::%s() get sending item fail, msgId:%d", "LiveChatManager", "OnShowPhoto", ticket));
//		}
//
//		// 生成警告消息
//		if (errType != LiveChatErrType.Success) {
//			if (null != item && null != item.getUserItem()) {
//				BuildAndInsertWarning(item.getUserItem(), errType);
//			}
//		}
}

/**
 * 播放微视频回调
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 */
@Override
public void OnPlayVideo(LiveChatErrType errType, string errmsg, int ticket)
{

}

/**
 * 获取用户信息
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 * @param item		用户信息item
 */
@Override
public void OnGetUserInfo(LiveChatErrType errType, string errmsg, LiveChatTalkUserListItem item)
{
	if (errType == LiveChatErrType.Success)
	{
		// 更新用户排序分值
		mInviteMgr.UpdateUserOrderValue(item.userId, item.orderValue);
	}
}

/**
 * 获取黑名单列表
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 * @param list		黑名单列表
 */
@Override
public void OnGetBlockList(LiveChatErrType errType, string errmsg, LiveChatTalkUserListItem[] list)
{
	if (errType == LiveChatErrType.Success) {
		mBlockMgr.UpdateWithBlockList(list);
	}
}

/**
 * 获取LiveChat联系人列表
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 * @param list		联系人列表
 */
@Override
public void OnGetContactList(LiveChatErrType errType, string errmsg, LiveChatTalkUserListItem[] list)
{
	if (errType == LiveChatErrType.Success) {
		mContactMgr.UpdateWithContactList(list);
	}
}

/**
 * 获取被屏蔽女士列表
 * @param errType	处理结果类型
 * @param errmsg	处理结果描述
 * @param usersId	被屏蔽女士列表
 */
public void OnGetBlockUsers(LiveChatErrType errType, string errmsg, string[] usersId)
{
	if (errType == LiveChatErrType.Success) {
		mBlockMgr.UpdateWithBlockUsers(usersId);
	}
}

/**
 * 接收聊天文本消息回调
 * @param toId		接收者ID
 * @param fromId	发送者ID
 * @param fromName	发送者用户名
 * @param inviteId	邀请ID
 * @param charget	是否已付费
 * @param ticket	票根
 * @param msgType	聊天消息类型
 * @param message	消息内容
 */
@Override
public void OnRecvMessage(
		string toId
		, string fromId
		, string fromName
		, string inviteId
		, bool charget
		, int ticket
		, TalkMsgType msgType
		, string message)
{
	// 返回票根给服务器
	LiveChatClient.UploadTicket(fromId, ticket);

	LCMessageItem item = null;

	// 判断是否邀请消息
	HandleInviteMsgType handleType = mInviteMgr.IsToHandleInviteMsg(fromId, charget, msgType);
	if (handleType == HandleInviteMsgType.HANDLE) {
		// 处理邀请消息
		item = mInviteMgr.HandleInviteMessage(mMsgIdIndex, toId, fromId, fromName, inviteId, charget, ticket, msgType, message);
	}
	else if (handleType == HandleInviteMsgType.PASS) {
		// 添加用户到列表中（若列表中用户不存在）
		LCUserItem userItem = m_userMgr->getUserItem(fromId);
		if (null == userItem) {
			Log.e("livechat", string.format("%s::%s() getUserItem fail, fromId:%s", "LiveChatManager", "OnRecvMessage", fromId));
			return;
		}
		userItem.inviteId = inviteId;
		userItem.userName = fromName;
		userItem.setChatTypeWithTalkMsgType(charget, msgType);
//			userItem.statusType = UserStatusType.USTATUS_ONLINE;
		SetUserOnlineStatus(userItem, UserStatusType.USTATUS_ONLINE);

		// 生成MessageItem
		item = new LCMessageItem();
		item.init(mMsgIdIndex.getAndIncrement()
				, SendType.Recv
				, fromId
				, toId
				, userItem.inviteId
				, StatusType.Finish);
		// 生成TextItem
		LCTextItem textItem = new LCTextItem();
		textItem.init(message);
		// 把TextItem添加到MessageItem
		item.setTextItem(textItem);
		// 添加到用户聊天记录中
		if (!userItem.insertSortMsgList(item)) {
			// 添加到用户聊天记录列表不成功
			item = null;
		}
	}

	if (null != item) {
		// callback
		mCallbackHandler.OnRecvMessage(item);
	}
}

/**
 * 接收高级表情消息回调
 * @param toId		接收者ID
 * @param fromId	发送者ID
 * @param fromName	发送者用户名
 * @param inviteId	邀请ID
 * @param charget	是否已付费
 * @param ticket	票根
 * @param msgType	聊天消息类型
 * @param emotionId	高级表情ID
 */
@Override
public void OnRecvEmotion(
		string toId
		, string fromId
		, string fromName
		, string inviteId
		, bool charget
		, int ticket
		, TalkMsgType msgType
		, string emotionId)
{
	// 返回票根给服务器
	LiveChatClient.UploadTicket(fromId, ticket);

	// 添加用户到列表中（若列表中用户不存在）
	LCUserItem userItem = m_userMgr->getUserItem(fromId);
	if (null == userItem) {
		Log.e("livechat", string.format("%s::%s() getUserItem fail, fromId:%s", "LiveChatManager", "OnRecvEmotion", fromId));
		return;
	}
	userItem.inviteId = inviteId;
	userItem.userName = fromName;
	userItem.setChatTypeWithTalkMsgType(charget, msgType);
//		userItem.statusType = UserStatusType.USTATUS_ONLINE;
	SetUserOnlineStatus(userItem, UserStatusType.USTATUS_ONLINE);

	// 生成MessageItem
	LCMessageItem item = new LCMessageItem();
	item.init(mMsgIdIndex.getAndIncrement()
			, SendType.Recv
			, fromId
			, toId
			, userItem.inviteId
			, StatusType.Finish);
	// 获取EmotionItem
	LCEmotionItem emotionItem = m_emotionMgr->getEmotion(emotionId);
	// 把EmotionItem添加到MessageItem
	item.setEmotionItem(emotionItem);

	// 添加到用户聊天记录中
	userItem.insertSortMsgList(item);

	// callback
	mCallbackHandler.OnRecvEmotion(item);
}

/**
 * 接收语音消息回调
 * @param toId		接收者ID
 * @param fromId	发送者ID
 * @param fromName	发送者用户名
 * @param inviteId	邀请ID
 * @param charget	是否已付费
 * @param msgType	聊天消息类型
 * @param voiceId	语音ID
 * @param fileType	语音文件类型
 * @param timeLen	语音时长
 */
@Override
public void OnRecvVoice(
		string toId
		, string fromId
		, string fromName
		, string inviteId
		, bool charget
		, TalkMsgType msgType
		, string voiceId
		, string fileType
		, int timeLen)
{
	// 添加用户到列表中（若列表中用户不存在）
	LCUserItem userItem = m_userMgr->getUserItem(fromId);
	if (null == userItem) {
		Log.e("livechat", string.format("%s::%s() getUserItem fail, fromId:%s", "LiveChatManager", "OnRecvVoice", fromId));
		return;
	}
	userItem.userName = fromName;
	userItem.inviteId = inviteId;
	userItem.setChatTypeWithTalkMsgType(charget, msgType);
//		userItem.statusType = UserStatusType.USTATUS_ONLINE;
	SetUserOnlineStatus(userItem, UserStatusType.USTATUS_ONLINE);

	// 生成MessageItem
	LCMessageItem item = new LCMessageItem();
	item.init(mMsgIdIndex.getAndIncrement()
			, SendType.Recv
			, fromId
			, toId
			, userItem.inviteId
			, StatusType.Finish);
	// 生成VoiceItem
	LCVoiceItem voiceItem = new LCVoiceItem();
	voiceItem.init(voiceId
			, m_voiceMgr->getVoicePath(voiceId, fileType)
			, timeLen
			, fileType
			, ""
			, charget);

	// 把VoiceItem添加到MessageItem
	item.setVoiceItem(voiceItem);

	// 添加到聊天记录中
	userItem.insertSortMsgList(item);

	// callback
	mCallbackHandler.OnRecvVoice(item);
}

/**
 * 接收警告消息回调
 * @param toId		接收者ID
 * @param fromId	发送者ID
 * @param fromName	发送者用户名
 * @param inviteId	邀请ID
 * @param charget	是否已付费
 * @param ticket	票根
 * @param msgType	聊天消息类型
 * @param message	消息内容
 */
@Override
public void OnRecvWarning(
		string toId
		, string fromId
		, string fromName
		, string inviteId
		, bool charget
		, int ticket
		, TalkMsgType msgType
		, string message)
{
	// 返回票根给服务器
	LiveChatClient.UploadTicket(fromId, ticket);

	// 添加用户到列表中（若列表中用户不存在）
	LCUserItem userItem = m_userMgr->getUserItem(fromId);
	if (null == userItem) {
		Log.e("livechat", string.format("%s::%s() getUserItem fail, fromId:%s", "LiveChatManager", "OnRecvWarning", fromId));
		return;
	}
	userItem.inviteId = inviteId;
	userItem.setChatTypeWithTalkMsgType(charget, msgType);
	userItem.userName = fromName;
//		userItem.statusType = UserStatusType.USTATUS_ONLINE;
	SetUserOnlineStatus(userItem, UserStatusType.USTATUS_ONLINE);

	// 生成MessageItem
	LCMessageItem item = new LCMessageItem();
	item.init(mMsgIdIndex.getAndIncrement()
			, SendType.Recv
			, fromId
			, toId
			, userItem.inviteId
			, StatusType.Finish);
	// 生成WarningItem
	LCWarningItem warningItem = new LCWarningItem();
	warningItem.init(message);
	// 把WarningItem添加到MessageItem
	item.setWarningItem(warningItem);

	// 添加到用户聊天记录中
	userItem.insertSortMsgList(item);

	// callback
	mCallbackHandler.OnRecvWarning(item);
}

/**
 * 接收更新在线状态消息回调
 * @param userId
 * @param server
 * @param clientType
 * @param statusType
 */
@Override
public void OnUpdateStatus(
		string userId
		, string server
		, LiveChatClient.ClientType clientType
		, LiveChatClient.UserStatusType statusType)
{
	LCUserItem userItem = m_userMgr->getUserItem(userId);
	if (null == userItem) {
		Log.e("livechat", string.format("%s::%s() getUserItem fail, userId:%s", "LiveChatManager", "OnUpdateStatus", userId));
		return;
	}
	userItem.clientType = clientType;
	SetUserOnlineStatus(userItem, statusType);

	mCallbackHandler.OnUpdateStatus(userItem);
}

/**
 * 接收更新票根消息回调
 * @param fromId	发送者ID
 * @param ticket	票根
 */
@Override
public void OnUpdateTicket(string fromId, int ticket)
{
	// 不用处理
}

/**
 * 接收用户正在编辑消息回调
 * @param fromId	用户ID
 */
@Override
public void OnRecvEditMsg(string fromId)
{
	mCallbackHandler.OnRecvEditMsg(fromId);
}

/**
 * 接收聊天事件消息回调
 * @param userId	聊天对象ID
 * @param eventType	聊天事件
 */
@Override
public void OnRecvTalkEvent(string userId, TalkEventType eventType)
{
	LCUserItem userItem = m_userMgr->getUserItem(userId);
	if (null == userItem) {
		Log.e("livechat", string.format("%s::%s() getUserItem fail, userId:%s, eventType:%s", "LiveChatManager", "OnRecvTalkEvent", userId, eventType.name()));
		return;
	}
	userItem.setChatTypeWithEventType(eventType);
	mCallbackHandler.OnRecvTalkEvent(userItem);

	if (eventType == TalkEventType.NoMoney
		|| eventType == TalkEventType.VideoNoMoney)
	{
		BuildAndInsertWarningWithErrType(userItem, LiveChatErrType.NoMoney);
	}
}

/**
 * 接收试聊开始消息回调
 * @param toId		接收者ID
 * @param fromId	发起者ID
 * @param time		试聊时长
 */
@Override
public void OnRecvTryTalkBegin(string toId, string fromId, int time)
{
	// 改变用户聊天状态并回调
	LCUserItem userItem = m_userMgr->getUserItem(toId);
	if (null == userItem) {
		Log.e("livechat", string.format("%s::%s() getUserItem fail, toId:%s", "LiveChatManager", "OnRecvTryTalkBegin", toId));
		return;
	}
	userItem.chatType = ChatType.InChatUseTryTicket;
	mCallbackHandler.OnRecvTryTalkBegin(userItem, time);
}

/**
 * 接收试聊结束消息回调
 * @param userId	聊天对象ID
 */
@Override
public void OnRecvTryTalkEnd(string userId)
{
	// 改变用户聊天状态并回调
	LCUserItem userItem = m_userMgr->getUserItem(userId);
	if (null == userItem) {
		Log.e("livechat", string.format("%s::%s() getUserItem fail, userId:%s", "LiveChatManager", "OnRecvTryTalkEnd", userId));
		return;
	}
	userItem.chatType = ChatType.InChatCharge;
	mCallbackHandler.OnRecvTryTalkEnd(userItem);

	// 插入系统消息
	string message = mContext.getstring(R.string.livechat_msg_trychat_start_end);
	BuildAndInsertSystemMsg(userId, message);
}


/**
 * 接收邮件更新消息回调
 * @param fromId		发送者ID
 * @param noticeType	邮件类型
 */
@Override
public void OnRecvEMFNotice(string fromId, TalkEmfNoticeType noticeType)
{
	mCallbackHandler.OnRecvEMFNotice(fromId, noticeType);
}

/**
 * 接收被踢下线消息回调
 * @param kickType	被踢下线原因
 */
@Override
public void OnRecvKickOffline(KickOfflineType kickType)
{
	FileLog("livechat", "LiveChatManager::OnRecvKickOffline() kickType:%s", kickType.name());

	// 用户在其它地方登录，被踢下线
	if (kickType == KickOfflineType.OtherLogin)
	{
		// 设置不自动重登录
		mIsAutoRelogin = false;

		// LoginManager注销
		Message msg = Message.obtain();
		msg.what = LiveChatRequestOptType.LoginManagerLogout.ordinal();
		mHandler.sendMessage(msg);

		// 回调
		mCallbackHandler.OnRecvKickOffline(kickType);
	}

	FileLog("livechat", "LiveChatManager::OnRecvKickOffline() end");
}

/**
 * 接收图片消息回调
 * @param toId		接收者ID
 * @param fromId	发送者ID
 * @param fromName	发送者用户名
 * @param inviteId	邀请ID
 * @param photoId	图片ID
 * @param sendId	图片发送ID
 * @param charget	是否已付费
 * @param photoDesc	图片描述
 * @param ticket	票根
 */
@Override
public void OnRecvPhoto(
		string toId
		, string fromId
		, string fromName
		, string inviteId
		, string photoId
		, string sendId
		, bool charget
		, string photoDesc
		, int ticket)
{
	// 返回票根给服务器
	LiveChatClient.UploadTicket(fromId, ticket);

	// 添加用户到列表中（若列表中用户不存在）
	LCUserItem userItem = m_userMgr->getUserItem(fromId);
	if (null == userItem) {
		Log.e("livechat", string.format("%s::%s() getUserItem fail, fromId:%s", "LiveChatManager", "OnRecvPhoto", fromId));
		return;
	}
	userItem.inviteId = inviteId;
	userItem.userName = fromName;
//		userItem.statusType = UserStatusType.USTATUS_ONLINE;
	SetUserOnlineStatus(userItem, UserStatusType.USTATUS_ONLINE);

	// 生成MessageItem
	LCMessageItem item = new LCMessageItem();
	item.init(mMsgIdIndex.getAndIncrement()
			, SendType.Recv
			, fromId
			, toId
			, userItem.inviteId
			, StatusType.Finish);
	// 生成PhotoItem
	LCPhotoItem photoItem = new LCPhotoItem();
	photoItem.init(photoId
			, sendId
			, photoDesc
			, m_photoMgr->getPhotoPath(photoId, PhotoModeType.Fuzzy, PhotoSizeType.Large)
			, m_photoMgr->getPhotoPath(photoId, PhotoModeType.Fuzzy, PhotoSizeType.Middle)
			, m_photoMgr->getPhotoPath(photoId, PhotoModeType.Clear, PhotoSizeType.Original)
			, m_photoMgr->getPhotoPath(photoId, PhotoModeType.Clear, PhotoSizeType.Large)
			, m_photoMgr->getPhotoPath(photoId, PhotoModeType.Clear, PhotoSizeType.Middle)
			, charget);
	// 把PhotoItem添加到MessageItem
	item.setPhotoItem(photoItem);

	// 添加到用户聊天记录中
	userItem.insertSortMsgList(item);

	// callback
	mCallbackHandler.OnRecvPhoto(item);
}

/**
 * 接收微视频消息回调
 * @param toId		接收者ID
 * @param fromId	发送者ID
 * @param fromName	发送者用户名
 * @param inviteId	邀请ID
 * @param videoId	视频ID
 * @param sendId	发送ID
 * @param charget	是否已付费
 * @param videoDesc	视频描述
 * @param ticket	票根
 */
@Override
public void OnRecvVideo(
		string toId
		, string fromId
		, string fromName
		, string inviteId
		, string videoId
		, string sendId
		, bool charget
		, string videoDesc
		, int ticket)
{
	// 返回票根给服务器
	LiveChatClient.UploadTicket(fromId, ticket);

	// 不允许接收视频消息
	if (!mIsRecvVideoMsg) {
		return;
	}

	// 添加用户到列表中（若列表中用户不存在）
	LCUserItem userItem = m_userMgr->getUserItem(fromId);
	if (null == userItem) {
		Log.e("livechat", string.format("%s::%s() getUserItem fail, fromId:%s", "LiveChatManager", "OnRecvVideo", fromId));
		return;
	}
	userItem.inviteId = inviteId;
	userItem.userName = fromName;
	SetUserOnlineStatus(userItem, UserStatusType.USTATUS_ONLINE);

	// 生成MessageItem
	LCMessageItem item = new LCMessageItem();
	item.init(mMsgIdIndex.getAndIncrement()
			, SendType.Recv
			, fromId
			, toId
			, userItem.inviteId
			, StatusType.Finish);
	// 生成VideoItem
	LCVideoItem videoItem = new LCVideoItem();
	videoItem.init(videoId
			, sendId
			, videoDesc
			, ""
			, ""
			, ""
			, ""
			, charget);
	// 把PhotoItem添加到MessageItem
	item.setVideoItem(videoItem);

	// 添加到用户聊天记录中
	userItem.insertSortMsgList(item);

	// callback
	mCallbackHandler.OnRecvVideo(item);
}

// --------------- 高级表情下载回调 ---------------
/**
 * 下载高级表情图片回调
 * @param result		下载结果
 * @param emotionItem	高级表情item
 */
@Override
public void OnDownloadEmotionImage(bool result, LCEmotionItem emotionItem)
{
	mCallbackHandler.OnGetEmotionImage(result, emotionItem);
}

/**
 * 下载高级表情播放图片回调
 * @param result		下载结果
 * @param emotionItem	高级表情item
 */
@Override
public void OnDownloadEmotionPlayImage(bool result, LCEmotionItem emotionItem)
{
	mCallbackHandler.OnGetEmotionPlayImage(result, emotionItem);
}

/**
 * 下载高级表情3gp回调
 * @param result		下载结果
 * @param emotionItem	高级表情item
 */
@Override
public void OnDownloadEmotion3gp(bool result, LCEmotionItem emotionItem)
{
	mCallbackHandler.OnGetEmotion3gp(result, emotionItem);
}

// ----------------------- OnLoginManagerCallback -----------------------
/**
 * 通过 LoginItem 登录
 * @param loginItem
 */
public void LoginWithLoginItem(final LoginItem loginItem)
{
	ConfigManager.getInstance().GetOtherSynConfigItem(new OnConfigManagerCallback()
	{

		@Override
		public void OnGetOtherSynConfigItem(bool isSuccess, string errno,
				string errmsg, OtherSynConfigItem otherSynConfigItem)
		{
			// TODO Auto-generated method stub

			// livechat站点
			final ArrayList<string> ipList = new ArrayList<string>();
			// livechat port
			int port = -1;

			// 获取当前站点
			final WebSite website = WebSiteManager.newInstance(mContext).GetWebSite();
			if( website != null )
			{
				switch (website.getSiteId()) {
				case 1: {
					// cl
					ipList.add(otherSynConfigItem.cl.host);
					for( string site : otherSynConfigItem.cl.proxyHost ) {
						ipList.add(site);
					}
					port = otherSynConfigItem.cl.port;
				}break;
				case 2:{
					// ida
					ipList.add(otherSynConfigItem.ida.host);
					for( string site : otherSynConfigItem.ida.proxyHost ) {
						ipList.add(site);
					}
					port = otherSynConfigItem.ida.port;
				}break;
				case 4:{
					// cd
					ipList.add(otherSynConfigItem.ch.host);
					for( string site : otherSynConfigItem.ch.proxyHost ) {
						ipList.add(site);
					}
					port = otherSynConfigItem.ch.port;
				}break;
				case 8:{
					// ld
					ipList.add(otherSynConfigItem.la.host);
					for( string site : otherSynConfigItem.la.proxyHost ) {
						ipList.add(site);
					}
					port = otherSynConfigItem.la.port;
				}break;
				default:
					break;
				}

				final int portFinal = port;
				final string host = website.getWebSiteHost();
				if( ipList.size() > 0 && port != -1 )
				{
					// 初始化并登录livechat
					TelephonyManager tm = (TelephonyManager) mContext.getSystemService(Context.TELEPHONY_SERVICE);
					Logout();
					Init(
						ipList.toArray(new string[ipList.size()]),
						portFinal,
						host
						);
					Login(
						loginItem.manid,
						loginItem.sessionid,
						RequestJni.GetDeviceId(tm),
						loginItem.videoreceived
						);
				}
			}
		}
	});
}

/**
 * LoginManager回调（php登录回调）
 */
@Override
public void OnLogin(bool isSuccess, string errno, string errmsg,
		LoginItem item, LoginErrorItem errItem) {
	// TODO Auto-generated method stub
	if (isSuccess)
	{
		mRiskControl = item.livechat;
		if (!item.livechat) {
			// 登录成功且没有风控则登录LiveChat
			Message msg = Message.obtain();
			msg.what = LiveChatRequestOptType.LoginWithLoginItem.ordinal();
			msg.obj = item;
			mHandler.sendMessage(msg);
			// for test
//				mHandler.sendMessageDelayed(msg, 30*1000);
		}
	}
}

/**
 * LoginManager注销回调（php注销回调）
 */
@Override
public void OnLogout(bool bActive) {
	// TODO Auto-generated method stub
	if (bActive) {
		Logout();
	}
}

// ------------------- LCEmotionManagerCallback -------------------
void LiveChatManManager::OnDownloadEmotionImage(bool result, LCEmotionItem* emotionItem)
{

}

void LiveChatManManager::OnDownloadEmotionPlayImage(bool result, LCEmotionItem* emotionItem)
{

}

// ------------------- ILiveChatClientListener -------------------
// 客户端主动请求
void LiveChatManManager::OnLogin(LCC_ERR_TYPE err, const string& errmsg)
{

}

void LiveChatManManager::OnLogout(LCC_ERR_TYPE err, const string& errmsg)
{

}

void LiveChatManManager::OnSetStatus(LCC_ERR_TYPE err, const string& errmsg)
{

}

void LiveChatManManager::OnEndTalk(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg)
{

}

void LiveChatManManager::OnGetUserStatus(const UserIdList& inList, LCC_ERR_TYPE err, const string& errmsg, const UserStatusList& list)
{

}

void LiveChatManManager::OnGetTalkInfo(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const string& userId, const string& invitedId, bool charge, unsigned int chatTime)
{

}

void LiveChatManManager::OnSendMessage(const string& inUserId, const string& inMessage, int inTicket, LCC_ERR_TYPE err, const string& errmsg)
{

}

void LiveChatManManager::OnSendEmotion(const string& inUserId, const string& inEmotionId, int inTicket, LCC_ERR_TYPE err, const string& errmsg)
{

}

void LiveChatManManager::OnSendVGift(const string& inUserId, const string& inGiftId, int inTicket, LCC_ERR_TYPE err, const string& errmsg)
{

}

void LiveChatManManager::OnGetVoiceCode(const string& inUserId, int ticket, LCC_ERR_TYPE err, const string& errmsg, const string& voiceCode)
{

}

void LiveChatManManager::OnSendVoice(const string& inUserId, const string& inVoiceId, int inTicket, LCC_ERR_TYPE err, const string& errmsg)
{

}

void LiveChatManManager::OnUseTryTicket(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const string& userId, TRY_TICKET_EVENT tickEvent)
{

}

void LiveChatManManager::OnGetTalkList(int inListType, LCC_ERR_TYPE err, const string& errmsg, const TalkListInfo& talkListInfo)
{

}

void LiveChatManManager::OnSendPhoto(LCC_ERR_TYPE err, const string& errmsg, int ticket)
{

}

void LiveChatManManager::OnShowPhoto(LCC_ERR_TYPE err, const string& errmsg, int ticket)
{

}

void LiveChatManManager::OnGetUserInfo(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const UserInfoItem& userInfo)
{

}

void LiveChatManManager::OnGetUsersInfo(LCC_ERR_TYPE err, const string& errmsg, const UserInfoList& userList)
{

}

void LiveChatManManager::OnGetContactList(CONTACT_LIST_TYPE inListType, LCC_ERR_TYPE err, const string& errmsg, const TalkUserList& list)
{

}

void LiveChatManManager::OnGetBlockUsers(LCC_ERR_TYPE err, const string& errmsg, const list<string>& users)
{

}

void LiveChatManManager::OnSearchOnlineMan(LCC_ERR_TYPE err, const string& errmsg, const list<string>& userList)
{

}

void LiveChatManManager::OnReplyIdentifyCode(LCC_ERR_TYPE err, const string& errmsg)
{

}

void LiveChatManManager::OnGetRecentContactList(LCC_ERR_TYPE err, const string& errmsg, const list<string>& userList)
{

}

void LiveChatManManager::OnGetFeeRecentContactList(LCC_ERR_TYPE err, const string& errmsg, const list<string>& userList)
{

}

void LiveChatManManager::OnGetLadyChatInfo(LCC_ERR_TYPE err, const string& errmsg, const list<string>& chattingList, const list<string>& chattingInviteIdList, const list<string>& missingList, const list<string>& missingInviteIdList)
{

}

void LiveChatManManager::OnPlayVideo(LCC_ERR_TYPE err, const string& errmsg, int ticket)
{

}

// 服务器主动请求
void LiveChatManManager::OnRecvMessage(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, int ticket, TALK_MSG_TYPE msgType, const string& message)
{

}

void LiveChatManManager::OnRecvEmotion(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, int ticket, TALK_MSG_TYPE msgType, const string& emotionId)
{

}

void LiveChatManManager::OnRecvVoice(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, TALK_MSG_TYPE msgType, const string& voiceId, const string& fileType, int timeLen)
{

}

void LiveChatManManager::OnRecvWarning(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, int ticket, TALK_MSG_TYPE msgType, const string& message)
{

}

void LiveChatManManager::OnUpdateStatus(const string& userId, const string& server, CLIENT_TYPE clientType, USER_STATUS_TYPE statusType)
{

}

void LiveChatManManager::OnUpdateTicket(const string& fromId, int ticket)
{

}

void LiveChatManManager::OnRecvEditMsg(const string& fromId)
{

}

void LiveChatManManager::OnRecvTalkEvent(const string& userId, TALK_EVENT_TYPE eventType)
{

}

void LiveChatManManager::OnRecvTryTalkBegin(const string& toId, const string& fromId, int time)
{

}

void LiveChatManManager::OnRecvTryTalkEnd(const string& userId)
{

}

void LiveChatManManager::OnRecvEMFNotice(const string& fromId, TALK_EMF_NOTICE_TYPE noticeType)
{

}

void LiveChatManManager::OnRecvKickOffline(KICK_OFFLINE_TYPE kickType)
{

}

void LiveChatManManager::OnRecvPhoto(const string& toId, const string& fromId, const string& fromName, const string& inviteId, const string& photoId, const string& sendId, bool charge, const string& photoDesc, int ticket)
{

}

void LiveChatManManager::OnRecvLadyVoiceCode(const string& voiceCode)
{

}

void LiveChatManManager::OnRecvIdentifyCode(const unsigned char* data, long dataLen)
{

}

void LiveChatManManager::OnRecvVideo(const string& toId, const string& fromId, const string& fromName, const string& inviteId, const string& videoId, const string& sendId, bool charge, const string& videoDesc, int ticket)
{

}

string LiveChatManManager::GetLogPath()
{
	return m_dirPath + LOG_DIR;
}
