/*
 * author: Samson.Fan
 *   date: 2015-03-19
 *   file: LiveChatManManager.cpp
 *   desc: LiveChat客户端实现类
 */

#include "LiveChatManManager.h"
#include <httpclient/HttpRequestDefine.h>
#include <common/CommonFunc.h>
#include <common/KLog.h>
#include <common/IAutoLock.h>
#include <common/CommonFunc.h>
#include <common/CheckMemoryLeak.h>

#include <livechat/ILiveChatClient.h>

// 消息管理器
#include "LCTextManager.h"
#include "LCEmotionManager.h"
#include "LCVoiceManager.h"
#include "LCPhotoManager.h"
#include "LCVideoManager.h"
#include "LCWarningItem.h"

// 用户管理器
#include "LCUserManager.h"
#include "LCInviteManager.h"
#include "LCBlockManager.h"
#include "LCContactManager.h"

#include "DeviceTypeDef.h"
static const int s_msgIdBegin = 1;		// 消息ID起始值

const char* LOG_DIR = "log/";						// log目录路径
const char* HTTP_LOG_DIR = LOG_DIR;					// http log目录路径
const char* HTTP_COOKIE_DIR = "cookie/";			// http cookie目录路径
const char* EMOTION_DIR = "livechat/emotion";		// 高级表情目录路径
const char* VOICE_DIR = "livechat/voice";			// 语音目录路径
const char* PHOTO_DIR = "livechat/photo";			// 图片目录路径
const char* PHOTO_TEMP_DIR = "livechat/temp/photo";	// 图片临时目录路径
const char* VIDEO_DIR = "livechat/video";			// 视频目录路径
const char* MAGICICON_DIR = "livechat/magicicon";   //小高级表情目录路径

LiveChatManManager::LiveChatManManager()
{
	m_listener = NULL;

	m_dirPath = "";
	m_port = 0;
	m_siteType = OTHER_SITE_UNKNOW;
	m_appVer = "";

	m_userId = "";
	m_sId = "";
    m_clientType = CLIENT_UNKNOW;
	m_deviceId = "";
	m_riskControl = false;
	m_isRecvVideoMsg = true;
	m_isLogin = false;
    m_isGetHistory = false;
    m_isResetParam = false;
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
    m_magicIconMgr = new LCMagicIconManager; //小高级表情管理器

	// 用户管理器
	m_userMgr = new LCUserManager;			// 用户管理器
	m_inviteMgr = new LCInviteManager;		// 邀请管理器
	m_blockMgr = new LCBlockManager;		// 黑名单管理器
	m_contactMgr = new LCContactManager;	// 联系人管理器

	// 请求线程
	m_requestThread = NULL;
	m_requestThreadStart = false;
	m_requestQueueLock = IAutoLock::CreateAutoLock();
	m_requestQueueLock->Init();
	// 请求管理器及控制器
	m_httpRequestManager = NULL;
	m_httpRequestHostManager = NULL;
	m_requestController = NULL;
	m_requestOtherController = NULL;
}

LiveChatManManager::~LiveChatManManager()
{
	ILiveChatClient::ReleaseClient(m_client);

	StopRequestThread();
	IAutoLock::ReleaseAutoLock(m_requestQueueLock);

	delete m_textMgr;
	delete m_emotionMgr;
	delete m_voiceMgr;
	delete m_photoMgr;
	delete m_videoMgr;
    // 释放小高级表情管理器
    delete m_magicIconMgr;

	delete m_userMgr;
	delete m_inviteMgr;
	delete m_blockMgr;
	delete m_contactMgr;

	delete m_requestController;
	delete m_requestOtherController;

	delete m_httpRequestManager;
	delete m_httpRequestHostManager;
}

// 设置本地缓存目录路径
bool LiveChatManManager::SetDirPath(const string& path)
{
	bool result = false;

	FileLog("LiveChatManager", "SetDirPath() path:%s", path.c_str());

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

		// http log目录
		string httpLogPath = m_dirPath + HTTP_LOG_DIR;
		HttpClient::SetLogDirectory(httpLogPath);
		// http cookie目录
		string httpCookiePath = m_dirPath + HTTP_COOKIE_DIR;
		HttpClient::SetCookiesDirectory(httpCookiePath);

		// log目录
		string logPath = m_dirPath + LOG_DIR;
		KLog::SetLogDirectory(logPath);

		// 设置emotion manager本地缓存目录
		string emotionPath = m_dirPath + EMOTION_DIR;
		result = result && m_emotionMgr->SetDirPath(emotionPath, logPath);

		FileLog("LiveChatManager", "SetDirPath() m_emotionMgr->SetDirPath() logPath:%s, result:%d", logPath.c_str(), result);

		// 设置voice manager本地缓存目录
		string voicePath = m_dirPath + VOICE_DIR;
		result = result && m_voiceMgr->Init(voicePath);

		FileLog("LiveChatManager", "SetDirPath() m_voiceMgr->Init() voicePath:%s, result:%d", voicePath.c_str(), result);

		// 设置photo manager本地缓存目录
		string photoPath = m_dirPath + PHOTO_DIR;
		result = result && m_photoMgr->SetDirPath(photoPath);
        
        FileLog("LiveChatManager", "SetDirPath() m_photoMgr->SetDirPath() photoPath:%s, result:%d", photoPath.c_str(), result);
        
        // 设置photo manager临时本地缓存目录
        string photoTempPath = m_dirPath + PHOTO_TEMP_DIR;
        result = result && m_photoMgr->SetTempDirPath(photoTempPath);

		FileLog("LiveChatManager", "SetDirPath() m_photoMgr->SetTempDirPath() photoTempPath:%s, result:%d", photoTempPath.c_str(), result);

		// 设置video manager本地缓存目录
		string videoPath = m_dirPath + VIDEO_DIR;
		result = result && m_videoMgr->SetDirPath(videoPath);

		FileLog("LiveChatManager", "SetDirPath() m_videoMgr->SetDirPath() videoPath:%s, result:%d", videoPath.c_str(), result);
        
        // 设置magicIcon manager本地缓存目录
        string MagicIconPath = m_dirPath + MAGICICON_DIR;
        result = result && m_magicIconMgr->SetDirPath(MagicIconPath, logPath);
        
        FileLog("LiveChatManager", "SetDirPath() m_magicIconMgr->SetDirPath() MagicIconPath:%s, logPath:%s, result:%d", MagicIconPath.c_str(), logPath.c_str(), result);
	}
	return result;
}

// 初始化
bool LiveChatManManager::Init(list<string> ipList
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
							, ILiveChatManManagerListener* listener)
{
	bool result = false;

	FileLog("LiveChatManager", "Init() begin, ipList.size:%d, port:%d, webHost:%s"
			, ipList.size(), port, webHost.c_str());

	if (!ipList.empty()
		&& port > 0
		&& !webHost.empty()
		&& !appHost.empty()
		&& !chatVoiceHost.empty()
		&& !appVer.empty()
		&& siteType != OTHER_SITE_UNKNOW
		&& SetDirPath(path))
	{
		FileLog("LiveChatManager", "Init() emotionMgr->Init() begin");
		result = m_emotionMgr->Init(webHost, this);
		FileLog("LiveChatManager", "Init() emotionMgr->Init() result:%d", result);
		result = result && m_photoMgr->Init(this);
		FileLog("LiveChatManager", "Init() m_photoMgr->Init() result:%d", result);
		result = result && m_videoMgr->Init(this, m_userMgr);
		FileLog("LiveChatManager", "Init() m_videoMgr->Init() result:%d", result);
        result = result && m_inviteMgr->Init(m_userMgr, m_blockMgr, m_contactMgr, m_client);
        FileLog("LiveChatManager", "Init() Mm_inviteMgr->Init() result:%d", result);
        //初始化小高级表情管理器 alex 2016-09-09
        result = result && m_magicIconMgr->Init(webHost, this);
        FileLog("LiveChatManager", "Init() magicIconMgr->Init() result:%d", result);
		result = result && m_client->Init(ipList, port, this);
		FileLog("LiveChatManager", "Init() client->Init() result:%d", result);
		if (result) {
			delete m_httpRequestHostManager;
			m_httpRequestHostManager = new HttpRequestHostManager();
			result = (NULL != m_httpRequestHostManager);
		}
		FileLog("LiveChatManager", "Init() new HttpRequestHostManager result:%d", result);
		if (result) {
			delete m_httpRequestManager;
			m_httpRequestManager = new HttpRequestManager();
			if (NULL != m_httpRequestManager) {
				m_httpRequestManager->SetHostManager(m_httpRequestHostManager);
			}
			result = (NULL != m_httpRequestManager);
		}
		FileLog("LiveChatManager", "Init() new HttpRequestManager result:%d", result);
		if (result) {
			delete m_requestController;
			m_requestController = new RequestLiveChatController(m_httpRequestManager, this);
			result = (NULL != m_requestController);
		}
		FileLog("LiveChatManager", "Init() new requestLiveChatController result:%d", result);
		if (result) {
			delete m_requestOtherController;
			m_requestOtherController = new RequestOtherController(m_httpRequestManager, this);
			result = (NULL != m_requestOtherController);
		}
		FileLog("LiveChatManager", "Init() new requestOtherController result:%d", result);
	}

	if (result)
	{
		// 设置http参数
		m_httpRequestManager->SetVersionCode(appVer);
		if (!httpUser.empty()) {
			m_httpRequestManager->SetAuthorization(httpUser, httpPassword);
		}
		m_httpRequestHostManager->SetWebSite(webHost);
		m_httpRequestHostManager->SetAppSite(appHost);
		m_httpRequestHostManager->SetChatVoiceSite(chatVoiceHost);

		// 设置高级表情管理器的http参数
		m_emotionMgr->SetAuthorization(httpUser, httpPassword);
        
        // 设置高级表情管理器的http参数
        m_magicIconMgr->SetAuthorization(httpUser, httpPassword);

		// 设置图片管理器的http控制器
		m_photoMgr->SetHttpRequest(m_httpRequestManager, m_requestController);

		// 设置视频管理器的http参数
		m_videoMgr->SetAuthorization(httpUser, httpPassword);
		// 设置视频管理器的http控制器
		m_videoMgr->SetHttpRequest(m_httpRequestManager, m_requestController);

		// 设置参数
		m_listener = listener;
		m_siteType = siteType;
		m_appVer = appVer;
		m_minBalance = minBalance;

		// 清除资源文件
		RemoveSourceFile();
	}

	FileLog("LiveChatManager", "Init() end, result:%d", result);
	return result;
}

// 重置参数（用于非自动登录时重置参数）
void LiveChatManManager::ResetParamWithNotAutoLogin()
{
    if (m_isResetParam) {
        // 停止请求队列处理线程
        StopRequestThread();
        // 清空请求队列
        CleanRequestTask();
        
        // 停止所有请求中的操作
        if (NULL != m_httpRequestManager) {
            m_httpRequestManager->StopAllRequest(true);
        }

        m_userId = "";
        m_sId = "";
        m_deviceId = "";
        m_riskControl = false;
        m_isRecvVideoMsg = true;
        m_msgIdBuilder.Init(s_msgIdBegin);

        FileLog("LiveChatManager", "ResetParamWithNotAutoLogin() clear emotion begin");
        // 停止获取高级表情配置请求
        if (HTTPREQUEST_INVALIDREQUESTID != m_emotionMgr->m_emotionConfigReqId) {
    //			RequestJni.StopRequest(m_emotionMgr->m_emotionConfigReqId);
            m_emotionMgr->m_emotionConfigReqId = HTTPREQUEST_INVALIDREQUESTID;
        }
        FileLog("LiveChatManager", "ResetParamWithNotAutoLogin() clear emotion StopAllDownloadImage");
        m_emotionMgr->StopAllDownloadImage();
        // 停止播放文件下载(注意之前没有调用这个，可能漏了，之后要注意添加后会不会有问题)
        m_emotionMgr->StopAllDownloadPlayImage();
        FileLog("LiveChatManager", "ResetParamWithNotAutoLogin() clear emotion removeAllSendingItems");
        m_emotionMgr->RemoveAllSendingItems();
        FileLog("LiveChatManager", "ResetParamWithNotAutoLogin() clear emotion ClearAllDownloader");
        m_emotionMgr->ClearAllDownloader();

        FileLog("LiveChatManager", "ResetParamWithNotAutoLogin() clear photo begin");
        // 停止所有图片请求
        m_photoMgr->ClearAllRequestItems();
        FileLog("LiveChatManager", "ResetParamWithNotAutoLogin() clear photo clearAllSendingItems");
        m_photoMgr->ClearAllSendingItems();
        FileLog("LiveChatManager", "ResetParamWithNotAutoLogin() clear photo ClearAllDownload");
        m_photoMgr->ClearAllDownload();
        FileLog("LiveChatManager", "ResetParamWithNotAutoLogin() clear photo ClearPhotoMap");
        m_photoMgr->ClearPhotoMap();
        FileLog("LiveChatManager", "ResetParamWithNotAutoLogin() clear photo ClearBindMap");
        m_photoMgr->ClearBindMap();

        FileLog("LiveChatManager", "ResetParamWithNotAutoLogin() clear voice begin");
        // 停止所有语音请求
        m_voiceMgr->ClearAllRequestItem();
    //		ArrayList<Long> voiceRequestIds = m_voiceMgr->clearAllRequestItem();
    //		if (NULL != voiceRequestIds) {
    //			for (Iterator<Long> iter = voiceRequestIds.iterator(); iter.hasNext(); ) {
    //				long requestId = iter.next();
    //				RequestJni.StopRequest(requestId);
    //			}
    //		}
        FileLog("LiveChatManager", "ResetParamWithNotAutoLogin() clear voice clearAllSendingItems");
        m_voiceMgr->ClearAllSendingItems();

        FileLog("LiveChatManager", "ResetParamWithNotAutoLogin() clear video begin");
        // 停止所有视频请求
        m_videoMgr->ClearAllDownloadVideoPhoto();
        FileLog("LiveChatManager", "ResetParamWithNotAutoLogin() clear video ClearAllDownloadVideoPhoto");
        m_videoMgr->ClearAllDownloadVideo();
        FileLog("LiveChatManager", "ResetParamWithNotAutoLogin() clear video ClearAllDownloadVideo");
        m_videoMgr->ClearAllPhotoFee();

        FileLog("LiveChatManager", "ResetParamWithNotAutoLogin() clear other begin");
        m_textMgr->RemoveAllSendingItems();
        FileLog("LiveChatManager", "ResetParamWithNotAutoLogin() clear other removeAllUserItem");
        m_userMgr->RemoveAllUserItem();
        
        // 停止所有小高表请求
        if(HTTPREQUEST_INVALIDREQUESTID != m_magicIconMgr->m_magicIconConfigReqId){
            m_magicIconMgr->m_magicIconConfigReqId = HTTPREQUEST_INVALIDREQUESTID;
        }
        // 停止所有下载的母子图
        m_magicIconMgr->StopAllDownloadThumbImage();
        // 停止所有下载的原图
        m_magicIconMgr->StopAllDownloadSourceImage();
        // 删除所有待发送的message
        m_magicIconMgr->RemoveAllSendingItems();
        // 清除所有的小高级表情的下载器
        m_magicIconMgr->ClearAllDownloader();
        
        // 检测试聊券http request
        m_checkCouponOptMap.lock();
        m_checkCouponOptMap.clear();
        m_checkCouponOptMap.unlock();

        // 清除HttpClient的cookies
        HttpClient::CleanCookies();
        m_cookies.clear();
    }
}

// 重置参数（用于自动登录时重置参数）
void LiveChatManManager::ResetParamWithAutoLogin()
{
    // 删除图片关联
    m_photoMgr->ClearAllRequestItems();
    m_photoMgr->ClearAllDownload();
    m_photoMgr->ClearBindMap();
    m_isGetHistory = false;
//   LCUserList userList = m_userMgr->GetChatingUsers();
//    for(LCUserList::iterator itr = userList.begin(); itr != userList.end(); itr++) {
//        LCUserItem *userItem = *itr;
//        userItem->m_getHistory = false;
//    }
}

// 登录
bool LiveChatManManager::Login(const string& userId, const string& sid, CLIENT_TYPE clientType, const list<string>& cookies, const string& deviceId, bool isRecvVideoMsg)
{
	string strCookies("");
	if (!cookies.empty()) {
		for (list<string>::const_iterator iter = cookies.begin();
			iter != cookies.end();
			iter++)
		{
			if (!strCookies.empty()) {
				strCookies += ", ";
			}
			strCookies += (*iter);
		}
	}

	FileLog("LiveChatManager", "Login() begin, userId:%s, sid:%s, clientType:%d, deviceId:%s, m_isLogin:%d, cookies:%s"
			, userId.c_str(), sid.c_str(), clientType, deviceId.c_str(), m_isLogin, strCookies.c_str());

	bool result = false;
	if (m_isLogin) {
		result = m_isLogin;
	}
	else {
		if (!m_isAutoLogin) {
			// 重置参数
			ResetParamWithNotAutoLogin();
		}

		// LiveChat登录
		result = m_client->Login(userId, sid, deviceId, clientType, USER_SEX_MALE);
		if (result && !m_isAutoLogin)
		{
			// 启动请求队列处理线程
			StartRequestThread();
			// 设置参数
			m_isAutoLogin = true;
			m_userId = userId;
			m_sId = sid;
            m_clientType = clientType;
			m_deviceId = deviceId;
			m_isRecvVideoMsg = isRecvVideoMsg;
			m_cookies = cookies;
			HttpClient::SetCookiesInfo(cookies);

			{
				// for test
				string strCookies("");
				list<string> getCookies = HttpClient::GetCookiesInfo();
				FileLog("LiveChatManager", "Login() cookies.size:%d", getCookies.size());

				for (list<string>::const_iterator iter = getCookies.begin();
					iter != getCookies.end();
					iter++)
				{
					if (!strCookies.empty()) {
						strCookies += ", ";
					}

					strCookies += (*iter);
				}

				FileLog("LiveChatManager", "Login() GetCookiesInfo() cookies:%s", strCookies.c_str());
			}
		}
	}

	FileLog("LiveChatManager", "Login() end, userId:%s, result:%d", userId.c_str(), result);
	return result;
}

