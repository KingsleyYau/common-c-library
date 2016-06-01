/*
 * author: Samson.Fan
 *   date: 2015-03-19
 *   file: LiveChatClient.cpp
 *   desc: LiveChat客户端实现类
 */

#include "LiveChatClient.h"
#include "TaskManager.h"
#include <common/KLog.h>
#include <common/CommonFunc.h>
#include <common/CheckMemoryLeak.h>

// task include
#include "CheckVerTask.h"
#include "LoginTask.h"
#include "SetStatusTask.h"
#include "UploadDeviceIdTask.h"
#include "UploadDeviceTypeTask.h"
#include "EndTalkTask.h"
#include "GetUserStatusTask.h"
#include "GetTalkInfoTask.h"
#include "UploadTicketTask.h"
#include "SendMsgTask.h"
#include "SendEmotionTask.h"
#include "SendVGiftTask.h"
#include "GetVoiceCodeTask.h"
#include "GetLadyVoiceCodeTask.h"
#include "SendVoiceTask.h"
#include "UseTryTicketTask.h"
#include "GetTalkListTask.h"
#include "SendPhotoTask.h"
#include "SendLadyPhotoTask.h"
#include "ShowPhotoTask.h"
#include "GetUserInfoTask.h"
#include "GetUsersInfoTask.h"
#include "GetContactListTask.h"
#include "UploadVerTask.h"
#include "GetBlockUsersTask.h"
#include "GetRecentContactListTask.h"
#include "SearchOnlineManTask.h"
#include "ReplyIdentifyCodeTask.h"
#include "RefreshIdentifyCodeTask.h"
#include "RefreshInviteTemplateTask.h"
#include "GetFeeRecentContactListTask.h"
#include "GetLadyChatInfoTask.h"
#include "SendLadyEditingMsgTask.h"
#include "PlayVideoTask.h"
#include "SendLadyVideoTask.h"
#include "GetLadyConditionTask.h"
#include "GetLadyCustomTemplateTask.h"
#include "UploadPopLadyAutoInviteTask.h"
#include "UploadAutoChargeStatusTask.h"
#include "SendMagicIconTask.h"
#include "GetPaidThemeTask.h"
#include "GetAllPaidThemeTask.h"
#include "UploadThemeListVerTask.h"
#include "ManFeeThemeTask.h"
#include "ManApplyThemeTask.h"
#include "PlayThemeMotionTask.h"
#include "HearbeatTask.h"

CLiveChatClient::CLiveChatClient()
{
	m_taskManager = NULL;
	m_listener = NULL;
	m_bInit = false;
	m_isHearbeatThreadRun = false;
	m_hearbeatThread = NULL;
}

CLiveChatClient::~CLiveChatClient()
{
	FileLog("LiveChatClient", "CLiveChatClient::~CLiveChatClient()");
	delete m_taskManager;
	m_taskManager = NULL;
	FileLog("LiveChatClient", "CLiveChatClient::~CLiveChatClient() end");
}

// ------------------------ ILiveChatClient接口函数 -------------------------
// 调用所有接口函数前需要先调用Init
bool CLiveChatClient::Init(const list<string>& svrIPs, unsigned int svrPort, ILiveChatClientListener* listener)
{
	bool result = false;

	// 初始化 TaskManager
	if (NULL == m_taskManager) {
		m_taskManager = new CTaskManager();
		if (NULL != m_taskManager) {
			result = m_taskManager->Init(svrIPs, svrPort, listener, this);
		}

		// 初始化 seq计数器
		if (result) {
			result = m_seqCounter.Init();
		}

		if (result) {
			// 所有初始化都成功，开始赋值
			m_listener = listener;
		}

		m_bInit = result;
	}

	return result;
}

// 判断是否无效seq
bool CLiveChatClient::IsInvalidSeq(int seq)
{
	return m_seqCounter.IsInvalidValue(seq);
}

// 连接服务器
bool CLiveChatClient::ConnectServer()
{
	bool result = false;

	FileLog("LiveChatClient", "CLiveChatClient::ConnectServer() begin");

	if (m_bInit) {
		if (NULL != m_taskManager) {
			if (m_taskManager->IsStart()) {
				m_taskManager->Stop();
			}
			result = m_taskManager->Start();
			FileLog("LiveChatClient", "CLiveChatClient::ConnectServer() result: %d", result);
		}
	}

	FileLog("LiveChatClient", "CLiveChatClient::ConnectServer() end");

	return result;
}

