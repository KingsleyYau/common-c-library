/*
 * author: Samson.Fan
 *   date: 2015-03-19
 *   file: LiveChatClient.h
 *   desc: LiveChat客户端实现类
 */

#pragma once

#include "ILiveChatClient.h"
#include "ITaskManager.h"
#include "IThreadHandler.h"
#include "Counter.h"

class CLiveChatClient : public ILiveChatClient
					  , private ITaskManagerListener
{
public:
	CLiveChatClient();
	virtual ~CLiveChatClient();

// ILiveChatClient接口函数
public:
	// 调用所有接口函数前需要先调用Init
	virtual bool Init(const list<string>& svrIPs, unsigned int svrPort, ILiveChatClientListener* listener);
	// 判断是否无效seq
	virtual bool IsInvalidSeq(int seq);
	// 登录
	virtual bool Login(const string& user, const string& password, const string& deviceId, CLIENT_TYPE clientType, USER_SEX_TYPE sexType, AUTH_TYPE authType = AUTH_TYPE_SID);
	// 注销
	virtual bool Logout();
	// 设置在线状态
	virtual bool SetStatus(USER_STATUS_TYPE status);
	// 结束聊天
	virtual bool EndTalk(const string& userId);
	// 获取用户在线状态
	virtual bool GetUserStatus(const UserIdList& list);
	// 获取会话信息
	virtual bool GetTalkInfo(const string& userId);
	// 获取会话信息(仅男士端使用)
	virtual bool GetSessionInfo(const string& userId);
	// 上传票根
	virtual bool UploadTicket(const string& userId, int ticket);
	// 通知对方女士正在编辑消息
	virtual bool SendLadyEditingMsg(const string& userId);
	// 发送聊天消息
	virtual bool SendTextMessage(const string& userId, const string& message, bool illegal, int ticket);
	// 发送高级表情
	virtual bool SendEmotion(const string& userId, const string& emotionId, int ticket);
	// 发送虚拟礼物
	virtual bool SendVGift(const string& userId, const string& giftId, int ticket);
	// 获取语音发送验证码
	virtual bool GetVoiceCode(const string& userId, int ticket);
	// 获取女士语音发送验证码
	virtual bool GetLadyVoiceCode(const string& userId);
	// 发送语音
	virtual bool SendVoice(const string& userId, const string& voiceId, int length, int ticket);
	// 使用试聊券
	virtual bool UseTryTicket(const string& userId);
	// 获取邀请列表或在聊列表
	virtual bool GetTalkList(int listType);
	// 发送图片
	virtual bool SendPhoto(const string& userId, const string& inviteId, const string& photoId, const string& sendId, bool charget, const string& photoDesc, int ticket);
	// 女士发送图片
	virtual bool SendLadyPhoto(const string& userId, const string& inviteId, const string& photoId, const string& sendId, bool charge, const string& photoDesc, int ticket);
	// 显示图片
	virtual bool ShowPhoto(const string& userId, const string& inviteId, const string& photoId, const string& sendId, bool charget, const string& photoDesc, int ticket);
	// 获取用户信息
	virtual bool GetUserInfo(const string& userId);
	// 获取多个用户信息
	virtual int GetUsersInfo(const list<string>& userIdList);
	// 获取联系人/黑名单列表
	virtual bool GetContactList(CONTACT_LIST_TYPE listType);
	// 上传客户端版本号
	virtual bool UploadVer(const string& ver);
	// 获取被屏蔽女士列表
	virtual bool GetBlockUsers();
	// 获取最近联系人列表
	virtual bool GetRecentContactList();
	// 搜索在线男士
	virtual bool SearchOnlineMan(int beginAge, int endAge);
	// 回复验证码
	virtual bool ReplyIdentifyCode(string identifyCode);
	// 刷新验证码
	virtual bool RefreshIdentifyCode();
	// 刷新邀请模板
	virtual bool RefreshInviteTemplate();
	// 获取已扣费联系人列表
	virtual bool GetFeeRecentContactList();
	// 获取女士聊天信息（包括在聊及邀请的男士列表等）
	virtual bool GetLadyChatInfo();
	// 播放视频
	virtual bool PlayVideo(const string& userId, const string& inviteId, const string& videoId, const string& sendId, bool charget, const string& videoDesc, int ticket);
	// 女士发送微视频
	virtual bool SendLadyVideo(const string& userId, const string& inviteId, const string& videoId, const string& sendId, bool charge, const string& videoDesc, int ticket);
	// 获取女士择偶条件
	virtual bool GetLadyCondition(const string& userId);
	// 获取女士自定义邀请模板
	virtual bool GetLadyCustomTemplate(const string& userId);
	// 弹出女士自动邀请消息通知
	virtual bool UploadPopLadyAutoInvite(const string& userId, const string& msg, const string& key);
	// 上传自动充值状态
	virtual bool UploadAutoChargeStatus(bool isCharge);
	// 发送小高级表情
	virtual bool SendMagicIcon(const string& userId, const string& iconId, int ticket);
	// 获取指定男/女士已购主题包
	virtual bool GetPaidTheme(const string& userId);
	// 获取男/女士所有已购主题包
	virtual bool GetAllPaidTheme();
	// 上传主题包列表版本号
	virtual bool UploadThemeListVer(int themeVer);
	// 男士购买主题包
	virtual bool ManFeeTheme(const string& userId, const string& themeId);
	// 男士应用主题包
	virtual bool ManApplyTheme(const string& userId, const string& themeId);
	// 男/女士播放主题包动画
	virtual bool PlayThemeMotion(const string& userId, const string& themeId);
	// 获取自动邀请状态（仅女士）
	virtual bool GetAutoInviteMsgSwitchStatus();
	// 启动/关闭发送自动邀请消息（仅女士）
	virtual bool SwitchAutoInviteMsg(bool isOpen);
	// 女士推荐男士购买主题包（仅女士）
	virtual bool RecommendThemeToMan(const string& userId, const string& themeId);
	// 获取女士Cam状态
	virtual bool GetLadyCamStatus(const string& userId);
	// 发送CamShare邀请
	virtual bool SendCamShareInvite(const string& userId, const string& camShareMsg);
	// 男士发起CamShare并开始扣费
	virtual bool ApplyCamShare(const string& userId);
	// 女士接受男士Cam邀请
	virtual bool LadyAcceptCamInvite(const string& userId, const string& camShareMsg, bool isOpenCam);
    // CamShare聊天扣费心跳
	virtual bool CamShareHearbeat(const string& userId, const string& inviteId);
	// 批量获取女士端Cam状态
	virtual bool GetUsersCamStatus(const UserIdList& list);
	// Camshare使用试聊券
	virtual bool CamshareUseTryTicket(const string& targetId, const string& ticketId);
public:
	// 获取用户账号
	virtual string GetUser();

// ITaskManagerListener接口函数
private:
	// 连接成功回调
	virtual void OnConnect(bool success);
	// 连接失败回调(listUnsentTask：未发送/未回复的task列表)
	virtual void OnDisconnect(const TaskList& list);
	// 已完成交互的task
	virtual void OnTaskDone(ITask* task);

// 处理完成交互的task函数
private:
	void OnCheckVerTaskDone(ITask* task);

private:
	// 连接服务器
	bool ConnectServer();
	// 检测版本号
	bool CheckVersionProc();
	// 登录
	bool LoginProc();
	// 上传设备ID
	bool UploadDeviceIdProc();
	// 上传设备类型
	bool UploadDeviceTypeProc();
	// 启动发送心跳包线程
	void HearbeatThreadStart();

protected:
	static TH_RETURN_PARAM HearbeatThread(void* arg);
	void HearbeatProc();

private:
	bool			m_bInit;	// 初始化标志
	list<string>	m_svrIPs;	// 服务器ip列表
	string			m_svrIp;	// 当前连接的服务器IP
	unsigned int	m_svrPort;	// 服务器端口

	string			m_user;			// 用户名
	string			m_password;		// 密码
	string			m_deviceId;		// 设备ID
	CLIENT_TYPE		m_clientType;	// 客户端类型
	USER_SEX_TYPE	m_sexType;		// 性别
	AUTH_TYPE		m_authType;		// 认证类型

	Counter			m_seqCounter;	// seq计数器

	ITaskManager*	m_taskManager;	// 任务管理器
	ILiveChatClientListener*	m_listener;	// 监听器

	bool			m_isHearbeatThreadRun;	// 心跳线程运行标志
	IThreadHandler*		m_hearbeatThread;	// 心跳线程
};