// 注销
bool LiveChatManManager::Logout(bool isResetParam)
{
	FileLog("LiveChatManager", "Logout() begin");
    
    // 设置是否重置参数
    m_isResetParam = isResetParam;

    // 设置不自动重登录
	m_isAutoLogin = false;
    
	bool result =  m_client->Logout();

	FileLog("LiveChatManager", "Logout() end, isResetParam:%b, result:%b", isResetParam, result);

	return result;
}

// 重新登录
bool LiveChatManManager::Relogin()
{
    bool result =  m_client->Logout();
    
    // 立即重登录
    long time = 1 * 1000;	// 每1秒重登录一次
    InsertRequestTask(REQUEST_TASK_AutoRelogin, 0, time);
    
    return result;
}

// 清除所有图片、视频等资源文件
void LiveChatManManager::RemoveSourceFile()
{
	// 清除图片文件
	m_photoMgr->RemoveAllPhotoFile();
    // 清除图片临时文件
    m_photoMgr->RemoveAllTempPhotoFile();
	// 清除视频文件
	m_videoMgr->RemoveAllVideoFile();
}

// 设置用户在线状态，若用户在线状态改变则callback通知listener
void LiveChatManManager::SetUserOnlineStatus(LCUserItem* userItem, USER_STATUS_TYPE statusType)
{
	if (userItem->SetUserOnlineStatus(statusType))
	{
		m_listener->OnChangeOnlineStatus(userItem);
	}
}

// 根据错误类型设置用户在线状态，若用户在线状态改变则callback通知listener
void LiveChatManManager::SetUserOnlineStatusWithLccErrType(LCUserItem* userItem, LCC_ERR_TYPE errType)
{
    if (LCC_ERR_SUCCESS == errType) {
        SetUserOnlineStatus(userItem, USTATUS_ONLINE);
    }
    else if (LCC_ERR_SIDEOFFLINE == errType) {
        SetUserOnlineStatus(userItem, USTATUS_OFFLINE_OR_HIDDEN);
    }
}

// 是否自动重登录
bool LiveChatManManager::IsAutoRelogin(LCC_ERR_TYPE err)
{
	if (m_isAutoLogin)
	{
		m_isAutoLogin = (LCC_ERR_CONNECTFAIL == err);
	}
	return m_isAutoLogin;
}

// 插入重登录任务
void LiveChatManManager::InsertReloginTask()
{
	FileLog("LiveChatManager", "InsertReloginTask() begin");

	long time = 30 * 1000;	// 每30秒重登录一次
	InsertRequestTask(REQUEST_TASK_AutoRelogin, 0, time);

	FileLog("LiveChatManager", "InsertReloginTask() end");
}

// 自动重登录
void LiveChatManManager::AutoRelogin()
{
	FileLog("LiveChatManager", "AutoRelogin() begin, m_userId:%s, m_sId:%s, m_deviceId:%s"
			, m_userId.c_str(), m_sId.c_str(), m_deviceId.c_str());

	if (!m_userId.empty()
		&& !m_sId.empty()
		&& !m_deviceId.empty())
	{
		Login(m_userId, m_sId, m_clientType, m_cookies, m_deviceId, m_isRecvVideoMsg);
	}

	FileLog("LiveChatManager", "AutoRelogin() end");
}

///**
// * 注销
// * @return
// */
//public bool Logout()
//{
//	FileLog("LiveChatManager", "Logout() begin");
//
//	// 设置不自动重登录
//	m_isAutoLogin = false;
//	bool result =  m_client->Logout();
//
//	FileLog("LiveChatManager", "Logout() end, result:%b", result);
//
//	return result;
//}

// 是否已经登录
bool LiveChatManManager::IsLogin()
{
	return m_isLogin;
}

// 是否获取历史记录
bool LiveChatManManager::IsGetHistory()
{
    return m_isGetHistory;
}


/**
 * 是否处理发送操作
 * @return
 */
bool LiveChatManManager::IsHandleSendOpt()
{
	bool result = false;
	if (!m_riskControl
		 && m_isAutoLogin)
	{
		// 没有风控且自动重连
		result = true;
	}
	return result;
}

// 检测试聊券，或者马上发送消息
bool LiveChatManManager::SendMsgWithCoupon(LCUserItem* userItem, LCMessageItem* msgItem) {
    bool result = false;
    
    if (IsSendMessageNow(userItem))
    {
        if (!IsCheckCoupon(userItem)) {
            // 立即发送
            SendMessageItem(msgItem);
        }
        else {
            // 把消息添加到待发列表
            userItem->InsertSendMsgList(msgItem);
            // 执行尝试使用试聊券流程
            CheckCouponProc(userItem);
        }
        
        result = true;
    }
    else if (m_isAutoLogin)
    {
        // 把消息添加到待发列表
        userItem->InsertSendMsgList(msgItem);
        result = true;
    }
    
    return result;
}
// 是否立即发送消息给用户
bool LiveChatManManager::IsSendMessageNow(LCUserItem* userItem)
{
	bool result = false;
	if (NULL != userItem)
	{
        
//        if (!userItem->m_getHistory){
//            result = false;
//        }else{
            // 已经登录及聊天状态为inchat或男士邀请
            result = IsLogin() && IsGetHistory();
//        }

	}
	return result;
}

// 是否检测试聊券
bool LiveChatManManager::IsCheckCoupon(LCUserItem* userItem)
{
    bool result = false;
    result = !(userItem->m_chatType == LCUserItem::LC_CHATTYPE_IN_CHAT_CHARGE
                 || userItem->m_chatType == LCUserItem::LC_CHATTYPE_IN_CHAT_USE_TRY_TICKET
                 || userItem->m_chatType == LCUserItem::LC_CHATTYPE_MANINVITE );
    return result;
}

///**
// * 设置在线状态
// * @param statusType	在线状态
// * @return
// */
//public bool SetStatus(UserStatusType statusType)
//{
//	return m_client->SetStatus(statusType);
//}

// 获取用户状态(多个)
bool LiveChatManManager::GetUserStatus(const list<string>& userIds)
{
    bool result = false;
    if (IsLogin()) {
        result = m_client->GetUserStatus(userIds);
    }
    return result;
}

// 获取用户信息
bool LiveChatManManager::GetUserInfo(const string& userId)
{
    bool result = false;
    if (IsLogin()) {
        result = m_client->GetUserInfo(userId);
    }
    return result;
}

// 获取多个用户信息
bool LiveChatManManager::GetUsersInfo(const list<string>& userList)
{
    bool result = false;
    if (IsLogin()) {
        result = m_client->GetUsersInfo(userList) >= 0;
    }
    return result;
}

// 获取邀请的用户列表（使用前需要先完成GetTalkList()调用）
LCUserList LiveChatManManager::GetInviteUsers()
{
	return m_userMgr->GetInviteUsers();
}

// 获取最后一个邀请用户
LCUserItem* LiveChatManManager::GetLastInviteUser()
{
	LCUserItem* userItem = NULL;
	LCUserList inviteUsers = m_userMgr->GetInviteUsers();
	if (inviteUsers.begin() != inviteUsers.end())
	{
		userItem = (*inviteUsers.begin());
	}
	return userItem;
}

// 获取在聊用户列表（使用前需要先完成GetTalkList()调用）
LCUserList LiveChatManManager::GetChatingUsers()
{
	return m_userMgr->GetChatingUsers();
}

// 获取用户最后一条聊天消息
LCMessageItem* LiveChatManManager::GetLastMessage(const string& userId)
{
    LCMessageItem* msgItem = NULL;
    LCUserItem* userItem = m_userMgr->GetUserItem(userId);
    if (NULL != userItem) {
        LCMessageList msgList = userItem->GetMsgList();
        if (!msgList.empty()) {
            for (LCMessageList::const_reverse_iterator iter = msgList.rbegin();
                 iter != msgList.rend();
                 iter++)
            {
                if ((*iter)->IsChatMessage()) {
                    msgItem = (*iter);
                    break;
                }
            }
        }
    }
    return msgItem;
}


// 获取用户最后一条聊天消息
LCMessageItem* LiveChatManManager::GetTheOtherLastMessage(const string& userId)
{
    LCMessageItem* msgItem = NULL;
    LCUserItem* userItem = m_userMgr->GetUserItem(userId);
    if (NULL != userItem) {
        msgItem = userItem->GetTheOtherLastMessage();
    }
    return msgItem;
}
///**
// * 检测用户帐号是否有足够点数
// * @return
// */
//private bool CheckMoney(final string userId)
//{
//	ConfigManager.getInstance().GetOtherSynConfigItem(new OnConfigManagerCallback() {
//
//		@Override
//		public void OnGetOtherSynConfigItem(bool isSuccess, string errno,
//				string errmsg, OtherSynConfigItem item) {
//			// TODO Auto-generated method stub
//			double money = 0.5;
//			if (isSuccess) {
//				if (WebSiteManager.getInstance().GetWebSite().getSiteKey().compareTo(WebSiteType.ChnLove.name()) == 0) {
//					money = item.cl.minChat;
//				}
//				else if (WebSiteManager.getInstance().GetWebSite().getSiteKey().compareTo(WebSiteType.CharmDate.name()) == 0) {
//					money = item.ch.minChat;
//				}
//				else if (WebSiteManager.getInstance().GetWebSite().getSiteKey().compareTo(WebSiteType.IDateAsia.name()) == 0) {
//					money = item.ida.minChat;
//				}
//				else if (WebSiteManager.getInstance().GetWebSite().getSiteKey().compareTo(WebSiteType.LatamDate.name()) == 0) {
//					money = item.la.minChat;
//				}
//			}
//			final double minMoney = money;
//
//			long reuqestId = RequestJniOther.GetCount(true, false, false, false, false, false, new OnOtherGetCountCallback() {
//
//				@Override
//				public void OnOtherGetCount(bool isSuccess, string errno, string errmsg,
//						OtherGetCountItem item) {
//					// TODO Auto-generated method stub
//					if (isSuccess) {
//						if (item.money >= minMoney) {
//							// 若当前状态为Other，则标记为ManInvite(男士邀请)状态
//							LCUserItem* userItem = m_userMgr->GetUserItem(userId);
//							if (NULL != userItem) {
//								if (userItem->m_chatType == ChatType.Other
//									|| userItem->m_chatType == ChatType.Invite)
//								{
//									userItem->m_chatType = ChatType.ManInvite;
//								}
//							}
//							// 余额足够，发送待发消息
//							Message msg = Message.obtain();
//							msg.what = LiveChatRequestOptType.SendMessageList.ordinal();
//							msg.obj = userId;
//							mHandler.sendMessage(msg);
//						}
//						else {
//							// 返回发送消息余额不足，发送失败
//							Message msgSendMsg = Message.obtain();
//							msgSendMsg.what = LiveChatRequestOptType.SendMessageListNoMoneyFail.ordinal();
//							msgSendMsg.obj = userId;
//							mHandler.sendMessage(msgSendMsg);
//						}
//					}
//					else {
//						// 请求失败
//						Message msgSendMsg = Message.obtain();
//						msgSendMsg.what = LiveChatRequestOptType.SendMessageListConnectFail.ordinal();
//						msgSendMsg.obj = userId;
//						mHandler.sendMessage(msgSendMsg);
//					}
//				}
//			});
//
//			if (HTTPREQUEST_INVALIDREQUESTID == reuqestId) {
//				// 请求失败，返回发送消息余额不足失败
//				Message msgSendMsg = Message.obtain();
//				msgSendMsg.what = LiveChatRequestOptType.SendMessageListConnectFail.ordinal();
//				msgSendMsg.obj = userId;
//				mHandler.sendMessage(msgSendMsg);
//			}
//		}
//	});
//	return true;
//}

// 检测是否可使用试聊券
bool LiveChatManManager::CheckCoupon(const string& userId)
{
	bool result = false;

	FileLog("LiveChatManager", "CheckCoupon() userId:%s", userId.c_str());

	if (NULL != m_requestController
		&& !userId.empty())
	{
		long requestId = m_requestController->CheckCoupon(userId);
		result = (requestId != HTTPREQUEST_INVALIDREQUESTID);
		if (result) {
			m_checkCouponOptMap.lock();
			m_checkCouponOptMap.insert(map_lock<long, long>::value_type(requestId, requestId));
			m_checkCouponOptMap.unlock();
		}
		FileLog("LiveChatManager", "CheckCoupon() userId:%s, requestId:%d, result:%d", userId.c_str(), requestId, result);
	}

	FileLog("LiveChatManager", "CheckCoupon() userId:%s, end", userId.c_str());

	return result;
}

///**
// * 查询是否能使用试聊
// * @param userId	对方用户ID
// * @return
// */
//private bool CheckCouponProc(LCUserItem* userItem) {
//	// 执行尝试使用试聊券流程
//	if (!userItem.tryingSend) {
//		userItem.tryingSend = true;
//		long requestId = RequestOperator.getInstance().CheckCoupon(userItem->m_userId, new OnCheckCouponCallCallback() {
//
//			@Override
//			public void OnCheckCoupon(long requestId, bool isSuccess, string errno, string errmsg,
//					Coupon item) {
//				// TODO Auto-generated method stub
//				if (isSuccess && item.status == CouponStatus.Yes)
//				{
//					// 尝试使用试聊券
//					Message msg = Message.obtain();
//					msg.what = LiveChatRequestOptType.TryUseTicket.ordinal();
//					msg.obj = item;
//					mHandler.sendMessage(msg);
//				}
//				else {
//					// 检测是否有点
//					Message msg = Message.obtain();
//					msg.what = LiveChatRequestOptType.GetCount.ordinal();
//					msg.obj = item;
//					mHandler.sendMessage(msg);
//				}
//			}
//		});
//		return requestId != HTTPREQUEST_INVALIDREQUESTID;
//	}
//	else {
//		return true;
//	}
//}
///**
// * 结束对话
// * @param userId	对方的用户ID
// * @return
// */
//public bool EndTalk(string userId)
//{
//	return m_client->EndTalk(userId);
//}

// 获取单个用户历史聊天记录（包括文本、高级表情、语音、图片）
bool LiveChatManManager::GetHistoryMessage(const string& userId)
{
	bool result = false;
	LCUserItem* userItem = m_userMgr->GetUserItem(userId);
	if (NULL != userItem) {
		if (!userItem->GetMsgList().empty()
			&& m_getUsersHistoryMsgRequestId == HTTPREQUEST_INVALIDREQUESTID) // 已完成获取多个用户历史聊天记录的请求
		{
			result = true;
			m_listener->OnGetHistoryMessage(true, "", "", userItem);
		}
		else if (!userItem->m_inviteId.empty())
		{
			long requestId = m_requestController->QueryChatRecord(userItem->m_inviteId);
			result = (requestId != HTTPREQUEST_INVALIDREQUESTID);
		}
	}

	return result;
}

// 获取多个用户历史聊天记录（包括文本、高级表情、语音、图片）
bool LiveChatManManager::GetUsersHistoryMessage(const list<string>& userIds)
{
	bool result = false;

	list<string> inviteIds;
	for (list<string>::const_iterator iter = userIds.begin();
		iter != userIds.end();
		iter++)
	{
		LCUserItem* userItem = m_userMgr->GetUserItem((*iter));
		if (NULL != userItem
			&& !userItem->m_inviteId.empty())
		{
			inviteIds.push_back(userItem->m_inviteId);
			FileLog("LiveChatManager", "GetUsersHistoryMessage() userId:%s, inviteId:%s"
					, userItem->m_userId.c_str(), userItem->m_inviteId.c_str());
		}
	}

	if (!inviteIds.empty())
	{
		m_getUsersHistoryMsgRequestId = m_requestController->QueryChatRecordMutiple(inviteIds);
		result = (m_getUsersHistoryMsgRequestId != HTTPREQUEST_INVALIDREQUESTID);
	}

	return result;
}

// 结束会话
bool LiveChatManManager::EndTalk(const string& userId)
{
	return m_client->EndTalk(userId);
}

// 插入历史消息记录
bool LiveChatManManager::InsertHistoryMessage(const string& userId, LCMessageItem* msgItem)
{
	bool result = false;

	LCUserItem* userItem = m_userMgr->GetUserItem(userId);
	if (NULL != userItem) {
		result = userItem->InsertSortMsgList(msgItem);
		msgItem->m_msgId = m_msgIdBuilder.GetAndIncrement();
		msgItem->m_createTime = LCMessageItem::GetCreateTime();
	}

	return result;
}

// 删除历史消息记录
bool LiveChatManManager::RemoveHistoryMessage(const string& userId, int msgId)
{
	bool result = false;
	LCMessageItem* item = GetMessageWithMsgId(userId, msgId);
	if (NULL != item && NULL != item->GetUserItem())
	{
		LCUserItem* userItem = item->GetUserItem();
		result = userItem->RemoveSortMsgList(item);
	}
	return result;
}

// 获取指定消息Id的消息Item
LCMessageItem* LiveChatManManager::GetMessageWithMsgId(const string& userId, int msgId)
{
	LCMessageItem* item = NULL;
	LCUserItem* userItem = m_userMgr->GetUserItem(userId);
	if (userItem != NULL) {
		item = userItem->GetMsgItemWithId(msgId);
	}
	return item;
}

//  获取指定用户Id的用户item
LCUserItem* LiveChatManManager::GetUserWithId(const string& userId)
{
	LCUserItem* userItem = m_userMgr->GetUserItem(userId);
	return userItem;
}