// 登录
bool CLiveChatClient::Login(const string& user, const string& password, const string& deviceId, CLIENT_TYPE clientType, USER_SEX_TYPE sexType, AUTH_TYPE authType)
{
	bool result = false;

	FileLog("LiveChatClient", "CLiveChatClient::Login() begin");

	if (!user.empty()
		&& !password.empty()
		&& !deviceId.empty()
		&& ConnectServer())
	{
		m_user = user;
		m_password = password;
		m_deviceId = deviceId;
		m_clientType = clientType;
		m_sexType = sexType;
		m_authType = authType;

		result = true;
	}

	FileLog("LiveChatClient", "CLiveChatClient::Login() end");

	return result;
}

// 注销
bool CLiveChatClient::Logout()
{
	bool result = false;

	FileLog("LiveChatClient", "CLiveChatClient::Logout() begin, m_taskManager:%p", m_taskManager);

	if (NULL != m_taskManager) {
		FileLog("LiveChatClient", "CLiveChatClient::Logout() m_taskManager->Stop(), m_taskManager:%p", m_taskManager);
		result = m_taskManager->Stop();

		if (result) {
			m_user = "";
			m_password = "";
		}
	}

	FileLog("LiveChatClient", "CLiveChatClient::Logout() end");

	return result;
}

// 设置在线状态
bool CLiveChatClient::SetStatus(USER_STATUS_TYPE status)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::SetStatus() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		SetStatusTask* task = new SetStatusTask();
		FileLog("LiveChatClient", "CLiveChatClient::SetStatus() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(status);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::SetStatus() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::SetStatus() end");
	return result;
}

// 结束聊天
bool CLiveChatClient::EndTalk(const string& userId)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::EndTalk() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		EndTalkTask* task = new EndTalkTask();
		FileLog("LiveChatClient", "CLiveChatClient::EndTalk() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::EndTalk() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::EndTalk() end");
	return result;
}

// 获取用户在线状态
bool CLiveChatClient::GetUserStatus(const UserIdList& list)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::GetUserStatus() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		GetUserStatusTask* task = new GetUserStatusTask();
		FileLog("LiveChatClient", "CLiveChatClient::GetUserStatus() task:%p", task);
		if (NULL != task) {
			// 转换成对方的性别
			USER_SEX_TYPE sexType = (m_sexType == USER_SEX_FEMALE ? USER_SEX_MALE : USER_SEX_FEMALE);

			result = task->Init(m_listener);
			result = result && task->InitParam(sexType, list);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::GetUserStatus() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::GetUserStatus() end");
	return result;
}

// 获取会话信息
bool CLiveChatClient::GetTalkInfo(const string& userId)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::GetTalkInfo() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		GetTalkInfoTask* task = new GetTalkInfoTask();
		FileLog("LiveChatClient", "CLiveChatClient::GetTalkInfo() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::GetTalkInfo() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::GetTalkInfo() end");
	return result;
}

// 上传票根
bool CLiveChatClient::UploadTicket(const string& userId, int ticket)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::UploadTicket() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		UploadTicketTask* task = new UploadTicketTask();
		FileLog("LiveChatClient", "CLiveChatClient::UploadTicket() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId, ticket);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::UploadTicket() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::UploadTicket() end");
	return result;
}

// 通知对方女士正在编辑消息
bool CLiveChatClient::SendLadyEditingMsg(const string& userId)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::SendLadyEditingMsg() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		SendLadyEditingMsgTask* task = new SendLadyEditingMsgTask();
		FileLog("LiveChatClient", "CLiveChatClient::SendLadyEditingMsg() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::SendLadyEditingMsg() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::SendLadyEditingMsg() end");
	return result;
}

// 发送聊天消息
bool CLiveChatClient::SendTextMessage(const string& userId, const string& message, bool illegal, int ticket)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::SendMessage() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		SendMsgTask* task = new SendMsgTask();
		FileLog("LiveChatClient", "CLiveChatClient::SendMessage() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId, message, illegal, ticket);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::SendMessage() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::SendMessage() end");
	return result;
}

