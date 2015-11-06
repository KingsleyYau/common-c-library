/*
 * author: Samson.Fan
 *   date: 2015-03-25
 *   file: GetUserInfoTask.cpp
 *   desc: 获取会话信息Task实现类
 */

#include "GetUserInfoTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include "CommonParsing.h"
#include <json/json.h>



GetUserInfoTask::GetUserInfoTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_userId = "";
}

GetUserInfoTask::~GetUserInfoTask(void)
{
}

// 初始化
bool GetUserInfoTask::Init(ILiveChatClientListener* listener)
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
bool GetUserInfoTask::Handle(const TransportProtocol* tp)
{
	bool result = false;

	// callback 参数
	UserInfoItem item;
		
	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if (!root.isnull()) {
		// 解析成功协议
		if (root->type == DT_OBJECT) {
			result = ParsingUserInfoItem(root, item);
		}

		if (!result) {
			// 解析失败协议
			int errType = 0;
			string errMsg = "";
			if (GetAMFProtocolError(root, errType, errMsg)) {
				m_errType = (LCC_ERR_TYPE)errType;
				m_errMsg = errMsg;
				result = true;
			}
		}
		else {
			m_errType = LCC_ERR_SUCCESS;
			string errMsg = "";
		}
	}

	// 协议解析失败
	if (!result) {
		m_errType = LCC_ERR_PROTOCOLFAIL;
		m_errMsg = "";
	}

	// 通知listener
	if (NULL != m_listener) {
		m_listener->OnGetUserInfo(m_userId, m_errType, m_errMsg, item);
	}
	
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool GetUserInfoTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;

	// 构造json协议
	Json::Value root(m_userId);
	Json::FastWriter writer;
	string json = writer.write(root);

	// 填入buffer
	if (json.length() < dataSize) {
		memcpy(data, json.c_str(), json.length());
		dataLen = json.length();

		result  = true;
	}
	return result;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE GetUserInfoTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int GetUserInfoTask::GetCmdCode()
{
	return TCMD_GETUSERINFO;
}

// 设置seq
void GetUserInfoTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int GetUserInfoTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool GetUserInfoTask::IsWaitToRespond()
{
	return true;
}

// 获取处理结果
void GetUserInfoTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 初始化参数
bool GetUserInfoTask::InitParam(const string& userId)
{
	bool result = false;
	if (!userId.empty()) {
		m_userId = userId;

		result = true;
	}
	
	return result;
}

// 未完成任务的断线通知
void GetUserInfoTask::OnDisconnect()
{
	if (NULL != m_listener) {
		UserInfoItem item;
		m_listener->OnGetUserInfo(m_userId, LCC_ERR_CONNECTFAIL, "", item);
	}
}
