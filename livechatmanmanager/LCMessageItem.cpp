/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCMessageItem.cpp
 *   desc: LiveChat消息item
 */

#include "LCMessageItem.h"
#include "LCEmotionManager.h"
#include "LCVoiceManager.h"
#include "LCPhotoManager.h"
#include "LCVideoManager.h"
#include "LCUserItem.h"
#include <common/CommonFunc.h>
#include <manrequesthandler/item/Record.h>
#include <common/KLog.h>
#include <common/CheckMemoryLeak.h>

using namespace lcmm;


long LCMessageItem::m_dbTime = 0;

LCMessageItem::LCMessageItem()
{
	m_msgId = 0;
	m_sendType = SendType_Unknow;
	m_fromId = "";
	m_toId = "";
	m_inviteId = "";
	m_createTime = 0;
	m_statusType = StatusType_Unprocess;
	m_msgType = MT_Unknow;

	m_textItem = NULL;
	m_warningItem = NULL;
	m_emotionItem = NULL;
	m_voiceItem = NULL;
	m_photoItem = NULL;
	m_videoItem = NULL;
	m_systemItem = NULL;
	m_customItem = NULL;

	m_userItem = NULL;
}

LCMessageItem::~LCMessageItem()
{
	Clear();
}

// 初始化
bool LCMessageItem::Init(
		int msgId
		, SendType sendType
		, const string& fromId
		, const string& toId
		, const string& inviteId
		, StatusType statusType)
{
	bool result = false;
	if (!fromId.empty()
		&& !toId.empty()
		&& sendType != SendType_Unknow)
	{
		m_msgId = msgId;
		m_sendType = sendType;
		m_fromId = fromId;
		m_toId = toId;
		m_inviteId = inviteId;
		m_statusType = statusType;
		m_procResult.SetSuccess();

		m_createTime = GetCreateTime();
		result = true;
	}
	return result;
}

// 获取生成时间
long LCMessageItem::GetCreateTime()
{
	return (long)(getCurrentTime() / 1000);
}

// 更新生成时间
void LCMessageItem::RenewCreateTime()
{
	m_createTime = GetCreateTime();
}

// 设置服务器当前数据库时间
void LCMessageItem::SetDbTime(long dbTime)
{
	m_dbTime = dbTime;
}

// 把服务器时间转换为手机时间
long LCMessageItem::GetLocalTimeWithServerTime(long serverTime)
{
	long synServerTime = 0;
	if(m_dbTime > 0){
		/*同步服务器时间成功*/
		long diffTime = GetCreateTime() - m_dbTime;
		synServerTime = serverTime + diffTime;
	}else {
		/*同步服务器时间失败，使用默认时间*/
		synServerTime = serverTime;
	}
	return synServerTime;
}