// 发送高级表情
bool CLiveChatClient::SendEmotion(const string& userId, const string& emotionId, int ticket)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::SendEmotion() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		SendEmotionTask* task = new SendEmotionTask();
		FileLog("LiveChatClient", "CLiveChatClient::SendEmotion() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId, emotionId, ticket);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::SendEmotion() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::SendEmotion() end");
	return result;
}

// 发送虚拟礼物
bool CLiveChatClient::SendVGift(const string& userId, const string& giftId, int ticket)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::SendVGift() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		SendVGiftTask* task = new SendVGiftTask();
		FileLog("LiveChatClient", "CLiveChatClient::SendVGift() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId, giftId, ticket);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::SendVGift() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::SendVGift() end");
	return result;
}

// 获取语音发送验证码
bool CLiveChatClient::GetVoiceCode(const string& userId, int ticket)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::GetVoiceCode() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		GetVoiceCodeTask* task = new GetVoiceCodeTask();
		FileLog("LiveChatClient", "CLiveChatClient::GetVoiceCode() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId, ticket);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::GetVoiceCode() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::GetVoiceCode() end");
	return result;
}

// 获取女士语音发送验证码
bool CLiveChatClient::GetLadyVoiceCode(const string& userId)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::GetLadyVoiceCode() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		GetLadyVoiceCodeTask* task = new GetLadyVoiceCodeTask();
		FileLog("LiveChatClient", "CLiveChatClient::GetLadyVoiceCode() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::GetLadyVoiceCode() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::GetLadyVoiceCode() end");
	return result;
}

// 发送语音
bool CLiveChatClient::SendVoice(const string& userId, const string& voiceId, int length, int ticket)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::SendVoice() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		SendVoiceTask* task = new SendVoiceTask();
		FileLog("LiveChatClient", "CLiveChatClient::SendVoice() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId, voiceId, length, ticket);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::SendVoice() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::SendVoice() end");
	return result;
}

// 使用试聊券
bool CLiveChatClient::UseTryTicket(const string& userId)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::UseTryTicket() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		UseTryTicketTask* task = new UseTryTicketTask();
		FileLog("LiveChatClient", "CLiveChatClient::UseTryTicket() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::UseTryTicket() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::UseTryTicket() end");
	return result;
}

// 获取邀请列表或在聊列表
bool CLiveChatClient::GetTalkList(int listType)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::GetTalkList() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		GetTalkListTask* task = new GetTalkListTask();
		FileLog("LiveChatClient", "CLiveChatClient::GetTalkList() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(listType);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::GetTalkList() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::GetTalkList() end");
	return result;
}

// 发送图片
bool CLiveChatClient::SendPhoto(const string& userId, const string& inviteId, const string& photoId, const string& sendId, bool charget, const string& photoDesc, int ticket)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::SendPhoto() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		SendPhotoTask* task = new SendPhotoTask();
		FileLog("LiveChatClient", "CLiveChatClient::SendPhoto() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId, inviteId, photoId, sendId, charget, photoDesc, ticket);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::SendPhoto() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::SendPhoto() end");
	return result;
}

// 女士发送图片
bool CLiveChatClient::SendLadyPhoto(const string& userId, const string& inviteId, const string& photoId, const string& sendId, bool charget, const string& photoDesc, int ticket)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::SendLadyPhoto() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		SendLadyPhotoTask* task = new SendLadyPhotoTask();
		FileLog("LiveChatClient", "CLiveChatClient::SendLadyPhoto() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId, inviteId, photoId, sendId, charget, photoDesc, ticket);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::SendLadyPhoto() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::SendLadyPhoto() end");
	return result;
}

// 显示图片
bool CLiveChatClient::ShowPhoto(const string& userId, const string& inviteId, const string& photoId, const string& sendId, bool charget, const string& photoDesc, int ticket)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::ShowPhoto() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		ShowPhotoTask* task = new ShowPhotoTask();
		FileLog("LiveChatClient", "CLiveChatClient::ShowPhoto() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId, inviteId, photoId, sendId, charget, photoDesc, ticket);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::ShowPhoto() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::ShowPhoto() end");
	return result;
}