// 获取消息处理状态
LCMessageItem::StatusType LiveChatManManager::GetMessageItemStatus(const string& userId, int msgId)
{
	LCMessageItem::StatusType statusType = LCMessageItem::StatusType_Unprocess;
	LCMessageItem* item = GetMessageWithMsgId(userId, msgId);
	if (NULL != item) {
		statusType = item->m_statusType;
	}
	return statusType;
}

// 发送待发消息列表
void LiveChatManManager::SendMessageList(LCUserItem* userItem)
{
	InsertRequestTask(REQUEST_TASK_SendMessageList, (unsigned long)userItem);
}

// 发送待发消息列表
void LiveChatManManager::SendMessageListProc(LCUserItem* userItem)
{
	if (NULL != userItem)
	{
		userItem->LockSendMsgList();
		userItem->m_tryingSend = false;
		for (LCMessageList::iterator iter = userItem->m_sendMsgList.begin();
			iter != userItem->m_sendMsgList.end();
			iter++)
		{
			// 发送消息item
			SendMessageItem((*iter));
		}
		userItem->m_sendMsgList.clear();
		userItem->UnlockSendMsgList();
	}
	else {
		FileLog("LiveChatManager", "SendMessageListProc() get user item fail");
	}
}

// 返回待发送消息错误
void LiveChatManManager::SendMessageListFailProc(LCUserItem* userItem, LCC_ERR_TYPE errType)
{
	if (NULL != userItem) {
		userItem->LockSendMsgList();
		userItem->m_tryingSend = false;
		for (LCMessageList::iterator iter = userItem->m_sendMsgList.begin();
			iter != userItem->m_sendMsgList.end();
			iter++)
		{
			(*iter)->m_statusType = LCMessageItem::StatusType_Fail;
			(*iter)->m_procResult.SetResult(errType, "", "");
		}
		m_listener->OnSendMessageListFail(errType, userItem->m_sendMsgList);
        BuildAndInsertWarningWithErrType(userItem->m_userId, errType);
        userItem->m_sendMsgList.clear();
		userItem->UnlockSendMsgList();
	}
	else {
		FileLog("LiveChatManager", "SendMessageListFailProc() get user item fail");
	}
}

// 发送消息item
void LiveChatManManager::SendMessageItem(LCMessageItem* item)
{
    // 发送消息
    switch (item->m_msgType)
    {
        case LCMessageItem::MT_Text:
            SendTextMessageProc(item);
            break;
        case LCMessageItem::MT_Emotion:
            SendEmotionProc(item);
            break;
        case LCMessageItem::MT_Photo:
            SendPhotoProc(item);
            break;
        case LCMessageItem::MT_Voice:
            SendVoiceProc(item);
            break;
        case LCMessageItem::MT_MagicIcon:
            SendMagicIconProc(item);
        default:
            FileLog("LiveChatManager", "SendMessageList() msgType error, msgType:%s", item->m_msgType);
            break;
    }
}

// ---------------- 文字聊天操作函数(message) ----------------
LCMessageItem* LiveChatManManager::SendTextMessage(const string& userId, const string& message)
{
	FileLog("LiveChatManager", "SendTextMessage() begin, userId:%s, message:%s"
			, userId.c_str(), message.c_str());

	// 判断是否处理发送操作
	if (!IsHandleSendOpt()) {
		FileLog("LiveChatManager", "SendTextMessage() IsHandleSendOpt()==false");
		return NULL;
	}

	// 获取用户item
	LCUserItem* userItem = m_userMgr->GetUserItem(userId);
	if (NULL == userItem) {
		FileLog("LiveChatManager", "SendTextMessage() getUserItem fail, userId:%s", userId.c_str());
		return NULL;
	}

	// 构造消息item
	LCMessageItem* item = NULL;
	if (!message.empty()) {
		// 生成MessageItem
		item = new LCMessageItem();
		item->Init(m_msgIdBuilder.GetAndIncrement()
				, LCMessageItem::SendType_Send
				, m_userId
				, userItem->m_userId
				, userItem->m_inviteId
				, LCMessageItem::StatusType_Processing);
		// 生成TextItem
		LCTextItem* textItem = new LCTextItem();
		textItem->Init(message, true);
		// 把TextItem加到MessageItem
		item->SetTextItem(textItem);
		// 添加到历史记录
		userItem->InsertSortMsgList(item);

		FileLog("LiveChatManager", "SendTextMessage() IsSendMessageNow:%d, m_isAutoLogin:%d, IsLogin:%d"
				, IsSendMessageNow(userItem), m_isAutoLogin, IsLogin());

        SendMsgWithCoupon(userItem, item);

	}
	else {
		FileLog("LiveChatManager", "SendTextMessage() param error, userId:%s, message:%s", userId.c_str(), message.c_str());
	}

	return item;
}

// 发送文本消息处理
void LiveChatManManager::SendTextMessageProc(LCMessageItem* item)
{
	if (m_client->SendTextMessage(item->m_toId, item->GetTextItem()->m_message, item->GetTextItem()->m_illegal, item->m_msgId))
	{
		m_textMgr->AddSendingItem(item);
	}
	else
	{
		item->m_statusType = LCMessageItem::StatusType_Fail;
		item->m_procResult.SetResult(LCC_ERR_FAIL, "", "");
		if (NULL != m_listener)
		{
			m_listener->OnSendTextMessage(item->m_procResult.m_errType, item->m_procResult.m_errMsg, item);
		}
	}
}

// 根据错误类型生成警告消息
void LiveChatManManager::BuildAndInsertWarningWithErrType(const string& userId, LCC_ERR_TYPE errType)
{
	if (errType == LCC_ERR_NOMONEY)
	{
		// 生成余额不足的警告消息
		BuildAndInsertWarningMsg(userId, LCWarningItem::NOMONEY);
	}
}

// 生成警告消息
void LiveChatManManager::BuildAndInsertWarningMsg(const string& userId, LCWarningItem::CodeType codeType)
{
	LCUserItem* userItem = m_userMgr->GetUserItem(userId);
	if (NULL != userItem) {
		// 生成warning消息
		LCWarningItem* warningItem = new LCWarningItem();
		warningItem->m_codeType = codeType;
		// 生成message消息
		LCMessageItem* item = new LCMessageItem();
		item->Init(m_msgIdBuilder.GetAndIncrement(), LCMessageItem::SendType_System, userItem->m_userId, m_userId, userItem->m_inviteId, LCMessageItem::StatusType_Finish);
		item->SetWarningItem(warningItem);
		// 插入到聊天记录列表中
		userItem->InsertSortMsgList(item);

		// 回调
		if (NULL != m_listener) {
			m_listener->OnRecvWarningMsg(item);
		}
	}
}

// 生成系统消息
void LiveChatManManager::BuildAndInsertSystemMsg(const string& userId, LCSystemItem::CodeType codeType)
{
	LCUserItem* userItem = m_userMgr->GetUserItem(userId);
	if (NULL != userItem) {
		// 生成系统消息
		LCSystemItem* systemItem = new LCSystemItem();
		systemItem->m_codeType = codeType;
		// 生成message消息
		LCMessageItem* item = new LCMessageItem();
		item->Init(m_msgIdBuilder.GetAndIncrement(), LCMessageItem::SendType_System, userId, m_userId, userItem->m_inviteId, LCMessageItem::StatusType_Finish);
		item->SetSystemItem(systemItem);
		// 插入到聊天记录列表中
		userItem->InsertSortMsgList(item);

		// 回调
		if (NULL != m_listener) {
			m_listener->OnRecvSystemMsg(item);
		}
	}
}

// ---------------- 高级表情操作函数(Emotion) ----------------
// 获取高级表情配置
bool LiveChatManManager::GetEmotionConfig()
{
	if (m_emotionMgr->m_emotionConfigReqId != HTTPREQUEST_INVALIDREQUESTID) {
		return true;
	}

	m_emotionMgr->m_emotionConfigReqId = m_requestOtherController->EmotionConfig();
	return m_emotionMgr->m_emotionConfigReqId != HTTPREQUEST_INVALIDREQUESTID;
}

// 获取高级表情配置item（PS：本次获取可能是旧的，当收到OnGetEmotionConfig()回调时，需要重新调用本函数获取）
OtherEmotionConfigItem LiveChatManManager::GetEmotionConfigItem() const
{
	return m_emotionMgr->GetConfigItem();
}

// 获取高级表情item
LCEmotionItem* LiveChatManManager::GetEmotionInfo(const string& emotionId)
{
	return m_emotionMgr->GetEmotion(emotionId);
}

// 发送高级表情
LCMessageItem* LiveChatManManager::SendEmotion(const string& userId, const string& emotionId)
{
	// 判断是否处理发送操作
	if (!IsHandleSendOpt()) {
		FileLog("LiveChatManager", "SendEmotion() IsHandleSendOpt()==false");
		return NULL;
	}

	// 获取用户item
	LCUserItem* userItem = m_userMgr->GetUserItem(userId);
	if (NULL == userItem) {
		FileLog("LiveChatManager", "SendEmotion() getUserItem fail, userId:%s", userId.c_str());
		return NULL;
	}

	LCMessageItem* item = NULL;
	if (!emotionId.empty()) {
		// 生成MessageItem
		item = new LCMessageItem();
		item->Init(m_msgIdBuilder.GetAndIncrement()
				, LCMessageItem::SendType_Send
				, m_userId
				, userId
				, userItem->m_inviteId
				, LCMessageItem::StatusType_Processing);
		// 获取EmotionItem
		LCEmotionItem* emotionItem = m_emotionMgr->GetEmotion(emotionId);
		// 把EmotionItem添加到MessageItem
		item->SetEmotionItem(emotionItem);
		// 添加到历史记录
		userItem->InsertSortMsgList(item);

		FileLog("LiveChatManager", "SendEmotion() IsSendMessageNow:%d, m_isAutoLogin:%d, IsLogin:%d"
						, IsSendMessageNow(userItem), m_isAutoLogin, IsLogin());

        SendMsgWithCoupon(userItem, item);
	}
	else {
		FileLog("LiveChatManager", "SendEmotion() param error, userId:%s, emotionId:%s", userId.c_str(), emotionId.c_str());
	}
	return item;
}

// 发送高级表情处理
void LiveChatManManager::SendEmotionProc(LCMessageItem* item)
{
	if (m_client->SendEmotion(item->m_toId, item->GetEmotionItem()->m_emotionId, item->m_msgId))
	{
		m_emotionMgr->AddSendingItem(item);
	}
	else {
		item->m_statusType = LCMessageItem::StatusType_Fail;
		item->m_procResult.SetResult(LCC_ERR_FAIL, "", "");
		m_listener->OnSendEmotion(item->m_procResult.m_errType, item->m_procResult.m_errMsg, item);
	}
}

// 手动下载/更新高级表情图片文件（缩略图）
bool LiveChatManManager::GetEmotionImage(const string& emotionId)
{
	LCEmotionItem* emotionItem = m_emotionMgr->GetEmotion(emotionId);

	FileLog("LiveChatManager", "GetEmotionImage() emotionId:%s, emotionItem:%p"
			, emotionId.c_str(), emotionItem);

	bool result = false;
	// 判断文件是否存在，若不存在则下载
	if (!emotionItem->m_imagePath.empty())
	{
		if (IsFileExist(emotionItem->m_imagePath))
		{
			if (NULL != m_listener) {
				m_listener->OnGetEmotionImage(true, emotionItem);
			}
			result = true;
		}
	}

	// 文件不存在，需要下载
	if (!result) {
		FileLog("LiveChatManager", "GetEmotionImage() begin download, emotionId:%s"
				, emotionId.c_str());

		result = m_emotionMgr->StartDownloadImage(emotionItem);
	}

	FileLog("LiveChatManager", "GetEmotionImage() emotionId:%s, result:%d"
			, emotionId.c_str(), result);
	return result;
}

// 手动下载/更新高级表情图片文件(大图)
bool LiveChatManManager::GetEmotionPlayImage(const string& emotionId)
{
	LCEmotionItem* emotionItem = m_emotionMgr->GetEmotion(emotionId);

	FileLog("LiveChatManager", "GetEmotionPlayImage() emotionId:%s, emotionItem:%p"
			, emotionId.c_str(), emotionItem);

	bool result = false;
	// 判断文件是否存在，若不存在则下载
	if (!emotionItem->m_playBigPath.empty()) {
		if (IsFileExist(emotionItem->m_playBigPath)) {
			if (!emotionItem->m_playBigPaths.empty()) {
				result = true;
				for (LCEmotionPathVector::const_iterator iter = emotionItem->m_playBigPaths.begin();
					iter != emotionItem->m_playBigPaths.end();
					iter++)
				{
					if (!IsFileExist((*iter))) {
						result = false;
						break;
					}
				}
			}

			// 所有文件都存在
			if (result) {
				m_listener->OnGetEmotionPlayImage(true, emotionItem);
			}
		}
	}

	// 有文件不存在，需要下载
	if (!result) {
		FileLog("LiveChatManager", "GetEmotionPlayImage() begin download, emotionId:%s"
				, emotionId.c_str());

		result = m_emotionMgr->StartDownloadPlayImage(emotionItem);
	}

	FileLog("LiveChatManager", "GetEmotionPlayImage() begin download, emotionId:%s, result:%d"
			, emotionId.c_str(), result);

	return result;
}

// ---------------- 图片操作函数(Private Album) ----------------
// 发送图片（包括上传图片文件(php)、发送图片(livechat)）
LCMessageItem* LiveChatManManager::SendPhoto(const string& userId, const string& photoPath)
{
	// 判断是否处理发送操作
	if (!IsHandleSendOpt()) {
		FileLog("LiveChatManager", "SendPhoto() IsHandleSendOpt()==false");
		return NULL;
	}

	// 获取用户item
	LCUserItem* userItem = m_userMgr->GetUserItem(userId);
	if (NULL == userItem) {
		FileLog("LiveChatManager", "SendPhoto() getUserItem fail, userId:%s", userId.c_str());
		return NULL;
	}
    
    // 复制文件至临时目录
    string tempPath("");
    if (!m_photoMgr->CopyPhotoToTempDir(photoPath, tempPath)) {
        FileLog("LiveChatManager", "SendPhoto() copy file to temp dir fail, userId:%s photoPath:%s", userId.c_str(), photoPath.c_str());
        return NULL;
    }

	// 生成MessageItem
	LCMessageItem* item = new LCMessageItem();
	item->Init(m_msgIdBuilder.GetAndIncrement()
			, LCMessageItem::SendType_Send
			, m_userId
			, userId
			, userItem->m_inviteId
			, LCMessageItem::StatusType_Processing);
	// 生成PhotoItem
	LCPhotoItem* photoItem = new LCPhotoItem();
	photoItem->Init(
			""
			, ""
			, ""
			, ""
			, ""
			, tempPath
			, ""
			, ""
			, true);
	// 把PhotoItem添加到MessageItem
	item->SetPhotoItem(photoItem);
	// 添加到历史记录
	userItem->InsertSortMsgList(item);

    SendMsgWithCoupon(userItem, item);
    
	return item;
}

// 发送图片处理
void LiveChatManManager::SendPhotoProc(LCMessageItem* item)
{
	LCUserItem* userItem = item->GetUserItem();
	LCPhotoItem* photoItem = item->GetPhotoItem();
	// 请求上传文件
	long requestId = m_requestController->SendPhoto(userItem->m_userId, userItem->m_inviteId, m_userId, m_sId, SMT_FROMPHOTOFILE, "", photoItem->m_srcFilePath);
	if (HTTPREQUEST_INVALIDREQUESTID != requestId) {
		// 请求成功
		item->m_statusType = LCMessageItem::StatusType_Processing;

		// 添加到请求表
		if (!m_photoMgr->AddRequestItem(requestId, item)) {
			FileLog("LiveChatManager", "SendPhotoProc() add request item fail, requestId:%d", requestId);
		}
	}
	else {
		item->m_statusType = LCMessageItem::StatusType_Fail;
		item->m_procResult.SetResult(LCC_ERR_FAIL, "", "");
		m_listener->OnSendPhoto(item->m_procResult.m_errType, item->m_procResult.m_errNum, item->m_procResult.m_errMsg, item);
	}
}

// 购买图片（包括付费购买图片(php)）
bool LiveChatManManager::PhotoFee(const string& userId, const string& photoId)
{
	bool result = false;
	LCUserItem* userItem = m_userMgr->GetUserItem(userId);
	if (NULL == userItem) {
		FileLog("LiveChatManager", "PhotoFee() get user item fail, userId:%s", userId.c_str());
		return false;
	}
    
    
    
    LCMessageItem* item = NULL;
    LCMessageList listItem = m_photoMgr->GetMsgListWithBindMap(photoId);
    for (LCMessageList::const_iterator msgIter = listItem.begin();  msgIter != listItem.end(); msgIter++) {
        if ( (*msgIter)->GetUserItem() == userItem) {
            item = *msgIter;
            break;
        }
    }
    if (NULL == item) {
        FileLog("LiveChatManager", "GetPhoto() get message item fail, photoId;%s", photoId.c_str());
        return false;
    }
    
//    result = GetPhoto(item, sizeType);
//    
//    
//    LCMessageItem* item = userItem->GetMsgItemWithId(msgId);
//	if (NULL == item) {
//		FileLog("LiveChatManager", "PhotoFee() get message item fail, msgId:%d", msgId);
//		return false;
//	}
    
    if (!m_isLogin) {
        FileLog("LiveChatManager", "PhotoFee() m_isLogin:%d, msgId:%d", m_isLogin);
        return false;
    }

	result = PhotoFee(item);

	return result;
}