// 使用Record初始化MessageItem
bool LCMessageItem::InitWithRecord(
				int msgId
				, const string& selfId
				, const string& userId
				, const string& inviteId
				, const Record& record
				, LCEmotionManager* emotionMgr
				, LCVoiceManager* voiceMgr
				, LCPhotoManager* photoMgr
				, LCVideoManager* videoMgr)
{
	bool result = false;
	m_msgId = msgId;
	m_toId = (record.toflag == LRT_RECV ? selfId : userId);
	m_fromId = (record.toflag == LRT_RECV ? userId : selfId);
	m_inviteId = inviteId;
	m_sendType = (record.toflag == LRT_RECV ? SendType_Recv : SendType_Send);
	m_statusType = StatusType_Finish;
	m_createTime = GetLocalTimeWithServerTime(record.adddate);

	switch(record.messageType) {
	case LRM_TEXT: {
		if (!record.textMsg.empty())
		{
			LCTextItem* textItem = new LCTextItem;
			textItem->Init(record.textMsg, record.toflag != LRT_RECV);
			SetTextItem(textItem);
			result = true;
		}
	}break;
	case LRM_INVITE: {
		if (!record.inviteMsg.empty())
		{
			LCTextItem* textItem = new LCTextItem;
			textItem->Init(record.inviteMsg, record.toflag != LRT_RECV);
			SetTextItem(textItem);
			result = true;
		}
	}break;
	case LRM_WARNING: {
		if (!record.warningMsg.empty())
		{
			LCWarningItem* warningItem = new LCWarningItem;
			warningItem->Init(record.warningMsg);
			SetWarningItem(warningItem);
			result = true;
		}
	}break;
	case LRM_EMOTION: {
		if (!record.emotionId.empty())
		{
			LCEmotionItem* emotionItem = emotionMgr->GetEmotion(record.emotionId);
			SetEmotionItem(emotionItem);
			result = true;
		}
	}break;
	case LRM_PHOTO: {
		if (!record.photoId.empty())
		{
			// 男士端发送的为已付费
			bool photoCharge = (m_sendType == SendType_Send ? true : record.photoCharge);
            // 获取PhotoItem
            LCPhotoItem* photoItem = photoMgr->GetPhotoItem(record.photoId, this);
			photoItem->Init(
					record.photoId
					, ""
					, record.photoDesc
					, photoMgr->GetPhotoPath(record.photoId, GMT_FUZZY, GPT_LARGE)
					, photoMgr->GetPhotoPath(record.photoId, GMT_FUZZY, GPT_MIDDLE)
					, photoMgr->GetPhotoPath(record.photoId, GMT_CLEAR, GPT_ORIGINAL)
					, photoMgr->GetPhotoPath(record.photoId, GMT_CLEAR, GPT_LARGE)
					, photoMgr->GetPhotoPath(record.photoId, GMT_CLEAR, GPT_MIDDLE)
					, photoCharge);
			result = true;
		}
	}break;
	case LRM_VOICE: {
		if (!record.voiceId.empty())
		{
			LCVoiceItem* voiceItem = new LCVoiceItem();
			voiceItem->Init(record.voiceId
					, voiceMgr->GetVoicePath(record.voiceId, record.voiceType)
					, record.voiceTime
					, record.voiceType
					, ""
					, true);
			SetVoiceItem(voiceItem);
			result = true;
		}
	}break;
	case LRM_VIDEO: {
		if (!record.videoId.empty())
		{
			lcmm::LCVideoItem* videoItem = new lcmm::LCVideoItem;
			videoItem->Init(
					record.videoId
					, record.videoSendId
					, record.videoDesc
					, ""
					, record.videoCharge);
			SetVideoItem(videoItem);
			result = true;
		}
	}break;
	default: {
		FileLog("LiveChatManager", "LCMessageItem::InitWithRecord() unknow message type");
	}break;
	}
	return result;
}

// 设置语音item
void LCMessageItem::SetVoiceItem(LCVoiceItem* theVoiceItem)
{
	if (m_msgType == MT_Unknow
			&& theVoiceItem != NULL)
	{
		m_voiceItem = theVoiceItem;
		m_msgType = MT_Voice;
	}
}

// 获取语音item
LCVoiceItem* LCMessageItem::GetVoiceItem() const
{
	return m_voiceItem;
}

// 设置图片item
void LCMessageItem::SetPhotoItem(LCPhotoItem* thePhotoItem)
{
	if ((m_msgType == MT_Unknow || m_msgType == MT_Photo)
			&& thePhotoItem != NULL)
	{
		m_photoItem = thePhotoItem;
		m_msgType = MT_Photo;
	}
}

// 获取图片item
LCPhotoItem* LCMessageItem::GetPhotoItem() const
{
	return m_photoItem;
}

// 设置视频item
void LCMessageItem::SetVideoItem(lcmm::LCVideoItem* theVideoItem)
{
	if (m_msgType == MT_Unknow
			&& theVideoItem != NULL)
	{
		m_videoItem = theVideoItem;
		m_msgType = MT_Video;
	}
}

// 获取视频item
lcmm::LCVideoItem* LCMessageItem::GetVideoItem() const
{
	return m_videoItem;
}

// 设置文本item
void LCMessageItem::SetTextItem(LCTextItem* theTextItem)
{
	if (m_msgType == MT_Unknow
			&& theTextItem != NULL)
	{
		m_textItem = theTextItem;
		m_msgType = MT_Text;
	}
}

// 获取文本item
LCTextItem* LCMessageItem::GetTextItem() const
{
	return m_textItem;
}

