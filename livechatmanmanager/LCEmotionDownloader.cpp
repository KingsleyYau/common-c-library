/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCEmotionDownloader.h
 *   desc: LiveChat高级表情下载器
 */

#include "LCEmotionDownloader.h"
#include <common/CommonFunc.h>
#include <common/KLog.h>
#include <common/CheckMemoryLeak.h>

LCEmotionDownloader::LCEmotionDownloader()
{
	m_callback = NULL;
	m_emotionItem = NULL;
	m_fileType = funknow;
}

LCEmotionDownloader::~LCEmotionDownloader()
{

}

// 开始下载
bool LCEmotionDownloader::Start(const string& url
								, const string& filePath
								, EmotionFileType fileType
								, LCEmotionItem* item
								, LCEmotionDownloaderCallback* callback
								, const string& httpUser
								, const string& httpPassword)
{
	bool result = false;

	FileLog("LiveChatManager", "LCEmotionDownloader::Start() url:%s, filePath:%s, fileType:%d, item:%p, callback:%p"
			, url.c_str(), filePath.c_str(), fileType, item, callback);

	if (!url.empty()
		&& !filePath.empty()
		&& fileType != funknow
		&& item != NULL
		&& callback != NULL)
	{
		m_emotionItem = item;
		m_callback = callback;
		m_fileType = fileType;
		result = m_downloader.StartDownload(url, filePath, this, httpUser, httpPassword);

		FileLog("LiveChatManager", "LCEmotionDownloader::Start() url:%s, filePath:%s, fileType:%d, result:%d"
				, url.c_str(), filePath.c_str(), fileType, result);
	}

	return result;
}

// 停止下载
void LCEmotionDownloader::Stop()
{
	m_downloader.Stop();
}

// --------------- IHttpDownloaderCallback ---------------
// 下载成功
void LCEmotionDownloader::onSuccess(HttpDownloader* loader)
{
	if (m_emotionItem != NULL) {
		if (m_fileType == fimage) {
			m_emotionItem->m_imagePath = loader->GetFilePath();
		}
		else if (m_fileType == fplaybigimage) {
			m_emotionItem->m_playBigPath = loader->GetFilePath();
		}
	}

	if (m_callback != NULL) {
		m_callback->onSuccess(this, m_fileType, m_emotionItem);
	}
}

// 下载失败
void LCEmotionDownloader::onFail(HttpDownloader* loader)
{
	if (m_callback != NULL) {
		m_callback->onFail(this, m_fileType, m_emotionItem);
	}
}

// 下载进度更新
void LCEmotionDownloader::onUpdate(HttpDownloader* loader)
{

}