// 购买图片（包括付费购买图片(php)）
bool LiveChatManManager::PhotoFee(LCMessageItem* item)
{
	// 判断参数是否有效
	if (item->m_msgType != LCMessageItem::MT_Photo
		|| item->m_fromId.empty()
		|| item->m_inviteId.empty()
		|| item->GetPhotoItem()->m_photoId.empty())
	{
		FileLog("LiveChatManager", "PhotoFee() param error, msgType:%d, fromId:%s, inviteId%s, photoId:%s"
				, item->m_msgType, item->m_fromId.c_str(), item->m_inviteId.c_str(), item->GetPhotoItem()->m_photoId.c_str());
		return false;
	}

	bool result = item->GetPhotoItem()->IsFee();

	// 若当前并未请求购买
	if (!result)
	{
		result = false;

		// 请求付费获取图片
		long requestId = m_requestController->PhotoFee(item->m_fromId, item->m_inviteId, m_userId, m_sId, item->GetPhotoItem()->m_photoId);
		if (requestId != HTTPREQUEST_INVALIDREQUESTID)
		{
			// 请求成功
			LCPhotoItem* photoItem = item->GetPhotoItem();
			photoItem->AddFeeStatus();

			result = m_photoMgr->AddRequestItem(requestId, item);
			if (!result) {
				FileLog("LiveChatManager", "PhotoFee() requestId:%d AddRequestItem fail", requestId);
			}
		}
		else {
			item->m_procResult.SetResult(LCC_ERR_FAIL, "request fail", "");
			m_listener->OnPhotoFee(false, item->m_procResult.m_errNum, item->m_procResult.m_errMsg, item);
		}
	}

	return result;
}

// 根据消息ID获取图片(模糊或清晰)（包括获取/下载对方私密照片(php)、显示图片(livechat)）
bool LiveChatManManager::GetPhoto(const string& userId, const string& photoId, GETPHOTO_PHOTOSIZE_TYPE sizeType, LCMessageItem::SendType sendType)
{
    bool result = false;
    LCUserItem* userItem = m_userMgr->GetUserItem(userId);
    if (NULL == userItem) {
        FileLog("LiveChatManager", "GetPhoto() get user item fail, userId;%s", userId.c_str());
        return false;
    }
    
    LCMessageItem* item = NULL;
    LCMessageList listItem = m_photoMgr->GetMsgListWithBindMap(photoId);
    for (LCMessageList::const_iterator msgIter = listItem.begin();  msgIter != listItem.end(); msgIter++) {
        if ( (*msgIter)->m_sendType == sendType && (*msgIter)->GetUserItem() == userItem) {
            item = *msgIter;
            break;
        }
    }
    if (NULL == item) {
        FileLog("LiveChatManager", "GetPhoto() get message item fail, photoId;%s", photoId.c_str());
        return false;
    }
    
    result = GetPhoto(item, sizeType);
    
    return result;
}


// 获取图片(模糊或清晰)（包括获取/下载对方私密照片(php)、显示图片(livechat)）
bool LiveChatManManager::GetPhoto(LCMessageItem* item, GETPHOTO_PHOTOSIZE_TYPE sizeType)
{
	FileLog("LiveChatManager", "GetPhoto() begin");

	if (item->m_msgType != LCMessageItem::MT_Photo
		|| item->m_fromId.empty()
		|| item->GetPhotoItem()->m_photoId.empty())
	{
		FileLog("LiveChatManager", "GetPhoto() param error, msgType:%d, fromId:%s, photoId:%s"
				, item->m_msgType, item->m_fromId.c_str(), item->GetPhotoItem()->m_photoId.c_str());
		return false;
	}

	bool result = false;

	// 请求下载图片
	GETPHOTO_PHOTOMODE_TYPE modeType;
	if (item->m_sendType == LCMessageItem::SendType_Send) {
		// 男士发送（直接获取清晰图片）
		modeType = GMT_CLEAR;
	}
	else  {
		// 女士发送（判断是否已购买）
		modeType = (item->GetPhotoItem()->m_charge ? GMT_CLEAR : GMT_FUZZY);
	}

	// 下载图片
	result = m_photoMgr->DownloadPhoto(item, m_userId, m_sId, sizeType, modeType);

	FileLog("LiveChatManager", "GetPhoto() end, result:%d", result);

	return result;
}

//// 获取图片发送/下载进度
//int LiveChatManManager::GetPhotoProcessRate(LCMessageItem* item)
//{
//	int percent = 0;
//	long requestId = m_photoMgr->GetRequestIdWithItem(item);
//	if (requestId != HTTPREQUEST_INVALIDREQUESTID) {
//		int total = RequestJni.GetDownloadContentLength(requestId);
//		int recv = RequestJni.GetRecvLength(requestId);
//
//		if (total > 0) {
//			recv = recv * 100;
//			percent = recv / total;
//		}
//	}
//	return percent;
//}

// ---------------- 语音操作函数(Voice) ----------------
// 发送语音（包括获取语音验证码(livechat)、上传语音文件(livechat)、发送语音(livechat)）
LCMessageItem* LiveChatManManager::SendVoice(const string& userId, const string& voicePath, const string& fileType, int timeLength)
{
	// 判断是否处理发送操作
	if (!IsHandleSendOpt()) {
		FileLog("LiveChatManager", "SendVoice() IsHandleSendOpt()==false");
		return NULL;
	}

	// 获取用户item
	LCUserItem* userItem = m_userMgr->GetUserItem(userId);
	if (NULL == userItem) {
		FileLog("LiveChatManager", "SendVoice getUserItem fail, userId:%s", userId.c_str());
		return NULL;
	}

	// 生成MessageItem
	LCMessageItem* item = new LCMessageItem();
	item->Init(m_msgIdBuilder.GetAndIncrement()
			, LCMessageItem::SendType_Send
			, m_userId
			, userId
			, userItem->m_inviteId
			, LCMessageItem::StatusType_Processing);
	// 生成VoiceItem
	LCVoiceItem* voiceItem = new LCVoiceItem();
	voiceItem->Init(""
			, voicePath
			, timeLength
			, fileType
			, ""
			, true);
	// 把VoiceItem添加到MessageItem
	item->SetVoiceItem(voiceItem);
	// 添加到聊天记录中
	userItem->InsertSortMsgList(item);

	FileLog("LiveChatManager", "SendVoice() IsSendMessageNow:%d, m_isAutoLogin:%d, IsLogin:%d"
					, IsSendMessageNow(userItem), m_isAutoLogin, IsLogin());

    SendMsgWithCoupon(userItem, item);
	return item;
}

// 发送语音处理
void LiveChatManManager::SendVoiceProc(LCMessageItem* item)
{
	if (m_client->GetVoiceCode(item->m_toId, item->m_msgId))
	{
		m_voiceMgr->AddSendingItem(item->m_msgId, item);
	}
	else {
		item->m_statusType = LCMessageItem::StatusType_Fail;
		item->m_procResult.SetResult(LCC_ERR_FAIL, "", "");
		m_listener->OnSendVoice(item->m_procResult.m_errType, item->m_procResult.m_errNum, item->m_procResult.m_errMsg, item);
	}
}

// 获取语音（包括下载语音(livechat)）
bool LiveChatManManager::GetVoice(const string& userId, int msgId)
{
	LCMessageItem* item = NULL;
	LCUserItem* userItem = m_userMgr->GetUserItem(userId);
	if (NULL != userItem) {
		item = userItem->GetMsgItemWithId(msgId);
	}

	if (NULL != item
		&& item->m_msgType != LCMessageItem::MT_Voice
		&& NULL == item->GetVoiceItem())
	{
		FileLog("LiveChatManager", "GetVoice() param error.");
		return false;
	}

	bool result = false;
	LCVoiceItem* voiceItem = item->GetVoiceItem();
	voiceItem->m_filePath = m_voiceMgr->GetVoicePath(item);
	long requestId = m_requestController->PlayVoice(voiceItem->m_voiceId, (OTHER_SITE_TYPE)m_siteType, voiceItem->m_filePath);
	if (requestId != HTTPREQUEST_INVALIDREQUESTID) {
		// 添加至请求map
		m_voiceMgr->AddRequestItem(requestId, item);
		voiceItem->m_processing = true;
		result = true;

		FileLog("LiveChatManager", "GetVoice() requestId:%d", requestId);
	}
	else {
		FileLog("LiveChatManager", "GetVoice() RequestOperator.getInstance().PlayVoice fail, voiceId:%s, siteType:%d, filePath:%s"
				, voiceItem->m_voiceId.c_str(), m_siteType, voiceItem->m_filePath.c_str());
		item->m_procResult.SetResult(LCC_ERR_FAIL, "", "");
		m_listener->OnGetVoice(item->m_procResult.m_errType, item->m_procResult.m_errNum, item->m_procResult.m_errMsg, item);
		result = false;
	}
	return result;
}

///**
// * 获取语音发送/下载进度
// * @param item	消息item
// * @return
// */
//public int GetVoiceProcessRate(LCMessageItem* item) {
//	int percent = 0;
//	long requestId = m_voiceMgr->getRequestIdWithItem(item);
//	if (requestId != HTTPREQUEST_INVALIDREQUESTID) {
//		int total = RequestJni.GetDownloadContentLength(requestId);
//		int recv = RequestJni.GetRecvLength(requestId);
//
//		if (total > 0) {
//			recv = recv * 100;
//			percent = recv / total;
//		}
//	}
//	return percent;
//}

// ---------------- 视频操作函数(Video) ----------------
// 获取微视频图片
bool LiveChatManManager::GetVideoPhoto(const string& userId, const string& videoId, const string& inviteId)
{
	bool result = false;

	result = m_videoMgr->DownloadVideoPhoto(m_userId, m_sId, userId, videoId, inviteId, VPT_BIG);

	return result;
}

// 购买微视频
bool LiveChatManManager::VideoFee(const string& userId, int msgId)
{
	bool result = false;

	LCUserItem* userItem = m_userMgr->GetUserItem(userId);
	if (NULL == userItem) {
		return false;
	}

	LCMessageItem* msgItem = userItem->GetMsgItemWithId(msgId);
	if (NULL == msgItem) {
		return false;
	}

	lcmm::LCVideoItem* videoItem = msgItem->GetVideoItem();
	if (msgItem->m_msgType == LCMessageItem::MT_Video
		&& NULL != videoItem)
	{
		result = videoItem->IsFee();
		if (!result) {
			// 未付费，现在付费
			long requestId = m_requestController->GetVideo(
									m_sId
									, m_userId
									, userItem->m_userId
									, videoItem->m_videoId
									, msgItem->m_inviteId
									, GVCT_MAN
									, videoItem->m_sendId);
			if (HTTPREQUEST_INVALIDREQUESTID != requestId)
			{
				m_videoMgr->AddPhotoFee(msgItem, requestId);
				videoItem->AddProcessStatusFee();
				result = true;
			}
		}
	}
	return result;
}

// 获取微视频播放文件
bool LiveChatManManager::GetVideo(const string& userId, const string& videoId, const string& inviteId, const string& videoUrl)
{
	bool result = false;

	// 判断是否已在下载
	result = m_videoMgr->IsDownloadVideo(videoId);
	if (!result) 
	{
		// 还没请求下载，现在下载
		result = m_videoMgr->DownloadVideo(m_userId, m_sId, userId, videoId, inviteId, videoUrl);
	}

	return result;
}

// 获取视频当前下载状态
bool LiveChatManManager::IsGetingVideo(const string& videoId)
{
	return m_videoMgr->IsDownloadVideo(videoId);
}

// 获取视频图片文件路径（仅文件存在）
string LiveChatManManager::GetVideoPhotoPathWithExist(const string& userId, const string& inviteId, const string& videoId, VIDEO_PHOTO_TYPE type)
{
	return m_videoMgr->GetVideoPhotoPathWithExist(userId, inviteId, videoId, type);
}

// 获取视频文件路径（仅文件存在）
string LiveChatManManager::GetVideoPathWithExist(const string& userId, const string& inviteId, const string& videoId)
{
	return m_videoMgr->GetVideoPathWithExist(userId, inviteId, videoId);
}

// ---------------- 小高级表情（小高表）操作函数(MagicIcon) ----------------
// 发送小高级表情消息处理
void LiveChatManManager::SendMagicIconProc(LCMessageItem* item)
{
    if (m_client->SendMagicIcon(item->m_toId, item->GetMagicIconItem()->m_magicIconId, item->m_msgId)) {
        m_magicIconMgr->AddSendingItem(item);
    }
    else{
        item->m_statusType = LCMessageItem::StatusType_Fail;
        item->m_procResult.SetResult(LCC_ERR_FAIL, "", "");
        m_listener->OnSendMagicIcon(item->m_procResult.m_errType, item->m_procResult.m_errMsg, item);
    }
}

////获取小高级表情配置
bool LiveChatManManager::GetMagicIconConfig()
{
    if (m_magicIconMgr->m_magicIconConfigReqId != HTTPREQUEST_INVALIDREQUESTID) {
        return true;
    }
    
    m_magicIconMgr->m_magicIconConfigReqId = m_requestController->GetMagicIconConfig();
    return m_magicIconMgr->m_magicIconConfigReqId != HTTPREQUEST_INVALIDREQUESTID;
}

// 发送小高级表情
LCMessageItem* LiveChatManManager::SendMagicIcon(const string& userId, const string& iconId)
{
    if(!IsHandleSendOpt()){
        FileLog("LiveChatManager", "SendMagicIcon() IsHandleSendOpt()==false");
        return NULL;
    }
    
    LCUserItem* userItem = m_userMgr->GetUserItem(userId);
    if(NULL == userItem){
        FileLog("LiveChatManager", "SendMagicIcon() getUserItem fail, userId:%s", userId.c_str());
        return NULL;
    }
    
    LCMessageItem* item = NULL;
    if(!iconId.empty()){
        item = new LCMessageItem();
        item->Init(m_msgIdBuilder.GetAndIncrement()
                   , LCMessageItem::SendType_Send
                   , m_userId
                   , userId
                   , userItem->m_inviteId
                   , LCMessageItem::StatusType_Processing);
        // 获取MagicIconItem
        LCMagicIconItem* magicIconItem = m_magicIconMgr->GetMagicIcon(iconId);
        // 把MagicIconItem添加到MessageItem
        item->SetMagicIconItem(magicIconItem);
        // 添加到历史记录
        userItem->InsertSortMsgList(item);
        
        FileLog("LiveChatManManager", "SendMagicIcon() IsSendMeesageNow:%d, m_isAutoLogin:%d, IsLogin:%d",
                IsSendMessageNow(userItem), m_isAutoLogin, m_isLogin);
        
        SendMsgWithCoupon(userItem, item);
    }
    else{
        FileLog("LiveChatManager", "SendMagicIcon() param error, userId:%s, iconId:%s", userId.c_str(), iconId.c_str());
    }
    return item;
}

// 获取小高级表情配置item
MagicIconConfig LiveChatManManager::GetMagicIconConfigItem() const
{
    return m_magicIconMgr->GetConfigItem();
}

// 获取小高级表情item
LCMagicIconItem* LiveChatManManager::GetMagicIconInfo(const string& magicIconId)
{
    return m_magicIconMgr->GetMagicIcon(magicIconId);
}

// 手动下载／更新小高级表情原图
bool LiveChatManManager::GetMagicIconSrcImage(const string& magicIconId)
{
    LCMagicIconItem* magicIconItem = m_magicIconMgr->GetMagicIcon(magicIconId);
    FileLog("LiveChatManManager", "GetMagicIconSrcImage() magicIconId:%s, magicIconItem:%p", magicIconId.c_str(), magicIconItem);;
    
    bool result = false;
    if (!magicIconItem->m_sourcePath.empty()) {
        if (IsFileExist(magicIconItem->m_sourcePath)) {
            if(NULL != m_listener){
                m_listener->OnGetMagicIconSrcImage(true, magicIconItem);
            }
            result = true;
        }
    }
    
    // 文件不存在，需要下载
    if(!result){
        FileLog("LiveChatManager", "GetMagicIconSrcImage() magicIconId:%s", magicIconId.c_str());
        result = m_magicIconMgr->StartDownloadSourceImage(magicIconItem);
    }
    
    FileLog("LiveChatManManager", "GetManicIconSrcImage() magicIconId:%s, result:%d", magicIconId.c_str(), result);
    return result;
    
}
// 手动下载／更新小高级表情拇子图
bool LiveChatManManager::GetMagicIconThumbImage(const string& magicIconId)
{
    LCMagicIconItem* magicIconItem = m_magicIconMgr->GetMagicIcon(magicIconId);
    FileLog("LiveChatManManager", "GetMagicIconThumnImage() magicIconId:%s, magicIconItem:%p", magicIconId.c_str(), magicIconItem);
    
    bool result = false;
    
    if (!magicIconItem->m_thumbPath.empty()) {
        if (IsFileExist(magicIconItem->m_thumbPath)) {
            if (NULL != m_listener) {
                m_listener->OnGetMagicIconThumbImage(true, magicIconItem);
            }
            result = true;
        }
    }
    
    if (!result) {
        FileLog("LiveChatManManage", "GetMagicIconThumbImage() magicIconId:%s", magicIconId.c_str());
        result = m_magicIconMgr->StartDownloadThumbImage(magicIconItem);
    }
    FileLog("LiveChatManManage", "GetMagicIconThumbImage() magicIconId:%s, result:%d", magicIconId.c_str(), result);
    
    return result;
}

// ------------------- LCEmotionManagerCallback -------------------
void LiveChatManManager::OnDownloadEmotionImage(bool result, LCEmotionItem* emotionItem)
{
	FileLog("LiveChatManager", "OnDownloadEmotionImage() result:%d, emotionId:%s", result, emotionItem->m_emotionId.c_str());

	if (NULL != m_listener) {
		m_listener->OnGetEmotionImage(result, emotionItem);
	}
}

void LiveChatManManager::OnDownloadEmotionPlayImage(bool result, LCEmotionItem* emotionItem)
{
	FileLog("LiveChatManager", "OnDownloadEmotionImage() result:%d, emotionId:%s", result, emotionItem->m_emotionId.c_str());

	if (NULL != m_listener) {
		m_listener->OnGetEmotionPlayImage(result, emotionItem);
	}
}

