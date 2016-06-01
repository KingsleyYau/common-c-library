/*
 * author: Samson.Fan
 *   date: 2016-01-28
 *   file: LCVideoDownloader.h
 *   desc: LiveChat视频下载类
 */

#include "LCVideoDownloader.h"
#include "LCVideoManager.h"
#include <manrequesthandler/HttpRequestManager.h>
#include <manrequesthandler/RequestLiveChatController.h>
#include <common/CheckMemoryLeak.h>

LCVideoDownloader::LCVideoDownloader()
{
	m_videoMgr = NULL;
	m_callback = NULL;

	m_womanId = "";
	m_videoId = "";
	m_inviteId = "";
	m_filePath = "";
}

LCVideoDownloader::~LCVideoDownloader()
{
	Stop();
}

bool LCVideoDownloader::Init(LCVideoManager* videoMgr)
{
	bool result = false;
	if (NULL != videoMgr)
	{
		m_videoMgr = videoMgr;
		result = true;
	}

	FileLog("LiveChatManager", "LCVideoDownloader::Init()videoMgr:%p, result:%d"
			, videoMgr, result);

	return result;
}

bool LCVideoDownloader::StartDownload(
			LCVideoDownloaderCallback* callback
			, const string& userId
			, const string& sid
			, const string& womanId
			, const string& videoId
			, const string& inviteId
			, const string& videoUrl
			, const string& filePath
			, const string& httpUser
			, const string& httpPassword)
{
	bool result = false;
	if (NULL != callback
		&& !userId.empty()
		&& !sid.empty()
		&& !womanId.empty()
		&& !videoId.empty()
		&& !inviteId.empty()
		&& !filePath.empty())
	{
		result = m_downloader.StartDownload(videoUrl, filePath, this, httpUser, httpPassword);
		if (result)
		{
			m_callback = callback;

			m_womanId = womanId;
			m_videoId = videoId;
			m_inviteId = inviteId;
			m_videoUrl = videoUrl;
			m_filePath = filePath;
		}
	}
	return result;
}
	
void LCVideoDownloader::Stop()
{
	m_downloader.Stop();
}
	
string LCVideoDownloader::GetWomanId() const
{
	return m_womanId;
}
	
string LCVideoDownloader::GetVideoId() const
{
	return m_videoId;
}

string LCVideoDownloader::GetInviteId() const
{
	return m_inviteId;
}

string LCVideoDownloader::GetVideoUrl() const
{
	return m_videoUrl;
}
	
string LCVideoDownloader::GetFilePath() const
{
	return m_filePath;
}

// ---- IHttpDownloaderCallback ----
// 下载成功
void LCVideoDownloader::onSuccess(HttpDownloader* loader)
{
	if (NULL != m_callback) {
		m_callback->onFinish(this, true);
	}
}
	
// 下载失败
void LCVideoDownloader::onFail(HttpDownloader* loader)
{
	if (NULL != m_callback) {
		m_callback->onFinish(this, false);
	}
}

// 下载进度更新
void LCVideoDownloader::onUpdate(HttpDownloader* loader)
{
	
}