// 获取用户信息
bool CLiveChatClient::GetUserInfo(const string& userId)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::GetUserInfo() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		GetUserInfoTask* task = new GetUserInfoTask();
		FileLog("LiveChatClient", "CLiveChatClient::GetUserInfo() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::GetUserInfo() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::GetUserInfo() end");
	return result;
}

// 获取多个用户信息
int CLiveChatClient::GetUsersInfo(const list<string>& userIdList)
{
	int seq = m_seqCounter.GetInvalidValue();

	FileLog("LiveChatClient", "CLiveChatClient::GetUsersInfo() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		GetUsersInfoTask* task = new GetUsersInfoTask();
		FileLog("LiveChatClient", "CLiveChatClient::GetUsersInfo() task:%p", task);
		if (NULL != task) {
			bool result = task->Init(m_listener);
			result = result && task->InitParam(userIdList);

			if (result) {
				seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::GetUsersInfo() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::GetUsersInfo() end");

	return seq;
}

// 获取联系人/黑名单列表
bool CLiveChatClient::GetContactList(CONTACT_LIST_TYPE listType)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::GetContactList() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		GetContactListTask* task = new GetContactListTask();
		FileLog("LiveChatClient", "CLiveChatClient::GetContactList() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(listType);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::GetContactList() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::GetContactList() end");
	return result;
}

// 上传客户端版本号
bool CLiveChatClient::UploadVer(const string& ver)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::UploadVer() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		UploadVerTask* task = new UploadVerTask();
		FileLog("LiveChatClient", "CLiveChatClient::UploadVer() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(ver);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::UploadVer() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::UploadVer() end");
	return result;
}

// 获取被屏蔽女士列表
bool CLiveChatClient::GetBlockUsers()
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::GetBlockUsers() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		GetBlockUsersTask* task = new GetBlockUsersTask();
		FileLog("LiveChatClient", "CLiveChatClient::GetBlockUsers() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::GetBlockUsers() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::GetBlockUsers() end");
	return result;
}

// 获取最近人列表
bool CLiveChatClient::GetRecentContactList()
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::GetRecentContactList() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		GetRecentContactListTask* task = new GetRecentContactListTask();
		FileLog("LiveChatClient", "CLiveChatClient::GetRecentContactList() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::GetRecentContactList() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::GetRecentContactList() end");
	return result;
}

// 搜索在线男士
bool CLiveChatClient::SearchOnlineMan(int beginAge, int endAge)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::SearchOnlineMan() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		SearchOnlineManTask* task = new SearchOnlineManTask();
		FileLog("LiveChatClient", "CLiveChatClient::SearchOnlineMan() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(beginAge, endAge);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::SearchOnlineMan() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::SearchOnlineMan() end");
	return result;
}

// 回复验证码
bool CLiveChatClient::ReplyIdentifyCode(string identifyCode)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::ReplyIdentifyCode() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		ReplyIdentifyCodeTask* task = new ReplyIdentifyCodeTask();
		FileLog("LiveChatClient", "CLiveChatClient::ReplyIdentifyCode() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(identifyCode);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::ReplyIdentifyCode() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::ReplyIdentifyCode() end");
	return result;
}

// 刷新验证码
bool CLiveChatClient::RefreshIdentifyCode()
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::RefreshIdentifyCode() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		RefreshIdentifyCodeTask* task = new RefreshIdentifyCodeTask();
		FileLog("LiveChatClient", "CLiveChatClient::RefreshIdentifyCode() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::RefreshIdentifyCode() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::RefreshIdentifyCode() end");
	return result;
}

// 刷新邀请模板
bool CLiveChatClient::RefreshInviteTemplate()
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::RefreshInviteTemplate() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		RefreshInviteTemplateTask* task = new RefreshInviteTemplateTask();
		FileLog("LiveChatClient", "CLiveChatClient::RefreshInviteTemplate() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::RefreshInviteTemplate() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::RefreshInviteTemplate() end");
	return result;
}

// 获取已扣费最近联系人列表
bool CLiveChatClient::GetFeeRecentContactList()
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::GetFeeRecentContactList() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		GetFeeRecentContactListTask* task = new GetFeeRecentContactListTask();
		FileLog("LiveChatClient", "CLiveChatClient::GetFeeRecentContactList() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::GetFeeRecentContactList() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::GetFeeRecentContactList() end");
	return result;
}