// ------------------- LCPhotoManagerCallback -------------------
void LiveChatManManager::OnDownloadPhoto(bool success, GETPHOTO_PHOTOSIZE_TYPE sizeType, const string& errnum, const string& errmsg, const LCMessageList& msgList)
{
	FileLog("LiveChatManager", "OnDownloadPhoto() result:%d", success);

	if (NULL != m_listener) {
		LCC_ERR_TYPE err = (success ? LCC_ERR_SUCCESS : LCC_ERR_FAIL);
		m_listener->OnGetPhoto(sizeType, err, errnum, errmsg, msgList);
	}

	FileLog("LiveChatManager", "OnDownloadPhoto() callback end");
}

// ------------------- LCPhotoManagerCallback -------------------
void LiveChatManManager::OnDownloadVideoPhoto(
					bool success
					, const string& errnum
					, const string& errmsg
					, const string& womanId
					, const string& inviteId
					, const string& videoId
					, VIDEO_PHOTO_TYPE type
					, const string& filePath
					, const LCMessageList& msgList)
{
	// callback
	LCC_ERR_TYPE result = success ? LCC_ERR_SUCCESS : LCC_ERR_FAIL;
	m_listener->OnGetVideoPhoto(result, errnum, errmsg, womanId, inviteId, videoId, type, filePath, msgList);
}
	
// 视频下载完成回调
void LiveChatManManager::OnDownloadVideo(bool success, const string& userId, const string& videoId, const string& inviteId, const string& filePath, const LCMessageList& msgList)
{
	// callback
	LCC_ERR_TYPE result = success ? LCC_ERR_SUCCESS : LCC_ERR_FAIL;
	m_listener->OnGetVideo(result, userId, videoId, inviteId, filePath, msgList);
}

// ------------------- LCMagicIconManagerCallback -------------------
// 小高级表情原图下载完成回调
void LiveChatManManager::OnDownloadMagicIconImage(bool success, LCMagicIconItem* magicIconItem)
{
    FileLog("LiveChatManManager", "OnDownloadMagicIconImage() result:%d, magicIconId:%s", success, magicIconItem->m_magicIconId.c_str());
    if (NULL != m_listener) {
        m_listener->OnGetMagicIconSrcImage(success, magicIconItem);
    }
}
//  小高级表情拇子图下载完成回调
void LiveChatManManager::OnDownloadMagicIconThumbImage(bool success, LCMagicIconItem* magicIconItem)
{
    FileLog("LiveChatManManager", "OnDownloadMagicIconThumbImag() result:%d, magicIconId:%s", success, magicIconItem->m_magicIconId.c_str());
    if (NULL != m_listener) {
        m_listener->OnGetMagicIconThumbImage(success, magicIconItem);
    }
}

// ------------------- ILiveChatClientListener -------------------
// 客户端主动请求
void LiveChatManManager::OnLogin(LCC_ERR_TYPE err, const string& errmsg)
{
	FileLog("LiveChatManager", "OnLogin() err:%d, errmsg:%s, m_isAutoLogin:%d", err, errmsg.c_str(), m_isAutoLogin);

	if (LCC_ERR_SUCCESS == err)
	{
		// 登录成功
		m_isLogin = true;

		// 上传客户端内部版本号
		m_client->UploadVer(m_appVer);

		// 获取黑名单列表
		m_client->GetBlockUsers();

		// 获取被屏蔽女士列表
		m_client->GetContactList(CONTACT_LIST_BLOCK);

		// 获取联系人列表
		m_client->GetContactList(CONTACT_LIST_CONTACT);

		// 获取在聊/邀请用户列表
		m_client->GetTalkList(TALK_LIST_CHATING | TALK_LIST_PAUSE | TALK_LIST_WOMAN_INVITE);

		// 获取高级表情配置
		InsertRequestTask(REQUEST_TASK_GetEmotionConfig);
        // alex获取小高级表情配置
        InsertRequestTask(REQUEST_TASK_GetMagicIconConfig);

//		// 使用试聊券，发送待发消息
//		InsertRequestTask(REQUEST_TASK_CheckCouponWithToSendUser);

		// 启动定时释放高级表情下载器任务
		InsertRequestTask(REQUEST_TASK_ReleaseEmotionDownloader);

		// 启动定时释放图片下载器任务
		InsertRequestTask(REQUEST_TASK_ReleasePhotoDownloader);
        
        // 启动定时释放小高级表情下载器任务
        InsertRequestTask(REQUEST_TASK_ReleaseMagicIconDownloader);
        
	}
	else if (IsAutoRelogin(err))
	{
		// 重置参数
		ResetParamWithAutoLogin();
		// 自动重登录
		InsertReloginTask();
	}
	else
	{
		// 重置数据
		ResetParamWithNotAutoLogin();
	}

	if (NULL != m_listener) {
		m_listener->OnLogin(err, errmsg, m_isAutoLogin);
	}
}

void LiveChatManManager::OnLogout(LCC_ERR_TYPE err, const string& errmsg)
{
    if (m_isLogin)
    {
        // 已经登录
        FileLog("LiveChatManager", "OnLogout() err:%d, errmsg:%s", err, errmsg.c_str());

        // 重置登录参数
        m_isLogin = false;

        // callback
        bool isAutoLogin = IsAutoRelogin(err);
        if (NULL != m_listener) {
            m_listener->OnLogout(err, errmsg, m_isAutoLogin);
        }

        if (isAutoLogin) {
            // 重置自动登录数据
            ResetParamWithAutoLogin();
            // 自动重登录
            InsertReloginTask();
        }
        else {
            // 重置所有数据
            ResetParamWithNotAutoLogin();
        }
    }
}

void LiveChatManManager::OnSetStatus(LCC_ERR_TYPE err, const string& errmsg)
{
	m_listener->OnSetStatus(err, errmsg);
}

void LiveChatManManager::OnEndTalk(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg)
{
	LCUserItem* userItem = m_userMgr->GetUserItem(inUserId);
	if (err == LCC_ERR_SUCCESS) {
		if (NULL != userItem) {
			userItem->EndTalk();
		}
		else {
			FileLog("LiveChatManager", "OnEndTalk() getUserItem is NULL, userId:%s", inUserId.c_str());
		}
	}
	m_listener->OnEndTalk(err, errmsg, userItem);

	// 生成警告消息
	if (err != LCC_ERR_SUCCESS) {
		if (NULL != userItem) {
			BuildAndInsertWarningWithErrType(inUserId, err);
		}
	}
}

void LiveChatManManager::OnGetUserStatus(const UserIdList& inList, LCC_ERR_TYPE err, const string& errmsg, const UserStatusList& userList)
{
	LCUserList userItemList;
	for (UserStatusList::const_iterator iter = userList.begin();
		iter != userList.end();
		iter++)
	{
		LCUserItem* userItem = m_userMgr->GetUserItem((*iter).userId);
		if (NULL != userItem) {
			SetUserOnlineStatus(userItem, (*iter).statusType);
			userItemList.push_back(userItem);
		}
	}

	m_listener->OnGetUserStatus(err, errmsg, userItemList);
}

void LiveChatManManager::OnGetTalkInfo(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const string& userId, const string& invitedId, bool charge, unsigned int chatTime)
{

}

void LiveChatManManager::OnSendTextMessage(const string& inUserId, const string& inMessage, int inTicket, LCC_ERR_TYPE err, const string& errmsg)
{
	LCMessageItem* item = m_textMgr->GetAndRemoveSendingItem(inTicket);
	if (NULL != item) {
        // 修改在线状态
        SetUserOnlineStatusWithLccErrType(item->GetUserItem(), err);
        
        // 处理消息
		item->m_statusType = (LCC_ERR_SUCCESS==err ? LCMessageItem::StatusType_Finish : LCMessageItem::StatusType_Fail);
		item->m_procResult.SetResult(err, "", errmsg);
		if (NULL != m_listener) {
			m_listener->OnSendTextMessage(err, errmsg, item);
		}
	}
	else {
		FileLog("LiveChatManager", "OnSendTextMessage() get sending item fail, ticket:%d", inTicket);
	}

	// 生成警告消息
	if (LCC_ERR_SUCCESS != err) {
		BuildAndInsertWarningWithErrType(inUserId, err);
	}

	FileLog("LiveChatManager", "OnSendTextMessage() err:%d, userId:%s, message:%s", err, inUserId.c_str(), inMessage.c_str());
}

void LiveChatManManager::OnSendEmotion(const string& inUserId, const string& inEmotionId, int inTicket, LCC_ERR_TYPE err, const string& errmsg)
{
	LCMessageItem* item = m_emotionMgr->GetAndRemoveSendingItem(inTicket);
	if (NULL != item) {
        // 修改在线状态
        SetUserOnlineStatusWithLccErrType(item->GetUserItem(), err);
        
        // 处理消息
		item->m_statusType = (err==LCC_ERR_SUCCESS ? LCMessageItem::StatusType_Finish : LCMessageItem::StatusType_Fail);
		item->m_procResult.SetResult(err, "", errmsg);
		m_listener->OnSendEmotion(err, errmsg, item);
	}
	else {
		FileLog("LiveChatManager", "OnSendEmotion() get sending item fail, ticket:%d", inTicket);
	}

	// 生成警告消息
	if (err != LCC_ERR_SUCCESS) {
		BuildAndInsertWarningWithErrType(inUserId, err);
	}
}

void LiveChatManManager::OnSendVGift(const string& inUserId, const string& inGiftId, int inTicket, LCC_ERR_TYPE err, const string& errmsg)
{

}

void LiveChatManManager::OnGetVoiceCode(const string& inUserId, int ticket, LCC_ERR_TYPE err, const string& errmsg, const string& voiceCode)
{
	LCMessageItem* item = m_voiceMgr->GetAndRemoveSendingItem(ticket);
	if (err == LCC_ERR_SUCCESS) {
		if (NULL != item
			&& item->m_msgType == LCMessageItem::MT_Voice
			&& NULL != item->GetVoiceItem())
		{
			LCVoiceItem* voiceItem = item->GetVoiceItem();
			voiceItem->m_checkCode = voiceCode;
			LCUserItem* userItem = item->GetUserItem();

			// 请求上传语音文件
			long requestId = m_requestController->UploadVoice(
									voiceItem->m_checkCode
									, userItem->m_inviteId
									, m_userId
									, true
									, userItem->m_userId
									, (OTHER_SITE_TYPE)m_siteType
									, voiceItem->m_fileType
									, voiceItem->m_timeLength
									, voiceItem->m_filePath);
			if (requestId != HTTPREQUEST_INVALIDREQUESTID) {
				// 添加item到请求map
				m_voiceMgr->AddRequestItem(requestId, item);
			}
			else {
				item->m_statusType = LCMessageItem::StatusType_Fail;
				item->m_procResult.SetResult(LCC_ERR_FAIL, "", "");
				m_listener->OnSendVoice(item->m_procResult.m_errType, item->m_procResult.m_errNum, item->m_procResult.m_errMsg, item);
			}
		}
		else {
			FileLog("LiveChatManager", "OnGetVoiceCode() param fail.");
		}
	}
	else {
		item->m_statusType = LCMessageItem::StatusType_Fail;
		item->m_procResult.SetResult(err, "", errmsg);
		m_listener->OnSendVoice(item->m_procResult.m_errType, item->m_procResult.m_errNum, item->m_procResult.m_errMsg, item);
	}

	// 生成警告消息
	if (err != LCC_ERR_SUCCESS) {
		BuildAndInsertWarningWithErrType(inUserId, err);
	}
}

void LiveChatManManager::OnSendVoice(const string& inUserId, const string& inVoiceId, int inTicket, LCC_ERR_TYPE err, const string& errmsg)
{
	LCMessageItem* item = m_voiceMgr->GetAndRemoveSendingItem(inTicket);
	if (NULL == item
		|| item->m_msgType != LCMessageItem::MT_Voice
		|| NULL == item->GetVoiceItem())
	{
		FileLog("LiveChatManager", "OnSendVoice() param fail.");
		m_listener->OnSendVoice(LCC_ERR_FAIL, "", "", item);
		return;
	}
    
    // 修改在线状态
    SetUserOnlineStatusWithLccErrType(item->GetUserItem(), err);

	// 发送成功
	if (err == LCC_ERR_SUCCESS) {
		m_voiceMgr->CopyVoiceFileToDir(item);
	}

	// 回调
	item->m_statusType = (LCC_ERR_SUCCESS==err ? LCMessageItem::StatusType_Finish : LCMessageItem::StatusType_Fail);
	item->m_procResult.SetResult(err, "", errmsg);
	m_listener->OnSendVoice(item->m_procResult.m_errType, item->m_procResult.m_errNum, item->m_procResult.m_errMsg, item);

	// 生成警告消息
	if (err != LCC_ERR_SUCCESS) {
		BuildAndInsertWarningWithErrType(inUserId, err);
	}
}

void LiveChatManManager::OnUseTryTicket(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const string& userId, TRY_TICKET_EVENT tickEvent)
{
	m_listener->OnUseTryTicket(err, errmsg, userId, tickEvent);

	if (err != LCC_ERR_SUCCESS) {
		// 使用不成功，生成警告消息
		BuildAndInsertWarningWithErrType(inUserId, err);
		// 检测是否有足够余额
		LCUserItem* userItem = m_userMgr->GetUserItem(userId);
		InsertRequestTask(REQUEST_TASK_GetCount, (unsigned long)userItem);
	}
	else {
		// 若当前状态为Other或女士邀请，则标记为ManInvite(男士邀请)状态
		LCUserItem* userItem = m_userMgr->GetUserItem(userId);
		if (NULL != userItem) {
			if (userItem->m_chatType == LCUserItem::LC_CHATTYPE_OTHER
				|| userItem->m_chatType == LCUserItem::LC_CHATTYPE_INVITE)
			{
				userItem->m_chatType = LCUserItem::LC_CHATTYPE_MANINVITE;
			}
		}
		// 使用成功，发送待发消息
		InsertRequestTask(REQUEST_TASK_SendMessageList, (unsigned long)userItem);
	}
}

void LiveChatManManager::OnGetTalkList(int inListType, LCC_ERR_TYPE err, const string& errmsg, const TalkListInfo& talkListInfo)
{
	FileLog("LiveChatManager", "OnGetTalkList() listType:%d, err:%d, errmsg:%s" \
			", chating:%d, pause:%d, invite:%d, invited:%d"
			, inListType, err, errmsg.c_str()
			, talkListInfo.chating.size(), talkListInfo.pause.size(), talkListInfo.invite.size(), talkListInfo.invited.size());
	if (LCC_ERR_SUCCESS == err)
	{
		TalkUserList::const_iterator iter;
		TalkSessionList::const_iterator iterSession;

		// ------- 在线列表 -------
		// 在线session列表
		for (iterSession = talkListInfo.chatingSession.begin();
			iterSession != talkListInfo.chatingSession.end();
			iterSession++)
		{
			LCUserItem* userItem = m_userMgr->GetUserItem((*iterSession).targetId);
			if (NULL != userItem)
			{
				userItem->m_inviteId = (*iterSession).invitedId;

				FileLog("LiveChatManager", "OnGetTalkList() chating userId:%s, inviteId:%s"
						, userItem->m_userId.c_str(), userItem->m_inviteId.c_str());
			}
		}
		// 在线列表
		for (iter = talkListInfo.chating.begin();
			iter != talkListInfo.chating.end();
			iter++)
		{
			LCUserItem* userItem = m_userMgr->GetUserItem((*iter).userId);
			if (NULL != userItem
				&& !userItem->m_inviteId.empty())
			{
				userItem->m_userName = (*iter).userName;
				userItem->m_sexType = USER_SEX_FEMALE;
				SetUserOnlineStatus(userItem, USTATUS_ONLINE);
				userItem->m_chatType = LCUserItem::LC_CHATTYPE_IN_CHAT_CHARGE;
				userItem->m_clientType = (*iter).clientType;
				userItem->m_order = (*iter).orderValue;
			}
		}

		// ------- 在聊已暂停列表 -------
		// 在线已暂停session列表
		for (iterSession = talkListInfo.pauseSession.begin();
			iterSession != talkListInfo.pauseSession.end();
			iterSession++)
		{
			LCUserItem* userItem = m_userMgr->GetUserItem((*iterSession).targetId);
			if (NULL != userItem)
			{
				userItem->m_inviteId = (*iterSession).invitedId;

				FileLog("LiveChatManager", "OnGetTalkList() pause userId:%s, inviteId:%s"
						, userItem->m_userId.c_str(), userItem->m_inviteId.c_str());
			}
		}
		// 在聊已暂停列表
		for (iter = talkListInfo.pause.begin();
			iter != talkListInfo.pause.end();
			iter++)
		{
			LCUserItem* userItem = m_userMgr->GetUserItem((*iter).userId);
			if (NULL != userItem
				&& !userItem->m_inviteId.empty())
			{
				userItem->m_userName = (*iter).userName;
				userItem->m_sexType = USER_SEX_FEMALE;
				SetUserOnlineStatus(userItem, USTATUS_ONLINE);
				userItem->m_chatType = LCUserItem::LC_CHATTYPE_IN_CHAT_CHARGE;
				userItem->m_clientType = (*iter).clientType;
				userItem->m_order = (*iter).orderValue;
			}
		}

		// ------- 邀请列表 -------
		// 邀请session列表
		for (iterSession = talkListInfo.inviteSession.begin();
			iterSession != talkListInfo.inviteSession.end();
			iterSession++)
		{
			LCUserItem* userItem = m_userMgr->GetUserItem((*iterSession).targetId);
			if (NULL != userItem)
			{
				userItem->m_inviteId = (*iterSession).invitedId;

				FileLog("LiveChatManager", "OnGetTalkList() invite userId:%s, inviteId:%s"
						, userItem->m_userId.c_str(), userItem->m_inviteId.c_str());
			}
		}
		// 邀请列表
		for (iter = talkListInfo.invite.begin();
			iter != talkListInfo.invite.end();
			iter++)
		{
			LCUserItem* userItem = m_userMgr->GetUserItem((*iter).userId);
			if (NULL != userItem
				&& !userItem->m_inviteId.empty())
			{
				userItem->m_userName = (*iter).userName;
				userItem->m_sexType = USER_SEX_FEMALE;
				SetUserOnlineStatus(userItem, USTATUS_ONLINE);
				userItem->m_chatType = LCUserItem::LC_CHATTYPE_MANINVITE;
				userItem->m_clientType = (*iter).clientType;
				userItem->m_order = (*iter).orderValue;
			}
		}

		// ------- 被邀请列表 -------
		// 被邀请session列表
		for (iterSession = talkListInfo.invitedSession.begin();
			iterSession != talkListInfo.invitedSession.end();
			iterSession++)
		{
			LCUserItem* userItem = m_userMgr->GetUserItem((*iterSession).targetId);
			if (NULL != userItem)
			{
				userItem->m_inviteId = (*iterSession).invitedId;

				FileLog("LiveChatManager", "OnGetTalkList() invited userId:%s, inviteId:%s"
						, userItem->m_userId.c_str(), userItem->m_inviteId.c_str());
			}
		}
		// 被邀请列表
		for (iter = talkListInfo.invited.begin();
			iter != talkListInfo.invited.end();
			iter++)
		{
			LCUserItem* userItem = m_userMgr->GetUserItem((*iter).userId);
			if (NULL != userItem
				&& !userItem->m_inviteId.empty())
			{
				userItem->m_userName = (*iter).userName;
				userItem->m_sexType = USER_SEX_FEMALE;
				SetUserOnlineStatus(userItem, USTATUS_ONLINE);
				userItem->m_chatType = LCUserItem::LC_CHATTYPE_INVITE;
				userItem->m_clientType = (*iter).clientType;
				userItem->m_order = (*iter).orderValue;
			}
		}

		// 获取用户的聊天历史记录
		InsertRequestTask(REQUEST_TASK_GetUsersHistoryMessage);
	}

	m_listener->OnGetTalkList(err, errmsg);
}