// 设置warning item
void LCMessageItem::SetWarningItem(LCWarningItem* theWarningItem)
{
	if (m_msgType == MT_Unknow
			&& theWarningItem != NULL)
	{
		m_warningItem = theWarningItem;
		m_msgType = MT_Warning;
	}
}

// 获取warning item
LCWarningItem* LCMessageItem::GetWarningItem() const
{
	return m_warningItem;
}

// 设置高级表情item
void LCMessageItem::SetEmotionItem(LCEmotionItem* theEmotionItem)
{
	if (m_msgType == MT_Unknow
			&& theEmotionItem != NULL)
	{
		m_emotionItem = theEmotionItem;
		m_msgType = MT_Emotion;
	}
}

// 获取高级表情item
LCEmotionItem* LCMessageItem::GetEmotionItem() const
{
	return m_emotionItem;
}

// 设置系统消息item
void LCMessageItem::SetSystemItem(LCSystemItem* theSystemItem)
{
	if (m_msgType == MT_Unknow
			&& theSystemItem != NULL)
	{
		m_systemItem = theSystemItem;
		m_msgType = MT_System;
	}
}

// 获取系统消息item
LCSystemItem* LCMessageItem::GetSystemItem() const
{
	return m_systemItem;
}

// 设置自定义消息item
void LCMessageItem::SetCustomItem(LCCustomItem* theCustomItem)
{
	if (m_msgType == MT_Unknow
			&& theCustomItem != NULL)
	{
		m_customItem = theCustomItem;
		m_msgType = MT_Custom;
	}
}

// 获取自定义消息item
LCCustomItem* LCMessageItem::GetCustomItem() const
{
	return m_customItem;	
}

// 判断子消息item（如：语音、图片、视频等）是否正在处理
bool LCMessageItem::IsSubItemProcssign() const
{
	bool result = false;
	switch (m_msgType)
	{
		case MT_Voice:
			{
				LCVoiceItem* voiceItem = GetVoiceItem();
				result = voiceItem->m_processing;
			}
			break;
		case MT_Photo:
			{
				LCPhotoItem* photoItem = GetPhotoItem();
				result = photoItem->IsProcessing();
			}
			break;
		case MT_Video:
			{
				lcmm::LCVideoItem* videoItem = GetVideoItem();
				result = videoItem->IsFee();
			}
			break;
        default:
            break;
	}
	return result;
}

// 设置用户item
void LCMessageItem::SetUserItem(LCUserItem* theUserItem)
{
	m_userItem = theUserItem;
}

// 获取用户item
LCUserItem* LCMessageItem::GetUserItem() const
{
	return m_userItem;
}

// 重置所有成员变量
void LCMessageItem::Clear()
{
	m_msgId = 0;
	m_sendType = SendType_Unknow;
	m_fromId = "";
	m_toId = "";
	m_inviteId = "";
	m_createTime = 0;
	m_statusType = StatusType_Unprocess;
	m_msgType = MT_Unknow;

	delete m_textItem;
	m_textItem = NULL;

	delete m_warningItem;
	m_warningItem = NULL;

	// release in LCEmotionManager
//	delete m_emotionItem;
	m_emotionItem = NULL;

	delete m_voiceItem;
	m_voiceItem = NULL;

    // release in LCPhotoManager
//	delete m_photoItem;
	m_photoItem = NULL;

	delete m_videoItem;
	m_videoItem = NULL;

	delete m_systemItem;
	m_systemItem = NULL;

	delete m_customItem;
	m_customItem = NULL;

	m_userItem = NULL;
}

bool LCMessageItem::Sort(const LCMessageItem* item1, const LCMessageItem* item2)
{
	// true排前面，false排后面
	bool result = false;

	if (item1->m_createTime == item2->m_createTime)
	{
		result = item1->m_msgId < item2->m_msgId;
	}
	else
	{
		result = item1->m_createTime < item2->m_createTime;
	}

	return result;
}

// 判断是否聊天消息
bool LCMessageItem::IsChatMessage()
{
    bool result = false;
    switch (m_msgType) {
        case MT_Text:
        case MT_Photo:
        case MT_Video:
        case MT_Voice:
        case MT_Emotion:
            result = true;
            break;
            
        default:
            break;
    }
    return result;
}