// 获取女士聊天信息（包括在聊及邀请的男士列表等）
bool CLiveChatClient::GetLadyChatInfo()
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::GetLadyChatInfo() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		GetLadyChatInfoTask* task = new GetLadyChatInfoTask();
		FileLog("LiveChatClient", "CLiveChatClient::GetLadyChatInfo() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::GetLadyChatInfo() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::GetLadyChatInfo() end");
	return result;
}

// 播放视频
bool CLiveChatClient::PlayVideo(const string& userId, const string& inviteId, const string& videoId, const string& sendId, bool charget, const string& videoDesc, int ticket)
{
    bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::PlayVideo() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		PlayVideoTask* task = new PlayVideoTask();
		FileLog("LiveChatClient", "CLiveChatClient::PlayVideo() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId, inviteId, videoId, sendId, charget, videoDesc, ticket);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::PlayVideo() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::PlayVideo() end");
	return result;
}

// 女士发送微视频
bool CLiveChatClient::SendLadyVideo(const string& userId, const string& inviteId, const string& videoId, const string& sendId, bool charge, const string& videoDesc, int ticket)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::SendLadyVideo() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		SendLadyVideoTask* task = new SendLadyVideoTask();
		FileLog("LiveChatClient", "CLiveChatClient::SendLadyVideo() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId, inviteId, videoId, sendId, charge, videoDesc, ticket);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::SendLadyVideo() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::SendLadyVideo() end");
	return result;
}

// 获取女士择偶条件
bool CLiveChatClient::GetLadyCondition(const string& userId)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::GetLadyCondition() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		GetLadyConditionTask* task = new GetLadyConditionTask();
		FileLog("LiveChatClient", "CLiveChatClient::GetLadyCondition() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::GetLadyCondition() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::GetLadyCondition() end");
	return result;
}

// 获取女士自定义邀请模板
bool CLiveChatClient::GetLadyCustomTemplate(const string& userId)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::GetLadyCustomTemplate() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		GetLadyCustomTemplateTask* task = new GetLadyCustomTemplateTask();
		FileLog("LiveChatClient", "CLiveChatClient::GetLadyCustomTemplate() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::GetLadyCustomTemplate() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::GetLadyCustomTemplate() end");
	return result;
}

// 弹出女士自动邀请消息通知
bool CLiveChatClient::UploadPopLadyAutoInvite(const string& userId, const string& msg, const string& key)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::UploadPopLadyAutoInvite() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		UploadPopLadyAutoInviteTask* task = new UploadPopLadyAutoInviteTask();
		FileLog("LiveChatClient", "CLiveChatClient::UploadPopLadyAutoInvite() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId, msg, key);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::UploadPopLadyAutoInvite() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::UploadPopLadyAutoInvite() end");
	return result;
}

// 上传自动充值状态
bool CLiveChatClient::UploadAutoChargeStatus(bool isCharge)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::UploadAutoChargeStatus() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		UploadAutoChargeStatusTask* task = new UploadAutoChargeStatusTask();
		FileLog("LiveChatClient", "CLiveChatClient::UploadAutoChargeStatus() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(isCharge);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::UploadAutoChargeStatus() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::UploadAutoChargeStatus() end");
	return result;
}

// 发送小高级表情
bool CLiveChatClient::SendMagicIcon(const string& userId, const string& iconId, int ticket)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::SendMagicIcon() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		SendMagicIconTask* task = new SendMagicIconTask();
		FileLog("LiveChatClient", "CLiveChatClient::SendMagicIcon() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId, iconId, ticket);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::SendMagicIcon() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::SendMagicIcon() end");
	return result;
}

// 获取指定男/女士的已购主题包
bool CLiveChatClient::GetPaidTheme(const string& userId)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::GetPaidTheme() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		GetPaidThemeTask* task = new GetPaidThemeTask();
		FileLog("LiveChatClient", "CLiveChatClient::GetPaidTheme() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::GetPaidTheme() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::GetPaidTheme() end");
	return result;
}

// 获取男/女士所有已购主题包
bool CLiveChatClient::GetAllPaidTheme()
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::GetAllPaidTheme() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		GetAllPaidThemeTask* task = new GetAllPaidThemeTask();
		FileLog("LiveChatClient", "CLiveChatClient::GetAllPaidTheme() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::GetAllPaidTheme() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::GetAllPaidTheme() end");
	return result;
}