void LiveChatManManager::OnSendPhoto(LCC_ERR_TYPE err, const string& errmsg, int ticket)
{
	LCMessageItem* item = m_photoMgr->GetAndRemoveSendingItem(ticket);
	if (NULL != item) {
        // 修改在线状态
        SetUserOnlineStatusWithLccErrType(item->GetUserItem(), err);
        
        // 处理消息
		item->m_statusType = (err==LCC_ERR_SUCCESS ? LCMessageItem::StatusType_Finish : LCMessageItem::StatusType_Fail);
		item->m_procResult.SetResult(err, "", errmsg);
		m_listener->OnSendPhoto(item->m_procResult.m_errType, item->m_procResult.m_errNum, item->m_procResult.m_errMsg, item);
	}
	else {
		FileLog("LiveChatManager", "OnSendPhoto() get sending item fail, ticket:%d", ticket);
	}

	// 生成警告消息
	if (err != LCC_ERR_SUCCESS) {
		if (NULL != item && NULL != item->GetUserItem()) {
			BuildAndInsertWarningWithErrType(item->GetUserItem()->m_userId, err);
		}
	}
}

void LiveChatManManager::OnShowPhoto(LCC_ERR_TYPE err, const string& errmsg, int ticket)
{

}

void LiveChatManManager::OnGetUserInfo(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const UserInfoItem& userInfo)
{
	if (err == LCC_ERR_SUCCESS)
	{
        // 若用户已经存在，则更新用户信息
        if (m_userMgr->IsUserExists(userInfo.userId)) {
            LCUserItem* userItem = m_userMgr->GetUserItem(userInfo.userId);
            if (NULL != userItem) {
                userItem->UpdateWithUserInfo(userInfo);
            }
        }
        
        // 更新用户排序分值
        m_inviteMgr->UpdateUserOrderValue(userInfo.userId, userInfo.orderValue);
	}
    
    if (NULL != m_listener)
    {
        m_listener->OnGetUserInfo(inUserId, err, errmsg, userInfo);
    }
}

void LiveChatManManager::OnGetUsersInfo(LCC_ERR_TYPE err, const string& errmsg, int seq, const list<string>& userIdList,const UserInfoList& userList)
{
    if (err == LCC_ERR_SUCCESS)
    {
        for (UserInfoList::const_iterator iter = userList.begin();
             iter != userList.end();
             iter++)
        {
            const UserInfoItem& userInfo = (*iter);
            
            // 若用户已经存在，则更新用户信息
            if (m_userMgr->IsUserExists(userInfo.userId)) {
                LCUserItem* userItem = m_userMgr->GetUserItem(userInfo.userId);
                if (NULL != userItem) {
                    userItem->UpdateWithUserInfo(userInfo);
                }
            }
            
            // 更新用户排序分值
            m_inviteMgr->UpdateUserOrderValue(userInfo.userId, userInfo.orderValue);
        }
    }
    
    if (NULL != m_listener) {
        m_listener->OnGetUsersInfo(err, errmsg, userList);
    }
}

void LiveChatManManager::OnGetContactList(CONTACT_LIST_TYPE inListType, LCC_ERR_TYPE err, const string& errmsg, const TalkUserList& userList)
{
	FileLog("LiveChatManager", "OnGetContactList() listType:%d, err:%d, errmsg:%s, users.size:%d"
			, inListType, err, errmsg.c_str(), userList.size());
	if (LCC_ERR_SUCCESS == err) {
		switch (inListType)
		{
		case CONTACT_LIST_BLOCK:
			m_blockMgr->UpdateWithBlockList(userList);
			break;
		case CONTACT_LIST_CONTACT:
			m_contactMgr->UpdateWithContactList(userList);
			break;
		}
	}
}

void LiveChatManManager::OnGetBlockUsers(LCC_ERR_TYPE err, const string& errmsg, const list<string>& userList)
{
	FileLog("LiveChatManager", "OnGetBlockUsers() err:%d, errmsg:%s, users.size:%d"
			, err, errmsg.c_str(), userList.size());
	if (LCC_ERR_SUCCESS == err)
	{
		m_blockMgr->UpdateWithBlockUsers(userList);
	}
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

void LiveChatManManager::OnSendLadyVideo(LCC_ERR_TYPE err, const string& errmsg, int ticket)
{

}

void LiveChatManManager::OnGetLadyCondition(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const LadyConditionItem& item)
{

}

void LiveChatManManager::OnGetLadyCustomTemplate(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const vector<string>& contents, const vector<bool>& flags)
{

}

void LiveChatManManager::OnSendMagicIcon(const string& inUserId, const string& inIconId, int inTicket, LCC_ERR_TYPE err, const string& errmsg)
{
    FileLog("LiveChatManManager", "OnSendMagicIcon() begin");
    LCMessageItem* item = m_magicIconMgr->GetAndRemoveSendingItem(inTicket);
    if (NULL != item) {
        // 修改在线状态
        SetUserOnlineStatusWithLccErrType(item->GetUserItem(), err);
        
        //处理消息
        item->m_statusType = (err == LCC_ERR_SUCCESS ? LCMessageItem::StatusType_Finish : LCMessageItem::StatusType_Fail);
        item->m_procResult.SetResult(err, "", errmsg);
        m_listener->OnSendMagicIcon(err, errmsg, item);
    }
    else{
        FileLog("LiveChatManManager", "OnSendMagicIcon() get sending item fail, ticket:%d", inTicket);
    }
    // 生成警告消息
    if (err != LCC_ERR_SUCCESS) {
        BuildAndInsertWarningWithErrType(inUserId, err);
    }
}

void LiveChatManManager::OnGetPaidTheme(const string& inUserId, LCC_ERR_TYPE err, const string& errmsg, const ThemeInfoList& themeList)
{

}

void LiveChatManManager::OnGetAllPaidTheme(LCC_ERR_TYPE err, const string& errmsg, const ThemeInfoList& themeInfoList)
{

}

void LiveChatManManager::OnManFeeTheme(const string& inUserId, const string& inThemeId, LCC_ERR_TYPE err, const string& errmsg, const ThemeInfoItem& item)
{

}

void LiveChatManManager::OnManApplyTheme(const string& inUserId, const string& inThemeId, LCC_ERR_TYPE err, const string& errmsg, const ThemeInfoItem& item)
{

}

void LiveChatManManager::OnPlayThemeMotion(const string& inUserId, const string& inThemeId, LCC_ERR_TYPE err, const string& errmsg, bool success)
{

}

// 服务器主动请求
void LiveChatManManager::OnRecvShowVideo(const string& toId, const string& fromId, const string& fromName, const string& inviteId, const string& videoId, const string& sendId, bool charge, const string& videoDec, int ticket)
{

}

void LiveChatManManager::OnRecvAutoInviteMsg(const string& womanId, const string& manId, const string& key)
{

}

void LiveChatManManager::OnRecvAutoChargeResult(const string& manId, double money, TAUTO_CHARGE_TYPE type, bool result, const string& code, const string& msg)
{

}

void LiveChatManManager::OnRecvMessage(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, int ticket, TALK_MSG_TYPE msgType, const string& message,INVITE_TYPE inviteType)
{
	FileLog("LiveChatManager", "OnRecvMessage() begin, fromId:%s, ticket:%d, message:%s", fromId.c_str(), ticket, message.c_str());

	m_client->UploadTicket(fromId, ticket);

	LCMessageItem* item = NULL;

	// 判断是否邀请消息
	LCInviteManager::HandleInviteMsgType handleType = m_inviteMgr->IsToHandleInviteMsg(fromId, charge, msgType);
	if (handleType == LCInviteManager::HANDLE) {
		// 处理邀请消息
		item = m_inviteMgr->HandleInviteMessage(m_msgIdBuilder, toId, fromId, fromName, inviteId, charge, ticket, msgType, message);
	}
	else if (handleType == LCInviteManager::PASS) {
		// 添加用户到列表中（若列表中用户不存在）
		LCUserItem* userItem = m_userMgr->GetUserItem(fromId);
		if (NULL == userItem) {
			FileLog("LiveChatManager", "OnRecvMessage() getUserItem fail, fromId:%s", fromId.c_str());
			return;
		}
		userItem->m_inviteId = inviteId;
		userItem->m_userName = fromName;
		userItem->SetChatTypeWithTalkMsgType(charge, msgType);
		SetUserOnlineStatus(userItem, USTATUS_ONLINE);

		// 生成MessageItem
		item = new LCMessageItem;
		FileLog("LiveChatManager", "OnRecvMessage() item:%p", item);
		FileLog("LiveChatManager", "OnRecvMessage() inviteId:%s", userItem->m_inviteId.c_str());
		item->Init(m_msgIdBuilder.GetAndIncrement()
				, LCMessageItem::SendType_Recv
				, fromId
				, toId
				, userItem->m_inviteId
				, LCMessageItem::StatusType_Finish);
		// 生成TextItem
		LCTextItem* textItem = new LCTextItem();
		textItem->Init(message, false);
		// 把TextItem添加到MessageItem
		item->SetTextItem(textItem);
		// 添加到用户聊天记录中
		if (!userItem->InsertSortMsgList(item)) {
			// 添加到用户聊天记录列表不成功
			delete item;
			item = NULL;
		}
	}

	if (NULL != item) {
		FileLog("LiveChatManager", "OnRecvMessage() callback, item:%p", item);

		// callback
		m_listener->OnRecvMessage(item);
	}

	FileLog("LiveChatManager", "OnRecvMessage() end, fromId:%s, ticket:%d, message:%s"
			, fromId.c_str(), ticket, message.c_str());
}

void LiveChatManManager::OnRecvEmotion(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, int ticket, TALK_MSG_TYPE msgType, const string& emotionId)
{
	// 返回票根给服务器
	m_client->UploadTicket(fromId, ticket);

	// 添加用户到列表中（若列表中用户不存在）
	LCUserItem* userItem = m_userMgr->GetUserItem(fromId);
	if (NULL == userItem) {
		FileLog("LiveChatManager", "OnRecvEmotion() getUserItem fail, fromId:%s", fromId.c_str());
		return;
	}
	userItem->m_inviteId = inviteId;
	userItem->m_userName = fromName;
	userItem->SetChatTypeWithTalkMsgType(charge, msgType);
	SetUserOnlineStatus(userItem, USTATUS_ONLINE);

	// 生成MessageItem
	LCMessageItem* item = new LCMessageItem();
	item->Init(m_msgIdBuilder.GetAndIncrement()
			, LCMessageItem::SendType_Recv
			, fromId
			, toId
			, userItem->m_inviteId
			, LCMessageItem::StatusType_Finish);
	// 获取EmotionItem
	LCEmotionItem* emotionItem = m_emotionMgr->GetEmotion(emotionId);
	// 把EmotionItem添加到MessageItem
	item->SetEmotionItem(emotionItem);

	// 添加到用户聊天记录中
	userItem->InsertSortMsgList(item);

	// callback
	if (NULL != m_listener) {
		m_listener->OnRecvEmotion(item);
	}
}

void LiveChatManManager::OnRecvVoice(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, TALK_MSG_TYPE msgType, const string& voiceId, const string& fileType, int timeLen)
{
	// 添加用户到列表中（若列表中用户不存在）
	LCUserItem* userItem = m_userMgr->GetUserItem(fromId);
	if (NULL == userItem) {
		FileLog("LiveChatManager", "OnRecvVoice() getUserItem fail, fromId:%s", fromId.c_str());
		return;
	}
	userItem->m_userName = fromName;
	userItem->m_inviteId = inviteId;
	userItem->SetChatTypeWithTalkMsgType(charge, msgType);
	SetUserOnlineStatus(userItem, USTATUS_ONLINE);

	// 生成MessageItem
	LCMessageItem* item = new LCMessageItem();
	item->Init(m_msgIdBuilder.GetAndIncrement()
			, LCMessageItem::SendType_Recv
			, fromId
			, toId
			, userItem->m_inviteId
			, LCMessageItem::StatusType_Finish);
	// 生成VoiceItem
	LCVoiceItem* voiceItem = new LCVoiceItem();
	voiceItem->Init(voiceId
			, m_voiceMgr->GetVoicePath(voiceId, fileType)
			, timeLen
			, fileType
			, ""
			, charge);

	// 把VoiceItem添加到MessageItem
	item->SetVoiceItem(voiceItem);

	// 添加到聊天记录中
	userItem->InsertSortMsgList(item);

	// callback
	m_listener->OnRecvVoice(item);
}

void LiveChatManManager::OnRecvWarning(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, int ticket, TALK_MSG_TYPE msgType, const string& message)
{
	// 返回票根给服务器
	m_client->UploadTicket(fromId, ticket);

	// 添加用户到列表中（若列表中用户不存在）
	LCUserItem* userItem = m_userMgr->GetUserItem(fromId);
	if (NULL == userItem) {
		FileLog("LiveChatManager", "OnRecvWarning() getUserItem fail, fromId:%s", fromId.c_str());
		return;
	}
	userItem->m_inviteId = inviteId;
	userItem->SetChatTypeWithTalkMsgType(charge, msgType);
	userItem->m_userName = fromName;
	SetUserOnlineStatus(userItem, USTATUS_ONLINE);

	// 生成MessageItem
	LCMessageItem* item = new LCMessageItem();
	item->Init(m_msgIdBuilder.GetAndIncrement()
			, LCMessageItem::SendType_Recv
			, fromId
			, toId
			, userItem->m_inviteId
			, LCMessageItem::StatusType_Finish);
	// 生成WarningItem
	LCWarningItem* warningItem = new LCWarningItem;
	warningItem->Init(message);
	// 把WarningItem添加到MessageItem
	item->SetWarningItem(warningItem);

	// 添加到用户聊天记录中
	userItem->InsertSortMsgList(item);

	// callback
	m_listener->OnRecvWarningMsg(item);
}

void LiveChatManManager::OnUpdateStatus(const string& userId, const string& server, CLIENT_TYPE clientType, USER_STATUS_TYPE statusType)
{
	LCUserItem* userItem = m_userMgr->GetUserItem(userId);
	if (NULL == userItem) {
		FileLog("LiveChatManager", "OnUpdateStatus() getUserItem fail, userId:%s", userId.c_str());
		return;
	}
	userItem->m_clientType = clientType;
	SetUserOnlineStatus(userItem, statusType);

	m_listener->OnUpdateStatus(userItem);
}

void LiveChatManManager::OnUpdateTicket(const string& fromId, int ticket)
{
	// 不用处理
}

void LiveChatManManager::OnRecvEditMsg(const string& fromId)
{
	m_listener->OnRecvEditMsg(fromId);
}

void LiveChatManManager::OnRecvTalkEvent(const string& userId, TALK_EVENT_TYPE eventType)
{
	LCUserItem* userItem = m_userMgr->GetUserItem(userId);
	if (NULL == userItem) {
		FileLog("LiveChatManager", "OnRecvTalkEvent() getUserItem fail, userId:%s, eventType:%d", userId.c_str(), eventType);
		return;
	}
	userItem->SetChatTypeWithEventType(eventType);
	m_listener->OnRecvTalkEvent(userItem);

	if (eventType == TET_NOMONEY
		|| eventType == TET_VIDEONOMONEY)
	{
		BuildAndInsertWarningWithErrType(userId, LCC_ERR_NOMONEY);
	}
}

void LiveChatManManager::OnRecvTryTalkBegin(const string& toId, const string& fromId, int time)
{
	// 改变用户聊天状态并回调
	LCUserItem* userItem = m_userMgr->GetUserItem(toId);
	if (NULL == userItem) {
		FileLog("LiveChatManager", "OnRecvTryTalkBegin() getUserItem fail, toId:%s", toId.c_str());
		return;
	}
	userItem->m_chatType = LCUserItem::LC_CHATTYPE_IN_CHAT_USE_TRY_TICKET;
	m_listener->OnRecvTryTalkBegin(userItem, time);
}

void LiveChatManManager::OnRecvTryTalkEnd(const string& userId)
{
	// 改变用户聊天状态并回调
	LCUserItem* userItem = m_userMgr->GetUserItem(userId);
	if (NULL == userItem) {
		FileLog("LiveChatManager", "OnRecvTryTalkEnd() getUserItem fail, userId:%s", userId.c_str());
		return;
	}
	userItem->m_chatType = LCUserItem::LC_CHATTYPE_IN_CHAT_CHARGE;
	m_listener->OnRecvTryTalkEnd(userItem);

	// 插入系统消息
	BuildAndInsertSystemMsg(userId, LCSystemItem::TRY_CHAT_END);
}

void LiveChatManManager::OnRecvEMFNotice(const string& fromId, TALK_EMF_NOTICE_TYPE noticeType)
{
	m_listener->OnRecvEMFNotice(fromId, noticeType);
}

void LiveChatManManager::OnRecvKickOffline(KICK_OFFLINE_TYPE kickType)
{
	FileLog("LiveChatManager", "OnRecvKickOffline() kickType:%d", kickType);

	// 用户在其它地方登录，被踢下线
	if (KOT_OTHER_LOGIN == kickType)
	{
		// 回调
		m_listener->OnRecvKickOffline(kickType);
	}

	FileLog("LiveChatManager", "OnRecvKickOffline() end");
}

void LiveChatManManager::OnRecvPhoto(const string& toId, const string& fromId, const string& fromName, const string& inviteId, const string& photoId, const string& sendId, bool charge, const string& photoDesc, int ticket)
{
	// 返回票根给服务器
	m_client->UploadTicket(fromId, ticket);

	// 添加用户到列表中（若列表中用户不存在）
	LCUserItem* userItem = m_userMgr->GetUserItem(fromId);
	if (NULL == userItem) {
		FileLog("LiveChatManager", "OnRecvPhoto() getUserItem fail, fromId:%s", fromId.c_str());
		return;
	}
	userItem->m_inviteId = inviteId;
	userItem->m_userName = fromName;
	SetUserOnlineStatus(userItem, USTATUS_ONLINE);

	// 生成MessageItem
	LCMessageItem* item = new LCMessageItem();
	item->Init(m_msgIdBuilder.GetAndIncrement()
			, LCMessageItem::SendType_Recv
			, fromId
			, toId
			, userItem->m_inviteId
			, LCMessageItem::StatusType_Finish);
	// 生成PhotoItem
    LCPhotoItem* photoItem = m_photoMgr->GetPhotoItem(photoId, item);
	photoItem->Init(photoId
			, sendId
			, photoDesc
			, m_photoMgr->GetPhotoPath(photoId, GMT_FUZZY, GPT_LARGE)
			, m_photoMgr->GetPhotoPath(photoId, GMT_FUZZY, GPT_MIDDLE)
			, m_photoMgr->GetPhotoPath(photoId, GMT_CLEAR, GPT_ORIGINAL)
			, m_photoMgr->GetPhotoPath(photoId, GMT_CLEAR, GPT_LARGE)
			, m_photoMgr->GetPhotoPath(photoId, GMT_CLEAR, GPT_MIDDLE)
			, charge);

	// 添加到用户聊天记录中
	userItem->InsertSortMsgList(item);

	// callback
	m_listener->OnRecvPhoto(item);
}

void LiveChatManManager::OnRecvShowPhoto(const string& toId, const string& fromId, const string& fromName, const string& inviteId, const string& photoId, const string& sendId, bool charge, const string& photoDec, int ticket)
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
	// 返回票根给服务器
	m_client->UploadTicket(fromId, ticket);

	// 不允许接收视频消息
	if (!m_isRecvVideoMsg) {
		return;
	}

	// 添加用户到列表中（若列表中用户不存在）
	LCUserItem* userItem = m_userMgr->GetUserItem(fromId);
	if (NULL == userItem) {
		FileLog("LiveChatManager", "OnRecvVideo() getUserItem fail, fromId:%s", fromId.c_str());
		return;
	}
	userItem->m_inviteId = inviteId;
	userItem->m_userName = fromName;
	SetUserOnlineStatus(userItem, USTATUS_ONLINE);

	// 生成MessageItem
	LCMessageItem* item = new LCMessageItem();
	item->Init(m_msgIdBuilder.GetAndIncrement()
			, LCMessageItem::SendType_Recv
			, fromId
			, toId
			, userItem->m_inviteId
			, LCMessageItem::StatusType_Finish);
	// 生成VideoItem
	lcmm::LCVideoItem* videoItem = new lcmm::LCVideoItem();
	videoItem->Init(videoId
			, sendId
			, videoDesc
			, ""
			, charge);
	// 把PhotoItem添加到MessageItem
	item->SetVideoItem(videoItem);

	// 添加到用户聊天记录中
	userItem->InsertSortMsgList(item);

	// callback
	m_listener->OnRecvVideo(item);
}

