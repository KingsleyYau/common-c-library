/*
 * author: Samson.Fan
 *   date: 2015-08-13
 *   file: RecvVideoTask.cpp
 *   desc: 接收图片消息Task实现类
 */

#include "RecvVideoTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json.h>
#include <KLog.h>
#include <Arithmetic.h>

// 请求参数定义
#define TARGETID_PARAM		"targetId"		// 接收用户Id
#define FROMID_PARAM		"fromId"		// 发送用户Id
#define FROMNAME_PARAM		"fromUserName"	// 发送用户名称
#define INVITEID_PARAM		"inviteId"		// 邀请Id
#define VIDEO_PARAM			"video"			// 视频信息
#define TICKET_PARAM		"ticket"		// 票根

// video参数的分隔符
#define VIDEO_PARAM_DELIMIT	"|||"
// 是否已扣费值定义
#define CHARGET_YES			"1"			// 是
#define CHARGET_NO			"0"			// 否

RecvVideoTask::RecvVideoTask(void)
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

RecvVideoTask::~RecvVideoTask(void)
{
}

// 初始化
bool RecvVideoTask::Init(ILiveChatClientListener* listener)
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
bool RecvVideoTask::Handle(const TransportProtocol* tp)
{
	bool result = false;

	FileLog("LiveChatClient", "RecvVideoTask::Handle() begin, tp:%p", tp);

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
			ParsingVideoValue(videoObject->strValue, m_videoId, m_sendId, m_charget, m_videoDesc);
		}
	}

	FileLog("LiveChatClient", "RecvVideoTask::Handle() parsing ok, tp:%p", tp);

	// 通知listener
	if (NULL != m_listener
		&& result)
	{
		FileLog("LiveChatClient", "RecvVideoTask::Handle() callback begin, tp:%p", tp);
		m_listener->OnRecvVideo(m_toId, m_fromId, m_fromName, m_inviteId, m_videoId, m_sendId, m_charget, m_videoDesc, m_ticket);
		FileLog("LiveChatClient", "RecvVideoTask::Handle() callback end, tp:%p", tp);
	}
	FileLog("LiveChatClient", "RecvVideoTask::Handle() end, tp:%p", tp);

	return result;
}

// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool RecvVideoTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	// 本协议没有返回
	return result;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE RecvVideoTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}

// 获取命令号
int RecvVideoTask::GetCmdCode()
{
	return TCMD_RECVVIDEO;
}

// 设置seq
void RecvVideoTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int RecvVideoTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool RecvVideoTask::IsWaitToRespond()
{
	return false;
}

// 获取处理结果
void RecvVideoTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 未完成任务的断线通知
void RecvVideoTask::OnDisconnect()
{
	// 不用回调
}

// 构建video参数值
string RecvVideoTask::GetVideoValue(const string& videoId, const string& sendId, bool charget, const string& videoDesc)
{
	string video("");

	// [videoid]|||[0/1是否查看(是否已扣费)]|||[base64(描述)]|||[sendId]
	video += videoId;
	video += VIDEO_PARAM_DELIMIT;
	video += charget ? CHARGET_YES : CHARGET_NO;
	video += VIDEO_PARAM_DELIMIT;
	video += Arithmetic::Base64Encode(videoDesc.c_str(), videoDesc.length());
	video += VIDEO_PARAM_DELIMIT;
	video += sendId;

	return video;
}

// 解析video参数
bool RecvVideoTask::ParsingVideoValue(const string& video, string& videoId, string& sendId, bool& charget, string& videoDesc)
{
	bool result = false;
	size_t begin = 0;
	size_t end = 0;
	int i = 0;

	while (string::npos != (end = video.find_first_of(VIDEO_PARAM_DELIMIT, begin)))
	{
		if (i == 0) {
			// videoId
			videoId = video.substr(begin, end - begin);
			begin = end + strlen(VIDEO_PARAM_DELIMIT);
		}
		else if (i == 1) {
			// charget
			string strCharget = video.substr(begin, end - begin);
			charget = (strCharget==CHARGET_YES ? true : false);
			begin = end + strlen(VIDEO_PARAM_DELIMIT);
		}
		else if (i == 2) {
			// videoDesc
			videoDesc = video.substr(begin, end - begin);
			const int bufferSize = 1024;
			char buffer[bufferSize] = {0};
			if (!videoDesc.empty() && videoDesc.length() < bufferSize) {
				Arithmetic::Base64Decode(videoDesc.c_str(), videoDesc.length(), buffer);
				videoDesc = buffer;
			}
			begin = end + strlen(VIDEO_PARAM_DELIMIT);

			// sendId
			sendId = video.substr(begin);
			break;
		}
		i++;
	}

	if (!videoId.empty() && !sendId.empty()) {
		result = true;
	}

	return result;
}
