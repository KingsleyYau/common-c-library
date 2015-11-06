/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCMessageItem.cpp
 *   desc: LiveChat消息item
 */

#include "LCMessageItem.h"
#include "CommonFunc.h"

LCMessageItem::LCMessageItem()
{
	Clear();
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

		m_createTime = GetCreateTime();
		result = true;
	}
	return result;
}

// 获取生成时间
long LCMessageItem::GetCreateTime()
{
	return GetCurrentTime();
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
				, const Record* record
				, LCEmotionManager* emotionMgr
				, LCVoiceManager* voiceMgr
				, LCPhotoManager* photoMgr
				, LCVideoManager* videoMgr)
{
	bool result = false;
	m_msgId = msgId;
	m_toId = (record->toflag == LRT_RECV ? selfId : userId);
	m_fromId = (record->toflag == LRT_RECV ? userId : selfId);
	m_inviteId = inviteId;
	m_sendType = (record->toflag == LRT_RECV ? SendType_Recv : SendType_Send);
	m_statusType = StatusType_Finish;
	m_createTime = GetLocalTimeWithServerTime(record->adddate);

	switch(record->messageType) {
	case LRM_TEXT: {
		if (NULL != record->textMsg)
		{
			LCTextItem* textItem = new LCTextItem;
			textItem->Init(record->textMsg);
			SetTextItem(textItem);
			result = true;
		}
	}break;
	case LRM_INVITE: {
		if (NULL != record->inviteMsg)
		{
			LCTextItem* textItem = new LCTextItem;
			textItem->Init(record->inviteMsg);
			SetTextItem(textItem);
			result = true;
		}
	}break;
	case Warning: {
		if (NULL != record->warningMsg)
		{
			LCWarningItem* warningItem = new LCWarningItem;
			warningItem->Init(record->warningMsg);
			SetWarningItem(warningItem);
			result = true;
		}
	}break;
	case Emotion: {
		if (NULL != record->emotionId)
		{
			LCEmotionItem* emotionItem = emotionMgr->GetEmotion(record->emotionId);
			SetEmotionItem(emotionItem);
			result = true;
		}
	}break;
	case Photo: {
		if (NULL != record->photoId)
		{
			LCPhotoItem* photoItem = new LCPhotoItem;
			// 男士端发送的为已付费
			bool photoCharge = (m_sendType == SendType_Send ? true : record->photoCharge);
			photoItem->Init(
					record->photoId
					, ""
					, record->photoDesc
					, photoMgr->GetPhotoPath(record->photoId, PhotoModeType.Fuzzy, PhotoSizeType.Large)
					, photoMgr->GetPhotoPath(record->photoId, PhotoModeType.Fuzzy, PhotoSizeType.Middle)
					, photoMgr->GetPhotoPath(record->photoId, PhotoModeType.Clear, PhotoSizeType.Original)
					, photoMgr->GetPhotoPath(record->photoId, PhotoModeType.Clear, PhotoSizeType.Large)
					, photoMgr->GetPhotoPath(record->photoId, PhotoModeType.Clear, PhotoSizeType.Middle)
					, photoCharge);
			SetPhotoItem(photoItem);
			result = true;
		}
	}break;
	case Voice: {
		if (NULL != record->voiceId)
		{
			LCVoiceItem* voiceItem = new LCVoiceItem();
			voiceItem->Init(record->voiceId
					, voiceMgr->GetVoicePath(record->voiceId, record->voiceType)
					, record->voiceTime, record->voiceType
					, ""
					, true);
			SetVoiceItem(voiceItem);
			result = true;
		}
	}break;
	case Video: {
		if (NULL != record->videoId)
		{
			LCVideoItem videoItem = new LCVideoItem();
			videoItem.init(
					record->videoId
					, record->videoSendId
					, record->videoDesc
					, videoMgr.getVideoPhotoPath(userId, record->videoId, inviteId, VideoPhotoType.Big)
					, videoMgr.getVideoPhotoPath(userId, record->videoId, inviteId, VideoPhotoType.Default)
					, ""
					, videoMgr.getVideoPath(userId, record->videoId, inviteId)
					, record->videoCharge);
			setVideoItem(videoItem);
			result = true;
		}
	}break;
	default: {
		Log.e("livechat", String.format("%s::%s() unknow message type", "LCMessageItem", "InitWithRecord"));
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
LCVoiceItem* LCMessageItem::GetVoiceItem()
{
	return m_voiceItem;
}

// 设置图片item
void LCMessageItem::SetPhotoItem(LCPhotoItem* thePhotoItem)
{
	if (m_msgType == MT_Unknow
			&& thePhotoItem != NULL)
	{
		m_photoItem = thePhotoItem;
		m_msgType = MT_Photo;
	}
}

// 获取图片item
LCPhotoItem* LCMessageItem::GetPhotoItem()
{
	return m_photoItem;
}

// 设置视频item
void LCMessageItem::SetVideoItem(LCVideoItem* theVideoItem)
{
	if (m_msgType == MT_Unknow
			&& theVideoItem != NULL)
	{
		m_videoItem = theVideoItem;
		m_msgType = MT_Video;
	}
}

// 获取视频item
LCVideoItem* LCMessageItem::GetVideoItem()
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
LCTextItem LCMessageItem::GetTextItem()
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
LCWarningItem* LCMessageItem::GetWarningItem()
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
LCEmotionItem* LCMessageItem::GetEmotionItem()
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
LCSystemItem* LCMessageItem::GetSystemItem()
{
	return m_systemItem;
}

// 设置用户item
void LCMessageItem::SetUserItem(LCUserItem* theUserItem)
{
	m_userItem = theUserItem;
}

// 获取用户item
LCUserItem* LCMessageItem::GetUserItem()
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

	delete m_emotionItem;
	m_emotionItem = NULL;

	delete m_voiceItem;
	m_voiceItem = NULL;

	delete m_photoItem;
	m_photoItem = NULL;

	delete m_videoItem;
	m_videoItem = NULL;

	delete m_systemItem;
	m_systemItem = NULL;

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