void LiveChatManManager::OnRecvMagicIcon(const string& toId, const string& fromId, const string& fromName, const string& inviteId, bool charge, int ticket, TALK_MSG_TYPE msgType, const string& iconId)
{
    // 返回票根给服务器
    m_client->UploadTicket(fromId, ticket);
    
    //
    LCUserItem* userItem = m_userMgr->GetUserItem(fromId);
    if (NULL == userItem) {
        FileLog("LiveChatManManager", "OnRecvMagicIcon() getUserItem fail, fromId:%s", fromId.c_str());
        return;
    }
    userItem->m_inviteId = inviteId;
    userItem->m_userName = fromName;
    userItem->SetChatTypeWithTalkMsgType(charge, msgType);
    SetUserOnlineStatus(userItem, USTATUS_ONLINE);
    
    //生成MessageItem
    LCMessageItem* item = new LCMessageItem();
    item->Init(m_msgIdBuilder.GetAndIncrement()
               , LCMessageItem::SendType_Recv
               , fromId
               , toId
               , userItem->m_inviteId
               , LCMessageItem::StatusType_Finish);
    // 获取MagicIConItem
    LCMagicIconItem* magicIcomItem = m_magicIconMgr->GetMagicIcon(iconId);
    // 把MagicIconItem添加到MessageItem
    item->SetMagicIconItem(magicIcomItem);
    
    // 添加到用户聊天记录中
   userItem->InsertSortMsgList(item);
    
    // callback
    if (NULL != m_listener) {
        m_listener->OnRecvMagicIcon(item);
    }
    
}

void LiveChatManManager::OnRecvThemeMotion(const string& themeId, const string& manId, const string& womanId)
{

}

void LiveChatManManager::OnRecvThemeRecommend(const string& themeId, const string& manId, const string& womanId)
{
	
}

string LiveChatManManager::GetLogPath()
{
	return m_dirPath + LOG_DIR;
}

// ------------------- IRequestLiveChatControllerCallback -------------------
void LiveChatManManager::OnCheckCoupon(long requestId, bool success, Coupon item, string userId, string errnum, string errmsg)
{
	LCUserItem* userItem = m_userMgr->GetUserItem(userId);

	FileLog("LiveChatManager", "OnCheckCoupon() userId:%s, userItem:%p, success:%d, errnum:%s, errmsg:%s, status:%d"
			, userId.c_str(), userItem, success, errnum.c_str(), errmsg.c_str(), item.status);
	if (NULL != userItem)
	{
		// 判断是否外部操作（非LiveChatManManager内部操作）
		bool isOpt = false;
		m_checkCouponOptMap.lock();
		map_lock<long, long>::iterator iter = m_checkCouponOptMap.find(requestId);
		if (iter != m_checkCouponOptMap.end()) {
			isOpt = true;
			m_checkCouponOptMap.erase(iter);
		}
		m_checkCouponOptMap.unlock();

		FileLog("LiveChatManager", "OnCheckCoupon() userId:%s, isOpt:%d", userId.c_str(), isOpt);

        // 判断试聊券是否可用
        bool canUse = (item.status == CouponStatus_Yes || item.status == CouponStatus_Started);
        
		if (isOpt)
		{
			// 外部操作
            CouponStatus status = (canUse ? CouponStatus_Yes : item.status);
			m_listener->OnCheckCoupon(success, errnum, errmsg, userId, status);
		}
		else
		{
			// LiveChatManManager内部操作
			if (success && canUse)
			{
				// 尝试使用试聊券
				InsertRequestTask(REQUEST_TASK_TryUseTicket, (unsigned long)userItem);
			}
			else {
				// 检测是否有点
				InsertRequestTask(REQUEST_TASK_GetCount, (unsigned long)userItem);
			}
		}
	}
}

void LiveChatManManager::OnUseCoupon(long requestId, bool success, string errnum, string errmsg, string userId)
{
	bool isUseTryTicketOk = false;
	if (success) {
		isUseTryTicketOk = m_client->UseTryTicket(userId);
	}

	if (!isUseTryTicketOk) {
		// 使用试聊券失败，检测是否有点
		LCUserItem* userItem = m_userMgr->GetUserItem(userId);
		InsertRequestTask(REQUEST_TASK_GetCount, (unsigned long)userItem);
	}
}

void LiveChatManManager::OnQueryChatRecord(long requestId, bool success, int dbTime, list<Record> recordList, string errnum, string errmsg, string inviteId)
{
	LCUserItem* userItem = NULL;
	if (success)
	{
		// 设置服务器当前数据库时间
		LCMessageItem::SetDbTime(dbTime);

		// 插入聊天记录
		userItem = m_userMgr->GetUserItemWithInviteId(inviteId);
		if (userItem != NULL) {
			// 清除已完成的记录（保留未完成发送的记录）
			userItem->ClearFinishedMsgList();
			// 插入历史记录
			for (list<Record>::const_iterator iter = recordList.begin();
				iter != recordList.end();
				iter++)
			{
				LCMessageItem* item = new LCMessageItem();
				if (item->InitWithRecord(
						m_msgIdBuilder.GetAndIncrement(),
						m_userId,
						userItem->m_userId,
						userItem->m_inviteId,
						(*iter),
						m_emotionMgr,
						m_voiceMgr,
						m_photoMgr,
						m_videoMgr,
                        m_magicIconMgr))
				{
					userItem->InsertSortMsgList(item);
				}
				else {
					delete item;
				}
			}
			// 合并图片聊天记录
			m_photoMgr->CombineMessageItem(userItem);
			// 合并视频聊天记录
			m_videoMgr->CombineMessageItem(userItem);
		}
	}

	// callback
	m_listener->OnGetHistoryMessage(success, errnum, errmsg, userItem);
}

void LiveChatManManager::OnQueryChatRecordMutiple(long requestId, bool success, int dbTime, list<RecordMutiple> recordMutiList, string errnum, string errmsg)
{
	FileLog("LiveChatManager", "OnQueryChatRecordMutiple() requestId:%d, success:%d, dbTime:%d, recordMutiList.size:%d, errnum:%s, errmsg:%s"
			, requestId, success, dbTime, recordMutiList.size(), errnum.c_str(), errmsg.c_str());

	LCUserList userList;
	if (success)
	{
		// 设置服务器当前数据库时间
		LCMessageItem::SetDbTime(dbTime);

		FileLog("LiveChatManager", "OnQueryChatRecordMutiple() loop record");

		// 插入聊天记录
		for (list<RecordMutiple>::const_iterator iter = recordMutiList.begin();
			iter != recordMutiList.end();
			iter++)
		{
			LCUserItem* userItem = m_userMgr->GetUserItemWithInviteId((*iter).inviteId);
			if (userItem != NULL
				&& !(*iter).recordList.empty())
			{
				// 清除已完成的记录（保留未完成发送的记录）
				userItem->ClearFinishedMsgList();
				// 服务器返回的历史消息是倒序排列的
				for (list<Record>::const_iterator recordIter = (*iter).recordList.begin();
					recordIter != (*iter).recordList.end();
					recordIter++)
				{
					LCMessageItem* item = new LCMessageItem();
					if (item->InitWithRecord(
							m_msgIdBuilder.GetAndIncrement(),
							m_userId,
							userItem->m_userId,
							userItem->m_inviteId,
							(*recordIter),
							m_emotionMgr,
							m_voiceMgr,
							m_photoMgr,
							m_videoMgr,
                            m_magicIconMgr))
					{
						userItem->InsertSortMsgList(item);
					}
					else {
						delete item;
					}

					FileLog("LiveChatManager", "OnQueryChatRecordMutiple() loop record item:%p", item);
				}

				// 合并图片聊天记录
				m_photoMgr->CombineMessageItem(userItem);
				// 合并视频聊天记录
				m_videoMgr->CombineMessageItem(userItem);
				// 添加到用户数组
				userList.push_back(userItem);

				FileLog("LiveChatManager", "OnQueryChatRecordMutiple() loop user:%s", userItem->m_userId.c_str());
			}
		}
	}

	FileLog("LiveChatManager", "OnQueryChatRecordMutiple() OnGetUsersHistoryMessage() requestId:%d, success:%d, listener:%p"
			, requestId, success, m_listener);
	// callback
	m_listener->OnGetUsersHistoryMessage(success, errnum, errmsg, userList);

	// 重置ReuqestId
	m_getUsersHistoryMsgRequestId = HTTPREQUEST_INVALIDREQUESTID;

    if (success) {
        m_isGetHistory = true;
        // 使用试聊券，发送待发消息
        InsertRequestTask(REQUEST_TASK_CheckCouponWithToSendUser);
    }
}

void LiveChatManManager::OnSendPhoto(long requestId, bool success, const string& errnum, const string& errmsg, const LCSendPhotoItem& item)
{
	LCMessageItem* msgItem = m_photoMgr->GetAndRemoveRequestItem(requestId);
	if (NULL == msgItem) {
		FileLog("LiveChatManager", "OnSendPhoto() get request item fail, requestId:%d", requestId);
		return;
	}

	if (success) {
		// 发送请求成功
		LCPhotoItem* photoItem = msgItem->GetPhotoItem();
		photoItem->m_photoId = item.photoId;
		photoItem->m_sendId = item.sendId;
        // 更新PhotoItem
        photoItem = m_photoMgr->UpdatePhotoItem(photoItem, msgItem);

		// 把源文件copy到LiveChat目录下
		m_photoMgr->CopyPhotoFileToDir(photoItem, photoItem->m_srcFilePath);

		if (m_client->SendPhoto(msgItem->m_toId
				, msgItem->GetUserItem()->m_inviteId
				, photoItem->m_photoId
				, photoItem->m_sendId
				, false
				, photoItem->m_photoDesc
				, msgItem->m_msgId))
		{
			// 添加到发送map
			m_photoMgr->AddSendingItem(msgItem);
		}
		else {
			// LiveChatClient发送不成功
			msgItem->m_statusType = LCMessageItem::StatusType_Fail;
			msgItem->m_procResult.SetResult(LCC_ERR_FAIL, "", "");
			m_listener->OnSendPhoto(msgItem->m_procResult.m_errType, msgItem->m_procResult.m_errNum, msgItem->m_procResult.m_errMsg, msgItem);
		}
	}
	else {
		// 上传文件不成功
		msgItem->m_statusType = LCMessageItem::StatusType_Fail;
		msgItem->m_procResult.SetResult(LCC_ERR_FAIL, errnum, errmsg);
        if(errnum == "ERROR00003"){
            BuildAndInsertWarningWithErrType(msgItem->m_toId, LCC_ERR_NOMONEY);
        }
		m_listener->OnSendPhoto(msgItem->m_procResult.m_errType, msgItem->m_procResult.m_errNum, msgItem->m_procResult.m_errMsg, msgItem);
	}
}

void LiveChatManManager::OnSendLadyPhoto(LCC_ERR_TYPE err, const string& errmsg, int ticket)
{

}

void LiveChatManManager::OnPhotoFee(long requestId, bool success, const string& errnum, const string& errmsg)
{
	LCMessageItem* item = m_photoMgr->GetAndRemoveRequestItem(requestId);
	LCPhotoItem* photoItem = item->GetPhotoItem();
	if (NULL == item || NULL == photoItem) {
		FileLog("LiveChatManager", "OnPhotoFee() get request item fail, requestId:%d", requestId);
		return;
	}

	item->m_procResult.SetResult(success ? LCC_ERR_SUCCESS : LCC_ERR_FAIL, errnum, errmsg);
	photoItem->m_charge = success;
	photoItem->RemoveFeeStatus();

	if (success) {
		// 通知LiveChat服务器已经购买图片
		m_client->ShowPhoto(
			item->GetUserItem()->m_userId
			, item->GetUserItem()->m_inviteId
			, item->GetPhotoItem()->m_photoId
			, item->GetPhotoItem()->m_sendId
			, item->GetPhotoItem()->m_charge
			, item->GetPhotoItem()->m_photoDesc
			, item->m_msgId);
	}
	m_listener->OnPhotoFee(success, errnum, errmsg, item);
}

void LiveChatManManager::OnGetPhoto(long requestId, bool success, const string& errnum, const string& errmsg, const string& filePath)
{
	FileLog("LiveChatManager", "OnGetPhoto() begin");
	m_photoMgr->OnGetPhoto(requestId, success, errnum, errmsg, filePath);
	FileLog("LiveChatManager", "OnGetPhoto() end");
}

void LiveChatManManager::OnUploadVoice(long requestId, bool success, const string& errnum, const string& errmsg, const string& voiceId)
{
	LCMessageItem* item = m_voiceMgr->GetAndRemoveRquestItem(requestId);
	LCVoiceItem* voiceItem = item->GetVoiceItem();
	if (NULL == voiceItem) {
		FileLog("LiveChatManager", "OnUploadVoice() param fail. voiceItem is NULL.");
		m_listener->OnSendVoice(LCC_ERR_FAIL, "", "", item);
	}

	if (success) {
		voiceItem->m_voiceId = voiceId;
		if (m_client->SendVoice(item->m_toId, voiceItem->m_voiceId, voiceItem->m_timeLength, item->m_msgId)) {
			m_voiceMgr->AddSendingItem(item->m_msgId, item);
		}
		else {
			m_listener->OnSendVoice(LCC_ERR_FAIL, "", "", item);
		}
	}
	else {
		item->m_statusType = LCMessageItem::StatusType_Fail;
		item->m_procResult.SetResult(LCC_ERR_FAIL, errnum, errmsg);
		m_listener->OnSendVoice(item->m_procResult.m_errType, item->m_procResult.m_errNum, item->m_procResult.m_errMsg, item);
	}
}

