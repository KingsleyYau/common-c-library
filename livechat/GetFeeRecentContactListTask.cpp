/*
 * author: Samson.Fan
 *   date: 2015-08-03
 *   file: GetFeeRecentContactListTask.cpp
 *   desc: 获取已扣费最近联系人列表Task实现类
 */

#include "GetFeeRecentContactListTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include "CommonParsing.h"
#include <json/json/json.h>
#include <common/KLog.h>
#include <common/CheckMemoryLeak.h>

GetFeeRecentContactListTask::GetFeeRecentContactListTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";
}

GetFeeRecentContactListTask::~GetFeeRecentContactListTask(void)
{
}

// 初始化
bool GetFeeRecentContactListTask::Init(ILiveChatClientListener* listener)
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
bool GetFeeRecentContactListTask::Handle(const TransportProtocol* tp)
{
	// 服务器有可能返回空，因此默认为成功
	bool result = true;
	m_errType = LCC_ERR_SUCCESS;
	m_errMsg = "";

	list<string> userList;

	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if (!root.isnull()) {
		// 解析成功协议
		if (root->type == DT_ARRAY) {
			int i = 0;
			for (i = 0; i < root->childrens.size(); i++)
			{
				if (root->childrens[i]->type == DT_STRING)
				{
					string userId = root->childrens[i]->strValue;
					userList.push_back(userId);
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

	FileLog("LiveChatClient", "GetFeeRecentContactListTask::Handle() result:%d, userList.size:%d", result, userList.size());

	// 通知listener
	if (NULL != m_listener) {
		m_listener->OnGetFeeRecentContactList(m_errType, m_errMsg, userList);
	}

	return result;
}

// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool GetFeeRecentContactListTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
    // 没有请求参数
    dataLen = 0;
	return true;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE GetFeeRecentContactListTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}

// 获取命令号
int GetFeeRecentContactListTask::GetCmdCode()
{
	return TCMD_GETFEERECENTCONTACTLIST;
}

// 设置seq
void GetFeeRecentContactListTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int GetFeeRecentContactListTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool GetFeeRecentContactListTask::IsWaitToRespond()
{
	return true;
}

// 获取处理结果
void GetFeeRecentContactListTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 未完成任务的断线通知
void GetFeeRecentContactListTask::OnDisconnect()
{
	if (NULL != m_listener) {
		list<string> tmp;
		m_listener->OnGetFeeRecentContactList(LCC_ERR_CONNECTFAIL, "", tmp);
	}
}
