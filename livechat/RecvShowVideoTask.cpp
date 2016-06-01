/*
 * author: Samson.Fan
 *   date: 2016-01-06
 *   file: RecvShowVideoTask.cpp
 *   desc: 接收微视频被查看Task实现类
 */

#include "RecvShowVideoTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>
#include "SendLadyVideoTask.h"
#include <common/KLog.h>
#include <common/CheckMemoryLeak.h>

// 请求参数定义
#define TARGETID_PARAM		"targetId"		// 接收用户Id
#define FROMID_PARAM		"fromId"		// 发送用户Id
#define FROMNAME_PARAM		"fromUserName"	// 发送用户名称
#define INVITEID_PARAM		"inviteId"		// 邀请Id
#define VIDEO_PARAM			"video"			// 微视频信息
#define TICKET_PARAM		"ticket"		// 票根

RecvShowVideoTask::RecvShowVideoTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_toId = "";
	m_fromId = "";
	m_fromName = "";
	m_inviteId = "";
	m_videoId = "";
	m_videoDesc = "";
	m_sendId = "";
	m_charget = false;
	m_ticket = 0;
}

RecvShowVideoTask::~RecvShowVideoTask(void)
{
}

// 初始化
bool RecvShowVideoTask::Init(ILiveChatClientListener* listener)
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
bool RecvShowVideoTask::Handle(const TransportProtocol* tp)
{
	bool result = false;

	FileLog("LiveChatClient", "RecvShowVideoTask::Handle() begin, tp:%p", tp);
		
	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if (!root.isnull()
		&& root->type == DT_OBJECT)
	{
		// targetId
		amf_object_handle targetIdObject = root->get_child(TARGETID_PARAM);
		result = !targetIdObject.isnull() && targetIdObject->type == DT_STRING;
		if (result) {
			m_toId = targetIdObject->strValue;
		}

		// fromId
		amf_object_handle fromIdObject = root->get_child(FROMID_PARAM);
		result = !fromIdObject.isnull() && fromIdObject->type == DT_STRING;
		if (result) {
			m_fromId = fromIdObject->strValue;
		}

		// fromName
		amf_object_handle fromNameObject = root->get_child(FROMNAME_PARAM);
		result = !fromNameObject.isnull() && fromNameObject->type == DT_STRING;
		if (result) {
			m_fromName = fromNameObject->strValue;
		}

		// inviteId
		amf_object_handle inviteIdObject = root->get_child(INVITEID_PARAM);
		result = !inviteIdObject.isnull() && inviteIdObject->type == DT_STRING;
		if (result) {
			m_inviteId = inviteIdObject->strValue;
		}

		// ticket
		amf_object_handle ticketObject = root->get_child(TICKET_PARAM);
		result = !ticketObject.isnull() && ticketObject->type == DT_INTEGER;
		if (result) {
			m_ticket = ticketObject->intValue;
		}

		// video
		amf_object_handle videoObject = root->get_child(VIDEO_PARAM);
		result = !videoObject.isnull() && videoObject->type == DT_STRING;
		if (result) {
			SendLadyVideoTask::ParsingVideoValue(videoObject->strValue, m_videoId, m_sendId, m_charget, m_videoDesc);
		}
	}

	FileLog("LiveChatClient", "RecvShowVideoTask::Handle() parsing ok, tp:%p", tp);

	// 通知listener
	if (NULL != m_listener 
		&& result) 
	{
		FileLog("LiveChatClient", "RecvShowVideoTask::Handle() callback begin, tp:%p", tp);
		m_listener->OnRecvShowVideo(m_toId, m_fromId, m_fromName, m_inviteId, m_videoId, m_sendId, m_charget, m_videoDesc, m_ticket);
		FileLog("LiveChatClient", "RecvShowVideoTask::Handle() callback end, tp:%p", tp);
	}
	FileLog("LiveChatClient", "RecvShowVideoTask::Handle() end, tp:%p", tp);
	
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool RecvShowVideoTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	// 本协议没有返回
	return result;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE RecvShowVideoTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int RecvShowVideoTask::GetCmdCode()
{
	return TCMD_RECVSHOWVIDEO;
}

// 设置seq
void RecvShowVideoTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int RecvShowVideoTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool RecvShowVideoTask::IsWaitToRespond()
{
	return false;
}

// 获取处理结果
void RecvShowVideoTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 未完成任务的断线通知
void RecvShowVideoTask::OnDisconnect()
{
	// 不用回调
}
