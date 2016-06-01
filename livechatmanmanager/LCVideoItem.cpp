/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCVideoItem.h
 *   desc: LiveChat视频消息item
 */

#include "LCVideoItem.h"
#include <common/CommonFunc.h>
#include <manrequesthandler/RequestLiveChatDefine.h>
#include <common/CheckMemoryLeak.h>

using namespace lcmm;

LCVideoItem::LCVideoItem()
{
	m_videoId = "";
	m_videoDesc = "";
	m_sendId = "";
	m_videoUrl = "";
	m_charge = false;
}

LCVideoItem::~LCVideoItem()
{

}

// 初始化
bool LCVideoItem::Init(
		const string& videoId
		, const string& sendId
		, const string& videoDesc
		, const string& videoUrl
		, bool charge)
{
	bool result = false;

	if (!videoId.empty())
	{
		m_videoId = videoId;
		m_sendId = sendId;
		m_videoDesc = videoDesc;
		m_charge = charge;
		m_videoUrl = videoUrl;

		result = true;
	}

	return result;
}

// 添加视频付费状态
void LCVideoItem::AddProcessStatusFee()
{
	m_statusList.lock();
	if (!m_statusList.has(VideoFee)) {
		m_statusList.push_back(VideoFee);
	}
	m_statusList.unlock();
}

// 删除视频付费状态
void LCVideoItem::RemoveProcessStatusFee()
{
	m_statusList.lock();
	if (!m_statusList.has(VideoFee)) {
		m_statusList.erase(VideoFee);
	}
	m_statusList.unlock();
}
	
// 判断视频是否付费状态
bool LCVideoItem::IsFee()
{
	bool result = false;
	m_statusList.lock();
	result = m_statusList.has(VideoFee);
	m_statusList.unlock();
	return result;
}
