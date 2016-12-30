/*
 * author: Alex shum
 *   date: 2016-10-28
 *   file: RecvLadyCamStatusTask.cpp
 *   desc: 女士Cam状态改变通知（仅男士端） Task实现类
 */

#include "RecvLadyCamStatusTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>
#include <common/CheckMemoryLeak.h>
#include <common/CommonFunc.h>

// 返回参数定义
// 女士ID
#define USERID_PARAM	  "userId"
// 状态类型（0：下线 1：上线 2：隐藏 3：绑定 4：解开 5：视频开放 6：视频关闭 7：Cam打开 8：Cam关闭）
#define STATUSID_PARAM	  "statusId"
// 服务器名称
#define SERVER_PARAM      "server"
// 客服端类型（整形）
#define CLIENTTYPE_PARAM  "clientType"
// 声音开关（0：开 1：关）（整形）
#define SOUND_PARAM       "sound"
// 客户端版本号
#define VERSION_PARAM     "version"

RecvLadyCamStatusTask::RecvLadyCamStatusTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	
	m_userId = "";
	m_statusId = USTATUSPRO_CAMCLOSE;
	m_server = "";
	m_clientType = CLIENT_ANDROID;
	m_sound = 0;
	m_version = "";

}

RecvLadyCamStatusTask::~RecvLadyCamStatusTask(void)
{
}

// 初始化
bool RecvLadyCamStatusTask::Init(ILiveChatClientListener* listener)
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
bool RecvLadyCamStatusTask::Handle(const TransportProtocol* tp)
{
	bool result = false;
		
	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if (!root.isnull())
	{
		if(root->type == DT_OBJECT)
		{

			// userId
			amf_object_handle userIdObject = root->get_child(USERID_PARAM);
			result = !userIdObject.isnull() && userIdObject->type == DT_STRING;
			if (result) {
				m_userId = userIdObject->strValue;
			}

			// statusId
			amf_object_handle statusIdObject = root->get_child(STATUSID_PARAM);
			result = !statusIdObject.isnull() && statusIdObject->type == DT_INTEGER;
			if (result) {
				m_statusId = GetUserStatusProtocol(statusIdObject->intValue);
			}

			// server
			amf_object_handle serverObject = root->get_child(SERVER_PARAM);
			result = !serverObject.isnull() && serverObject->type == DT_STRING;
			if (result) {
				m_server = serverObject->strValue;
			}

			// clientType
			amf_object_handle clientTypeObject = root->get_child(CLIENTTYPE_PARAM);
			result = !clientTypeObject.isnull() && clientTypeObject->type == DT_INTEGER;
			if (result) {
				m_clientType = (CLIENT_TYPE)clientTypeObject->intValue;
			}

			// sound
			amf_object_handle soundObject = root->get_child(SOUND_PARAM);
			result = !soundObject.isnull() && soundObject->type == DT_INTEGER;
			if (result) {
				m_sound = soundObject->intValue;
			}

			// version
			amf_object_handle versionObject = root->get_child(VERSION_PARAM);
			result = !versionObject.isnull() && versionObject->type == DT_STRING;
			if (result) {
				m_version = versionObject->strValue;
			}
		}

			if (!result){
				int errType = 0;
				string errMsg = "";
				if (GetAMFProtocolError(root, errType, errMsg)) {
					m_errType = (LCC_ERR_TYPE)errType;
					m_errMsg = errMsg;
					// 解析成功
					result = true;
				}
			}
	}
	// 通知listener
	if (NULL != m_listener 
		&& result) 
	{
		m_listener->OnRecvLadyCamStatus(m_userId, m_statusId, m_server, m_clientType, m_sound, m_version);
	}
	
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool RecvLadyCamStatusTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	// 本协议没有返回
	return result;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE RecvLadyCamStatusTask::GetSendDataProtocolType()
{
	return AMF_PROTOCOL;
}
	
// 获取命令号
int RecvLadyCamStatusTask::GetCmdCode()
{
	return TCMD_RECVLADYCAMSTATUS;
}

// 设置seq
void RecvLadyCamStatusTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int RecvLadyCamStatusTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool RecvLadyCamStatusTask::IsWaitToRespond()
{
	return false;
}

// 获取处理结果
void RecvLadyCamStatusTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 未完成任务的断线通知
void RecvLadyCamStatusTask::OnDisconnect()
{
	// 不用回调
}