// 上传主题包列表版本号
bool CLiveChatClient::UploadThemeListVer(int themeVer)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::UploadThemeListVer() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		UploadThemeListVerTask* task = new UploadThemeListVerTask();
		FileLog("LiveChatClient", "CLiveChatClient::UploadThemeListVer() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(themeVer);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::UploadThemeListVer() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::UploadThemeListVer() end");
	return result;
}

// 男士购买主题包
bool CLiveChatClient::ManFeeTheme(const string& userId, const string& themeId)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::ManFeeTheme() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		ManFeeThemeTask* task = new ManFeeThemeTask();
		FileLog("LiveChatClient", "CLiveChatClient::ManFeeTheme() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId, themeId);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::ManFeeTheme() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::ManFeeTheme() end");
	return result;
}

// 男士应用主题包
bool CLiveChatClient::ManApplyTheme(const string& userId, const string& themeId)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::ManApplyTheme() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		ManApplyThemeTask* task = new ManApplyThemeTask();
		FileLog("LiveChatClient", "CLiveChatClient::ManApplyTheme() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId, themeId);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::ManApplyTheme() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::ManApplyTheme() end");
	return result;
}

// 男/女士播放主题包动画
bool CLiveChatClient::PlayThemeMotion(const string& userId, const string& themeId)
{
	bool result = false;
	FileLog("LiveChatClient", "CLiveChatClient::PlayThemeMotion() begin");
	if (NULL != m_taskManager
		&& m_taskManager->IsStart())
	{
		PlayThemeMotionTask* task = new PlayThemeMotionTask();
		FileLog("LiveChatClient", "CLiveChatClient::PlayThemeMotion() task:%p", task);
		if (NULL != task) {
			result = task->Init(m_listener);
			result = result && task->InitParam(userId, themeId);

			if (result) {
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				result = m_taskManager->HandleRequestTask(task);
			}
		}
		FileLog("LiveChatClient", "CLiveChatClient::PlayThemeMotion() task:%p end", task);
	}
	FileLog("LiveChatClient", "CLiveChatClient::PlayThemeMotion() end");
	return result;
}

// 获取用户账号
string CLiveChatClient::GetUser()
{
	return m_user;
}

// ------------------------ ITaskManagerListener接口函数 -------------------------
// 连接成功回调
void CLiveChatClient::OnConnect(bool success)
{
	FileLog("LiveChatClient", "CLiveChatClient::OnConnect() success: %d", success);
	if (success) {
		FileLog("LiveChatClient", "CLiveChatClient::OnConnect() CheckVersionProc()");
		// 连接服务器成功，检测版本号
		CheckVersionProc();
		// 启动发送心跳包线程
		HearbeatThreadStart();
	}
	else {
		FileLog("LiveChatClient", "CLiveChatClient::OnConnect() LCC_ERR_CONNECTFAIL, m_listener:%p", m_listener);
		m_listener->OnLogin(LCC_ERR_CONNECTFAIL, "");
	}
	FileLog("LiveChatClient", "CLiveChatClient::OnConnect() end");
}

// 连接失败回调(listUnsentTask：未发送的task列表)
void CLiveChatClient::OnDisconnect(const TaskList& listUnsentTask)
{
	TaskList::const_iterator iter;
	for (iter = listUnsentTask.begin();
		iter != listUnsentTask.end();
		iter++)
	{
		(*iter)->OnDisconnect();
	}

	// 停止心跳线程
	if (NULL != m_hearbeatThread) {
		m_isHearbeatThreadRun = false;
		m_hearbeatThread->WaitAndStop();
		IThreadHandler::ReleaseThreadHandler(m_hearbeatThread);
		m_hearbeatThread = NULL;
	}

	m_listener->OnLogout(LCC_ERR_CONNECTFAIL, "");
}

// 已完成交互的task
void CLiveChatClient::OnTaskDone(ITask* task)
{
	if (NULL != task) {
		// 需要LiveChatClient处理后续相关业务逻辑的task（如：检测版本）
		switch (task->GetCmdCode()) {
		case TCMD_CHECKVER:
			OnCheckVerTaskDone(task);
			break;
		case TCMD_LOGIN:
			UploadDeviceIdProc();
			break;
		case TCMD_UPLOADDEVID:
			UploadDeviceTypeProc();
			break;
		}
	}
}

