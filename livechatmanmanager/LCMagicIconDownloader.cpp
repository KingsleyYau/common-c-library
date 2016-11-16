/*
 * author: Alex shum
 *   date: 2016-09-08
 *   file: LCMagicIconDownloader.cpp
 *   desc: LiveChat小高级表情下载器
 */

#include "LCMagicIconDownloader.h"
#include <common/CommonFunc.h>
#include <common/KLog.h>
#include <common/CheckMemoryLeak.h>

LCMagicIconDownloader::LCMagicIconDownloader()
{
	m_callback = NULL;
	m_magicIconItem = NULL;
	m_downloadType = DEFAULT;
}

LCMagicIconDownloader::~LCMagicIconDownloader()
{

}

// 开始下载
bool LCMagicIconDownloader::Start(const string& url
								, const string& filePath
								, MagicIconDownloadType downloadType
								, LCMagicIconItem* item
								, LCMagicIconDownloaderCallback* callback
								, const string& httpUser
								, const string& httpPassword)
{
	bool result = false;

	FileLog("LiveChatManager", "LCMagicIconDownloader::Start() url:%s, filePath:%s, downloadType:%d, item:%p, callback:%p"
			, url.c_str(), filePath.c_str(), downloadType, item, callback);

	if (!url.empty()
		&& !filePath.empty()
		&& downloadType != DEFAULT
		&& item != NULL
		&& callback != NULL)
	{
		m_magicIconItem = item;
		m_callback = callback;
		m_downloadType = downloadType;
		result = m_downloader.StartDownload(url, filePath, this, httpUser, httpPassword);

		FileLog("LiveChatManager", "LCMagicIconDownloader::Start() url:%s, filePath:%s, downloadType:%d, result:%d"
				, url.c_str(), filePath.c_str(), downloadType, result);
	}

	return result;
}

// 停止下载
void LCMagicIconDownloader::Stop()
{
	m_downloader.Stop();
}

// --------------- IHttpDownloaderCallback ---------------
// 下载成功
void LCMagicIconDownloader::onSuccess(HttpDownloader* loader)
{
	if (m_magicIconItem != NULL) {
		if (m_downloadType == THUMB) {
			m_magicIconItem->m_thumbPath = loader->GetFilePath();
		}
		else if (m_downloadType == SOURCE) {
			m_magicIconItem->m_sourcePath = loader->GetFilePath();
		}
	}

	if (m_callback != NULL) {
		m_callback->onSuccess(this, m_downloadType, m_magicIconItem);
	}
}

// 下载失败
void LCMagicIconDownloader::onFail(HttpDownloader* loader)
{
	if (m_callback != NULL) {
		m_callback->onFail(this, m_downloadType, m_magicIconItem);
	}
}

// 下载进度更新
void LCMagicIconDownloader::onUpdate(HttpDownloader* loader)
{

}
