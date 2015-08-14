/*
 * author: Samson.Fan
 *   date: 2015-03-25
 *   file: LoginTask.cpp
 *   desc: 登录Task实现类
 */

#include "LoginTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>
#include <common/KLog.h>

// 请求参数定义
#define AUTHTYPE_PARAM		"authType"	// 认证密码类型
#define AUTHTYPE_PARAM_SID		0			// password为sid
#define AUTHTYPE_PARAM_PWD		1			// password为密码
#define FROMID_PARAM		"fromId"	// 客户端类型
#define FROMID_PARAM_ANDROID	5			// Android客户端
#define SEX_PARAM			"sex"		// 性别
#define SEX_PARAM_FEMALE		0			// 女士
#define SEX_PARAM_MALE			1			// 男士
#define TYPE_PARAM			"type"		// 用户类型
#define TYPE_PARAM_MEMBER		0			// 普通用户
#define TYPE_PARAM_INTER		1			// 翻译
#define USERID_PARAM		"userId"	// 帐号ID
#define PASSWORD_PARAM		"password"	// 密码（根据 authType 决定）

LoginTask::LoginTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_user = "";
	m_password = "";
	m_clientType = CLIENT_ANDROID;
	m_sexType = USER_SEX_MALE;
}

LoginTask::~LoginTask(void)
{
}

// 初始化
bool LoginTask::Init(ILiveChatClientListener* listener)
{
	bool result = false;
	if (NULL != listener)
	{
		m_listener = listener;
		result = true;
	}
	return result;
}
	
// 处理已接收数据
bool LoginTask::Handle(const TransportProtocol* tp)
{
	bool result = false;

	FileLog("LiveChatClient", "LoginTask::Handle() begin, tp->data:%p, tp->dataLen:%d, tp->data[0]:%d", tp->data, tp->GetDataLength(), tp->data[0]);
		
	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if (!root.isnull()) {
		// 解析成功协议
		if (root->type == DT_TRUE) {
			m_errType = LCC_ERR_SUCCESS;
			m_errMsg = "";
			result = true;
		}

		// 解析失败协议
		if (!result) {
			int errType = 0;
			string errMsg = "";
			if (GetAMFProtocolError(root, errType, errMsg)) {
				m_errType = (LCC_ERR_TYPE)errType;
				m_errMsg = errMsg;
				result = true;
			}
		}
	}

	// 协议解析失败
	if (!result) {
		m_errType = LCC_ERR_PROTOCOLFAIL;
		m_errMsg = "";
	}

	FileLog("LiveChatClient", "LoginTask::Handle() m_errType:%d", m_errType);

	// 通知listener
	if (NULL != m_listener) {
		m_listener->OnLogin(m_errType, m_errMsg);
		FileLog("LiveChatClient", "LoginTask::Handle() callback end, result:%d", result);
	}
	
	FileLog("LiveChatClient", "LoginTask::Handle() end");

	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool LoginTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	
	FileLog("LiveChatClient", "LoginTask::GetSendData() begin");

	// 构造json协议
	Json::Value root;
	root[AUTHTYPE_PARAM] = AUTHTYPE_PARAM_SID;
	//root[FROMID_PARAM] = m_clientType==CLIENT_ANDROID ? FROMID_PARAM_ANDROID;
	root[FROMID_PARAM] = m_clientType;
	root[SEX_PARAM] = m_sexType;
	root[TYPE_PARAM] = TYPE_PARAM_MEMBER;
	root[USERID_PARAM] = m_user;
	root[PASSWORD_PARAM] = m_password;
	Json::FastWriter writer;
	string json = writer.write(root);

	// 填入buffer
	if (json.length() < dataSize) {
		memcpy(data, json.c_str(), json.length());
		dataLen = json.length();

		result  = true;
	}

	FileLog("LiveChatClient", "LoginTask::GetSendData() end, result:%d", result);

	return result;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE LoginTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int LoginTask::GetCmdCode()
{
	return TCMD_LOGIN;	
}

// 设置seq
void LoginTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int LoginTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool LoginTask::IsWaitToRespond()
{
	return true;
}

// 获取处理结果
void LoginTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 初始化参数
bool LoginTask::InitParam(const string& user, const string& password, CLIENT_TYPE clientType, USER_SEX_TYPE sex)
{
	bool result = false;
	if (!user.empty() 
		&& !password.empty()) 
	{
		m_user = user;
		m_password = password;
		m_clientType = clientType;
		m_sexType = sex;

		result = true;
	}
	
	return result;
}

// 未完成任务的断线通知
void LoginTask::OnDisconnect()
{
	if (NULL != m_listener) {
		m_listener->OnLogin(LCC_ERR_CONNECTFAIL, "");
	}
}
