/*
 * author: Samson.Fan
 *   date: 2015-06-16
 *   file: GetContactListTask.cpp
 *   desc: 获取联系人或黑名单列表Task实现类
 */

#include "GetContactListTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include "CommonParsing.h"
#include <json/json.h>
#include <KLog.h>

GetContactListTask::GetContactListTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_listType = CONTACT_LIST_BLOCK;
}

GetContactListTask::~GetContactListTask(void)
{
}

// 初始化
bool GetContactListTask::Init(ILiveChatClientListener* listener)
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
bool GetContactListTask::Handle(const TransportProtocol* tp)
{
	// 服务器有可能返回空，因此默认为成功
	bool result = true;
	m_errType = LCC_ERR_SUCCESS;
	m_errMsg = "";


	TalkUserList list;
		
	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if (!root.isnull()) {
		// 解析成功协议
		if (root->type == DT_ARRAY) {
			size_t i = 0;
			for (i = 0; i < root->childrens.size(); i++)
			{
				TalkUserListItem item;
				if (ParsingUserInfoItem(root->childrens[i], item)) {
					list.push_back(item);
				}
			}
			result = true;
		}
		else {
			result = false;
		}

		// 解析失败协议
		int errType = 0;
		string errMsg = "";
		if (GetAMFProtocolError(root, errType, errMsg)) {
			m_errType = (LCC_ERR_TYPE)errType;
			m_errMsg = errMsg;
		}
		else {
			result = false;
		}
	}

	FileLog("LiveChatClient", "GetContactListTask::Handle() result:%d, list.size:%d", result, list.size());

	// 通知listener
	if (NULL != m_listener) {
		m_listener->OnGetContactList(m_listType, m_errType, m_errMsg, list);
	}
	
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool GetContactListTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;

	// 构造json协议
	Json::Value root(m_listType);
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
TASK_PROTOCOL_TYPE GetContactListTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int GetContactListTask::GetCmdCode()
{
	FileLog("LiveChatClient", "GetContactListTask::GetCmdCode(), TCMD_GETCONTACTLIST:%d", TCMD_GETCONTACTLIST);
	return TCMD_GETCONTACTLIST;
}

// 设置seq
void GetContactListTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int GetContactListTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool GetContactListTask::IsWaitToRespond()
{
	return true;
}

// 获取处理结果
void GetContactListTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 初始化参数
bool GetContactListTask::InitParam(CONTACT_LIST_TYPE listType)
{
	bool result = false;
	m_listType = listType;
	result = true;
	
	return result;
}

// 未完成任务的断线通知
void GetContactListTask::OnDisconnect()
{
	if (NULL != m_listener) {
		TalkUserList tmp;
		m_listener->OnGetContactList(m_listType, LCC_ERR_CONNECTFAIL, "", tmp);
	}
}