void LiveChatManManager::OnPlayVoice(long requestId, bool success, const string& errnum, const string& errmsg, const string& filePath)
{
	LCMessageItem* item = m_voiceMgr->GetAndRemoveRquestItem(requestId);
	if (NULL != item) {
		item->m_procResult.SetResult(success ? LCC_ERR_SUCCESS : LCC_ERR_FAIL, errnum, errmsg);

		// 处理完成
		if (NULL != item->GetVoiceItem()) {
			item->GetVoiceItem()->m_processing = false;
		}
		m_listener->OnGetVoice(item->m_procResult.m_errType, item->m_procResult.m_errNum, item->m_procResult.m_errMsg, item);
	}

	FileLog("LiveChatManager", "OnPlayVoice() end, requestId:%d, isSuccess:%d, errnum:%s, errmsg:%s, filePath:%s"
			, requestId, success, errnum.c_str(), errmsg.c_str(), filePath.c_str());
}

void LiveChatManManager::OnGetVideoPhoto(long requestId, bool success, const string& errnum, const string& errmsg, const string& filePath)
{
	// 回调到videoMgr
	m_videoMgr->OnGetVideoPhoto(requestId, success, errnum, errmsg, filePath);
}

void LiveChatManManager::OnGetVideo(long requestId, bool success, const string& errnum, const string& errmsg, const string& url)
{
	LCMessageItem* item = m_videoMgr->RemovePhotoFee(requestId);
	if (NULL != item && NULL != item->GetVideoItem())
	{
		lcmm::LCVideoItem* videoItem = item->GetVideoItem();
		videoItem->m_charge = true;
		videoItem->RemoveProcessStatusFee();
		videoItem->m_videoUrl = url;

		// 通知LiveChat服务器已经购买视频
		m_client->PlayVideo(
			item->m_fromId
			, item->m_inviteId
			, videoItem->m_videoId
			, videoItem->m_sendId
			, videoItem->m_charge
			, videoItem->m_videoDesc
			, item->m_msgId);
	}

	// callback
	m_listener->OnVideoFee(success, errnum, errmsg, item);
}

void LiveChatManManager::OnGetMagicIconConfig(long requestId, bool success, const string& errnum, const string& errmsg,const MagicIconConfig& config)
{
    FileLog("LiveChatManager", "OnGetMagicIconConfig() OnMagicIconConfig begin");
    bool isSuccess = success;
    MagicIconConfig configItem = config;
    if(isSuccess){
        if (m_magicIconMgr->IsVerNewThenConfigItem(config.maxupdatetime)) {
            isSuccess = m_magicIconMgr->UpdateConfigItem(config);
        }
        else{
            configItem = m_magicIconMgr->GetConfigItem();
        }
    }
    FileLog("LiveChatManager", "OnGetMagicIconConfig() OnMagicIconConfig callback");
    if (NULL != m_listener) {
        m_listener->OnGetMagicIconConfig(success, errnum, errmsg, config);
    }
    m_magicIconMgr->m_magicIconConfigReqId = HTTPREQUEST_INVALIDREQUESTID;
    FileLog("LiveChatManager", "OnGetMagicIconConfig() OnMagicIconCongig end");
}

// ------------------- IRequestOtherControllerCallback -------------------
void LiveChatManManager::OnEmotionConfig(long requestId, bool success, const string& errnum, const string& errmsg, const OtherEmotionConfigItem& item)
{
	FileLog("LiveChatManager", "OnEmotionConfig() OnOtherEmotionConfig begin");
	bool isSuccess = success;
	OtherEmotionConfigItem configItem = item;
	if (isSuccess) {
		// 请求成功
		if (m_emotionMgr->IsVerNewThenConfigItem(item.version)) {
			// 配置版本更新
			isSuccess = m_emotionMgr->UpdateConfigItem(item);
		}
		else {
			// 使用旧配置
			configItem = m_emotionMgr->GetConfigItem();
		}
	}
	FileLog("LiveChatManager", "OnEmotionConfig() OnOtherEmotionConfig callback");
	if (NULL != m_listener) {
		m_listener->OnGetEmotionConfig(isSuccess, errnum, errmsg, item);
	}
	m_emotionMgr->m_emotionConfigReqId = HTTPREQUEST_INVALIDREQUESTID;
	FileLog("LiveChatManager", "OnEmotionConfig() OnOtherEmotionConfig end");
}

//// 男士会员统计回调（money，coupon等等）
void LiveChatManManager::OnGetCount(long requestId, bool success, const string& errnum, const string& errmsg, const OtherGetCountItem& item)
{
    FileLog("LiveChatManager", "OnGetCount() begin");
    bool isSuccess = success;
    OtherGetCountItem CountItem = item;
    LCUserItem* userItem = NULL;
    // 获取待发消息的userItem
    m_inviteMsgMap.lock();
    map<long, LCUserItem*>::iterator iter = m_inviteMsgMap.find(requestId);
    if (iter != m_inviteMsgMap.end()) {
        userItem = (*iter).second;
        m_inviteMsgMap.erase(iter);
    }
    m_inviteMsgMap.unlock();
    if (isSuccess) {
        // 判断点是不是大于聊天最少点
        if (CountItem.money >= m_minBalance && NULL != userItem) {
                // 发送代发消息
                SendMessageList(userItem);
        }
        else{
                // 失败
                LCC_ERR_TYPE errType = LCC_ERR_NOMONEY;
                SendMessageListFailProc(userItem, errType);
        }
    }
    // 连接失败
    else
    {
        LCC_ERR_TYPE fErrType = LCC_ERR_CONNECTFAIL;
        SendMessageListFailProc(userItem, fErrType);
    }
}

// ------------------- 请求线程 -------------------
// 请求线程启动
bool LiveChatManManager::StartRequestThread()
{
	bool result = false;

	// 停止之前的请求线程
	StopRequestThread();

	// 启动线程
	m_requestThread = IThreadHandler::CreateThreadHandler();
	if (NULL != m_requestThread)
	{
		m_requestThreadStart = true;
		result = m_requestThread->Start(LiveChatManManager::RequestThread, this);

		if (!result) {
			m_requestThreadStart = false;
		}
	}

	return result;
}

// 请求线程结束
void LiveChatManManager::StopRequestThread()
{
	if (NULL != m_requestThread)
	{
		m_requestThreadStart = false;
		m_requestThread->WaitAndStop();
		IThreadHandler::ReleaseThreadHandler(m_requestThread);
		m_requestThread = NULL;
	}
}

// 请求线程函数
TH_RETURN_PARAM LiveChatManManager::RequestThread(void* obj)
{
	LiveChatManManager* pThis = (LiveChatManManager*)obj;
	pThis->RequestThreadProc();
	return 0;
}

// 请求线程处理函数
void LiveChatManManager::RequestThreadProc()
{
	while (m_requestThreadStart)
	{
		RequestItem item = PopRequestTask();
		if (REQUEST_TASK_Unknow != item.requestType)
		{
			if (getCurrentTime() >= item.delayTime)
			{
				switch (item.requestType)
				{
                case REQUEST_TASK_Unknow:break;
				case REQUEST_TASK_GetEmotionConfig:
					// 获取高级表情配置
					GetEmotionConfig();
					break;
                case REQUEST_TASK_GetMagicIconConfig:
                     // 获取小高级表情配置
                     GetMagicIconConfig();
                     break;
				case REQUEST_TASK_AutoRelogin:
					// 执行自动重登录流程
					AutoRelogin();
					break;
				case REQUEST_TASK_GetUsersHistoryMessage:
					// 获取聊天历史记录
					{
						list<string> userIds;
                        
                        // 获取在聊用户ID
						LCUserList userList = m_userMgr->GetChatingUsers();
						for (LCUserList::const_iterator iter = userList.begin();
							iter != userList.end();
							iter++)
						{
							userIds.push_back((*iter)->m_userId);
						}
                        
                        // 获取男士主动邀请用户ID
                        userList = m_userMgr->GetManInviteUsers();
                        for (LCUserList::const_iterator iter = userList.begin();
                             iter != userList.end();
                             iter++)
                        {
                            userIds.push_back((*iter)->m_userId);
                        }

						if (!userIds.empty()) {
							GetUsersHistoryMessage(userIds);
                        }
                        else
                        {
                            // 不需要获取历史纪录
                            m_isGetHistory = true;
                        }
					}
					break;
				case REQUEST_TASK_CheckCouponWithToSendUser:
					// 有待发消息的用户调用检测试聊券流程
					{
						LCUserList userList = m_userMgr->GetToSendUsers();
						for (LCUserList::iterator iter = userList.begin();
							iter != userList.end();
							iter++)
						{
							LCUserItem* userItem = (*iter);
							if (NULL != userItem) {
								if ( IsCheckCoupon(userItem) || !CheckCouponProc(userItem)) {
                                    // 执行失败，照常发送
                                    SendMessageList(userItem);
								}
							}
						}
					}
					break;
				case REQUEST_TASK_SendMessageList:
					// 发送指定用户的待发消息
				{
					LCUserItem* userItem = (LCUserItem*)item.param;
					if (NULL != userItem) {
						SendMessageListProc(userItem);
					}
				}
					break;
				case REQUEST_TASK_SendMessageListNoMoneyFail:
					// 处理指定用户的待发消息发送不成功(余额不足)
					break;
				case REQUEST_TASK_SendMessageListConnectFail:
					// 处理指定用户的待发消息发送不成功(连接失败)
					break;
				case REQUEST_TASK_TryUseTicket:
					// 执行使用试聊券流程
					{
						LCUserItem* userItem = (LCUserItem*)item.param;
						if (NULL != userItem) {
							if (!UseTryTicketProc(userItem)) {
								// 执行失败，检测是否有点
								InsertRequestTask(REQUEST_TASK_GetCount, (unsigned long)userItem);
							}
						}
					}
					break;
				case REQUEST_TASK_GetCount:
					// 执行获取余额流程
					{
						LCUserItem* userItem = (LCUserItem*)item.param;
						if (NULL != userItem) {
							// 暂不检测是否有足够点数，直接发送
							if (!GetCountProc(userItem)) {
								// 执行失败，照常发送
								//SendMessageList(userItem);
                                LCC_ERR_TYPE errType = LCC_ERR_CONNECTFAIL;
                                SendMessageListFailProc(userItem, errType);
							}
						}
					}
					break;
				case REQUEST_TASK_LoginManagerLogout:
					// LoginManager注销
					break;
				case REQUEST_TASK_ReleaseEmotionDownloader:
					// 释放高级表情downloader
					{
						m_emotionMgr->ClearFinishDownloaderWithTimer();
						static const long stepTime = 5 * 1000; // 每5秒释放一次
						InsertRequestTask(REQUEST_TASK_ReleaseEmotionDownloader, 0, stepTime);
					}
					break;
				case REQUEST_TASK_ReleasePhotoDownloader:
					// 释放图片downloader
					{
						m_photoMgr->ClearFinishDownloaderWithTimer();
						static const long stepTime = 5 * 1000; // 每5秒释放一次
						InsertRequestTask(REQUEST_TASK_ReleasePhotoDownloader, 0, stepTime);
					}
					break;
				case REQUEST_TASK_ReleaseVideoPhotoDownloader:
					// 释放视频图片downloader
					{
						m_videoMgr->ClearFinishVideoPhotoDownloaderWithTimer();
						static const long stepTime = 5 * 1000; // 每5秒释放一次
						InsertRequestTask(REQUEST_TASK_ReleaseVideoPhotoDownloader, 0, stepTime);
					}
					break;
				case REQUEST_TASK_ReleaseVideoDownloader:
					// 释放视频downloader
					{
						m_videoMgr->ClearFinishVideoDownloaderWithTimer();
						static const long stepTime = 5 * 1000; // 每5秒释放一次
						InsertRequestTask(REQUEST_TASK_ReleaseVideoDownloader, 0, stepTime);
					}
					break;
                case REQUEST_TASK_ReleaseMagicIconDownloader:
                        // 释放小高级表情downloader
                    {
                        m_magicIconMgr->ClearFinishDownloaderWithTimer();
                        static const long stepTime = 5 * 1000; // 每5秒释放一次
                        InsertRequestTask(REQUEST_TASK_ReleaseMagicIconDownloader, 0, stepTime);
                    }
                    break;
				}
			}
			else 
			{
				// 任务时间未到
				PushRequestTask(item);
				Sleep(50);
			}
		}
		else
		{
			// 请求队列为空
			Sleep(50);
		}
	}
}

bool LiveChatManManager::CheckCouponProc(LCUserItem* userItem)
{
	bool result = false;
	if (NULL != userItem)
	{
		long requestId = m_requestController->CheckCoupon(userItem->m_userId);
		result = HTTPREQUEST_INVALIDREQUESTID != requestId;

		FileLog("LiveChatManager", "CheckCouponProc() userId:%s", userItem->m_userId.c_str());
	}
	return result;
}

bool LiveChatManManager::UseTryTicketProc(LCUserItem* userItem)
{
	bool result = false;
	if (NULL != userItem)
	{
		long requestId = m_requestController->UseCoupon(userItem->m_userId);
		result = HTTPREQUEST_INVALIDREQUESTID != requestId;
	}
	return result;
}

bool LiveChatManManager::GetCountProc(LCUserItem* userItem)
{
	bool result = false;
	if (NULL != userItem)
	{
//		long requestId = m_requestController(userItem->m_userId);
//		result = HTTPREQUEST_INVALIDREQUESTID != requestId;
        long requestId = m_requestOtherController->GetCount(true, false, false, false, false, false);
        result = HTTPREQUEST_INVALIDREQUESTID != requestId;
        if (result) {
            m_inviteMsgMap.lock();
            m_inviteMsgMap.insert(map<long, LCUserItem*>::value_type(requestId, userItem));
            m_inviteMsgMap.unlock();
        }
		// 直接发送
		//SendMessageList(userItem);
	}
	return result;
}

// 判断请求队列是否为空
bool LiveChatManManager::IsRequestQueueEmpty()
{
	bool result = false;
	// 加锁
	if (NULL != m_requestQueueLock) {
		m_requestQueueLock->Lock();
	}

	// 处理
	result = m_requestQueue.empty();

	// 解锁
	if (NULL != m_requestQueueLock) {
		m_requestQueueLock->Unlock();
	}
	return result;
}

// 弹出请求队列第一个item
LiveChatManManager::RequestItem LiveChatManManager::PopRequestTask()
{
	RequestItem item;

	// 加锁
	if (NULL != m_requestQueueLock) {
		m_requestQueueLock->Lock();
	}

	// 处理
	if (!m_requestQueue.empty())
	{
		item = m_requestQueue.front();
		m_requestQueue.pop_front();
	}

	// 解锁
	if (NULL != m_requestQueueLock) {
		m_requestQueueLock->Unlock();
	}

	return item;
}

// 插入请求队列
bool LiveChatManManager::PushRequestTask(const RequestItem& item)
{
	bool result = false;

	// 加锁
	if (NULL != m_requestQueueLock) {
		m_requestQueueLock->Lock();
	}

	// 处理
	m_requestQueue.push_back(item);
	result = true;

	// 解锁
	if (NULL != m_requestQueueLock) {
		m_requestQueueLock->Unlock();
	}

	return result;
}

// 清空请求队列
void LiveChatManManager::CleanRequestTask()
{
	// 加锁
	if (NULL != m_requestQueueLock) {
		m_requestQueueLock->Lock();
	}

	// 处理
	m_requestQueue.clear();

	// 解锁
	if (NULL != m_requestQueueLock) {
		m_requestQueueLock->Unlock();
	}
}

// 插入请求任务
void LiveChatManManager::InsertRequestTask(REQUEST_TASK_TYPE requestType, unsigned long param, long delayTime)
{
	RequestItem requestItem;
	requestItem.requestType = requestType;
	requestItem.param = param;
	if (delayTime > 0) {
		requestItem.delayTime = getCurrentTime() + delayTime;
	}
	PushRequestTask(requestItem);
}

// 插入请求参数map表
bool LiveChatManManager::PushRequestMap(long requestId, void* param)
{
	bool result = false;
	// 加锁
	if (NULL != m_requestMapLock) {
		m_requestMapLock->Lock();
	}

	// 处理
	if (HTTPREQUEST_INVALIDREQUESTID != requestId
		&& NULL != param)
	{
		RequestMap::iterator iter = m_requestMap.find(requestId);
		if (m_requestMap.end() == iter)
		{
			m_requestMap.insert(RequestMap::value_type(requestId, param));
			result = true;
		}
	}

	// 加锁
	if (NULL != m_requestMapLock) {
		m_requestMapLock->Lock();
	}
	return result;
}

// 获取并移除请求参数
void* LiveChatManManager::RemoveRequestMap(long requestId)
{
	void* param = NULL;
	// 加锁
	if (NULL != m_requestMapLock) {
		m_requestMapLock->Lock();
	}

	// 处理
	RequestMap::iterator iter = m_requestMap.find(requestId);
	if (m_requestMap.end() != iter)
	{
		param = (*iter).second;
		m_requestMap.erase(iter);
	}

	// 加锁
	if (NULL != m_requestMapLock) {
		m_requestMapLock->Lock();
	}
	return param;
}

// 清空请求参数map表
void LiveChatManManager::CleanRequestMap()
{
	// 加锁
	if (NULL != m_requestMapLock) {
		m_requestMapLock->Lock();
	}

	// 处理
	m_requestMap.clear();

	// 加锁
	if (NULL != m_requestMapLock) {
		m_requestMapLock->Lock();
	}
}