// 检测版本已经完成
void CLiveChatClient::OnCheckVerTaskDone(ITask* task)
{
	LCC_ERR_TYPE errType = LCC_ERR_FAIL;
	string errMsg("");
	task->GetHandleResult(errType, errMsg);
	if (LCC_ERR_SUCCESS == errType) {
		// 检测版本成功，进行登录操作
		LoginProc();
	}
	else {
		// 检测版本失败，回调给上层
		m_listener->OnLogin(errType, errMsg);
	}
}

// ------------------------ 操作处理函数 ------------------------------
// 检测版本号
bool CLiveChatClient::CheckVersionProc()
{
	bool result = false;
	CheckVerTask* checkVerTask = new CheckVerTask();
	if (NULL != checkVerTask) {
		checkVerTask->Init(m_listener);
		checkVerTask->InitParam("1.1.0.0XCHAT");

		int seq = m_seqCounter.GetAndIncrement();
		checkVerTask->SetSeq(seq);
		result = m_taskManager->HandleRequestTask(checkVerTask);
	}
	return result;
}

// 登录
bool CLiveChatClient::LoginProc()
{
	bool result = false;
	LoginTask* loginTask = new LoginTask();
	if (NULL != loginTask) {
		loginTask->Init(m_listener);
		loginTask->InitParam(m_user, m_password, m_clientType, m_sexType, m_authType);

		int seq = m_seqCounter.GetAndIncrement();
		loginTask->SetSeq(seq);
		result =  m_taskManager->HandleRequestTask(loginTask);
	}
	return result;
}

// 上传设备ID
bool CLiveChatClient::UploadDeviceIdProc()
{
	bool result = false;
	UploadDeviceIdTask* task = new UploadDeviceIdTask();
	if (NULL != task) {
		task->Init(m_listener);
		task->InitParam(m_deviceId);

		int seq = m_seqCounter.GetAndIncrement();
		task->SetSeq(seq);
		result =  m_taskManager->HandleRequestTask(task);
	}
	return result;
}

// 上传设备类型
bool CLiveChatClient::UploadDeviceTypeProc()
{
	bool result = false;
	UploadDeviceTypeTask* task = new UploadDeviceTypeTask();
	if (NULL != task) {
		task->Init(m_listener);
		task->InitParam(m_clientType);

		int seq = m_seqCounter.GetAndIncrement();
		task->SetSeq(seq);
		result =  m_taskManager->HandleRequestTask(task);
	}
	return result;
}

// ------------------------ 心跳处理函数 ------------------------------
void CLiveChatClient::HearbeatThreadStart()
{
	// 启动心跳处理线程
	m_isHearbeatThreadRun = true;
	if (NULL == m_hearbeatThread) {
		m_hearbeatThread = IThreadHandler::CreateThreadHandler();
		m_hearbeatThread->Start(HearbeatThread, this);
	}
}

TH_RETURN_PARAM CLiveChatClient::HearbeatThread(void* arg)
{
	CLiveChatClient* pThis = (CLiveChatClient*)arg;
	pThis->HearbeatProc();
	return NULL;
}

void CLiveChatClient::HearbeatProc()
{
	FileLog("LiveChatClient", "CLiveChatClient::HearbeatProc() begin");

	const unsigned long nSleepStep = 200;	// ms
	const unsigned long nSendStep = 30 * 1000; // ms

	long long preTime = getCurrentTime();
	long long curTime = getCurrentTime();
	do {
		curTime = getCurrentTime();
		if (DiffTime(preTime, curTime) >= nSendStep) {
			HearbeatTask* task = new HearbeatTask();
			if (NULL != task) {
				task->Init(m_listener);
				int seq = m_seqCounter.GetAndIncrement();
				task->SetSeq(seq);
				m_taskManager->HandleRequestTask(task);
			}
			preTime = curTime;
		}
		Sleep(nSleepStep);
	} while (m_isHearbeatThreadRun);

	FileLog("LiveChatClient", "CLiveChatClient::HearbeatProc() end");
}
