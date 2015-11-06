/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCVideoManager.h
 *   desc: LiveChat视频管理器
 */

#include "LCVideoManager.h"
#include "IAutoLock.h"
#include <RequestLiveChatDefine.h>
#include <HttpRequestDefine.h>
#include <HttpDownloader.h>
#include <md5.h>
#include <CommonFunc.h>

LCVideoManager::LCVideoManager()
{
	m_dirPath = "";
}

LCVideoManager::~LCVideoManager()
{

}

// 初始化
bool LCVideoManager::Init(const string& dirPath)
{
	m_dirPath = dirPath;
	if (!m_dirPath.empty()
		&& m_dirPath.at(m_dirPath.length()-1) != '/'
		&& m_dirPath.at(m_dirPath.length()-1) != '\\')
	{
		m_dirPath += "/";
	}
	return !m_dirPath.empty();
}

// --------------------------- 获取视频本地缓存路径 -------------------------
// 获取视频图片本地缓存文件路径(全路径)
string LCVideoManager::GetVideoPhotoPath(const string& userId
										, const string& videoId
										, const string& inviteId
										, VIDEO_PHOTO_TYPE type)
{
	string path = "";
	if (!userId.empty()
		&& !videoId.empty()
		&& !inviteId.empty())
	{
		// 生成文件名
		string temp = userId + videoId + inviteId;
		char cFileName[128] = {0};
		GetMD5String(temp.c_str(), cFileName);

		// 生成类型
		char cType[32] = {0};
		snprintf(cType, sizeof(cType), "%d", type);

		// 生成文件全路径
		path = m_dirPath
				+ cFileName
				+ "_"
				+ "img"
				+ "_"
				+ cType;
	}
	return path;
}

// 获取视频图片临时文件路径
string LCVideoManager::GetVideoPhotoTempPath(const string& userId
											, const string& videoId
											, const string& inviteId
											, VIDEO_PHOTO_TYPE type)
{
	string tempPath = "";
	string path = GetVideoPhotoPath(userId, videoId, inviteId, type);
	if (!path.empty())
	{
		tempPath = path + "_temp";
	}
	return tempPath;
}

// 获取视频本地缓存文件路径(全路径)
string LCVideoManager::GetVideoPath(const string& userId, const string& videoId, const string& inviteId)
{
	string path = "";
	if (!userId.empty()
		&& !videoId.empty()
		&& !inviteId.empty())
	{
		// 生成文件名
		string temp = userId + videoId + inviteId;
		char cFileName[256] = {0};
		GetMD5String(temp.c_str(), cFileName);

		// 生成文件全路径
		path = m_dirPath + cFileName;
	}
	return path;
}

// 获取视频临时文件路径
string LCVideoManager::GetVideoTempPath(const string& userId, const string& videoId, const string& inviteId)
{
	string tempPath = "";
	string path = GetVideoPath(userId, videoId, inviteId);
	if (!path.empty())
	{
		tempPath = path + "_temp";
	}
	return tempPath;
}

// 下载完成的临时文件转换成正式文件
bool LCVideoManager::TempFileToDesFile(const string& tempPath, const string& desPath)
{
	bool result = false;
	if (!tempPath.empty()
		&& !desPath.empty())
	{
		if (RenameFile(tempPath, desPath))
		{
			result = true;
		}
	}
	return result;
}

// 清除所有视频文件
void LCVideoManager::RemoveAllVideoFile()
{
	if (!m_dirPath.empty())
	{
		CleanDir(m_dirPath);
	}
}

// --------------------------- Video消息处理 -------------------------
// 根据videoId获取消息列表里的所有视频消息
LCMessageList LCVideoManager::GetMessageItem(const string& videoId, LCUserItem* userItem)
{
	LCMessageList result;
	userItem->LockMsgList();
	if (!userItem->m_msgList.empty())
	{
		for (LCMessageList::iterator iter = userItem->m_msgList.begin();
				iter != userItem->m_msgList.end();
				iter++)
		{
			LCMessageItem* item = (*iter);
			if (item->m_msgType == LCMessageItem::MT_Video
				&& NULL != item->GetVideoItem()
				&& item->GetVideoItem()->m_videoId == videoId)
			{
				result.push_back(item);
			}
		}
	}
	userItem->UnlockMsgList();
	return result;
}

// 合并视频消息记录（把女士发出及男士已购买的视频记录合并为一条聊天记录）
void LCVideoManager::CombineMessageItem(LCUserItem* userItem)
{
	userItem->LockMsgList();
	if (!userItem->m_msgList.empty())
	{
		// 女士发送视频消息列表
		LCMessageList womanList;
		// 男士发送视频消息列表
		LCMessageList manList;
		// 找出所有男士和女士发出的视频消息
		for (LCMessageList::iterator iter = userItem->m_msgList.begin();
				iter != m_msgList.end();
				iter++)
		{
			LCMessageItem* item = (*iter);
			if (item->m_msgType == LCMessageItem::MT_Video
				&& NULL != item->GetVideoItem()
				&& !item->GetVideoItem()->m_videoId.empty())
			{
				if (item->m_sendType == LCMessageItem::SendType_Recv) {
					womanList.push_back(item);
				}
				else if (item->m_sendType == LCMessageItem::SendType_Send) {
					manList.push_back(item);
				}
			}
		}

		// 合并男士购买的视频消息
		if (!manList.empty() && !womanList.empty())
		{
			for (LCMessageList::iterator manIter = manList.begin();
					manIter != manList.end();
					manIter++)
			{
				for (LCMessageList::iterator womanIter = womanList.begin();
						womanIter != womanList.end();
						womanIter++)
				{
					LCMessageItem* manItem = (*manIter);
					LCMessageItem* womanItem = (*womanIter);
					LCVideoItem* manVideoItem = manItem->GetVideoItem();
					LCVideoItem* womanVideoItem = womanItem->GetVideoItem();
					if (manVideoItem->m_videoId == womanVideoItem->m_videoId
						&& manVideoItem->m_sendId == womanVideoItem->m_sendId)
					{
						// 男士发出的视频ID与女士发出的视频ID一致，需要合并
						userItem->m_msgList.remove(manItem);
						womanVideoItem.charget = true;
					}
				}
			}
		}
	}
	userItem->UnlockMsgList();
}

// --------------------------- 正在下载的视频图片对照表 -------------------------
// 获取正在下载的视频图片RequestId
long LCVideoManager::GetRequestIdWithVideoPhotoId(const string& videoId)
{
	long requestId = HTTPREQUEST_INVALIDREQUESTID;
	LockVideoPhotoRequestMap();
	VideoPhotoRequestMap::iterator iter = m_videoPhotoRequestMap.find(videoId);
	if (iter != m_videoPhotoRequestMap.end()) {
		requestId = (*iter).second;
	}
	UnlockVideoPhotoRequestMap();
	return requestId;
}

// 判断视频图片是否在下载
bool LCVideoManager::IsVideoPhotoRequest(const string& videoId)
{
	return GetRequestIdWithVideoPhotoId(videoId) != HTTPREQUEST_INVALIDREQUESTID;
}

// 获取并移除正在下载视频图片的视频Id
string LCVideoManager::GetAndRemoveRequestVideoPhoto(long requestId)
{
	string videoId = "";
	LockRequestVideoPhotoMap();
	LockVideoPhotoRequestMap();
	RequestVideoPhotoMap::iterator rvpIter = m_requestVideoPhotoMap.find(requestId);
	if (rvpIter != m_requestVideoPhotoMap.end()) {
		videoId = (*rvpIter).second;
		m_requestVideoPhotoMap.erase(videoId);

		VideoPhotoRequestMap::iterator vprIter = m_videoPhotoRequestMap.find(videoId);
		if (vprIter != m_videoPhotoRequestMap.end()) {
			m_videoPhotoRequestMap.erase(vprIter);
		}
	}
	UnlockVideoPhotoRequestMap();
	UnlockRequestVideoPhotoMap();
	return videoId;
}

// 添加下载的视频图片
bool LCVideoManager::AddRequestVideoPhoto(long requestId, const string& videoId)
{
	bool result = false;
	LockRequestVideoPhotoMap();
	LockVideoPhotoRequestMap();
	if (!videoId.empty()
		&& requestId != HTTPREQUEST_INVALIDREQUESTID)
	{
		if (m_requestVideoPhotoMap.find(requestId) == m_requestVideoPhotoMap.end())
		{
			m_requestVideoPhotoMap.insert(RequestVideoPhotoMap::value_type(requestId, videoId));
			m_videoPhotoRequestMap.insert(VideoPhotoRequestMap::value_type(videoId, requestId));

			result = true;
		}
	}
	UnlockVideoPhotoRequestMap();
	UnlockRequestVideoPhotoMap();
	return result;
}

// 清除所有下载的视频图片
list<long> LCVideoManager::ClearAllRequestVideoPhoto()
{
	list<long> result;
	LockRequestVideoPhotoMap();
	LockVideoPhotoRequestMap();

	// 找到所有下载视频图片的request
	for (RequestVideoPhotoMap::iterator iter = m_requestVideoPhotoMap.begin();
			iter != m_requestVideoPhotoMap.end();
			iter++)
	{
		result.push_back((*iter).first);
	}

	// 清空表
	m_requestVideoPhotoMap.clear();
	m_videoPhotoRequestMap.clear();

	UnlockRequestVideoPhotoMap();
	UnlockVideoPhotoRequestMap();
	return result;
}

// --------------------------- 正在下载的视频对照表 -------------------------
// 获取正在下载的视频下载器
public FileDownloader getDownloaderWithVideoId(String videoId)
{
	FileDownloader fileDownloader = null;
	synchronized(mVideoDownloadMap) {
		fileDownloader = mVideoDownloadMap.get(videoId);
	}
	return fileDownloader;
}

/**
 * 判断视频是否在下载
 * @param videoId	视频ID
 * @return
 */
public boolean isVideoDownload(String videoId)
{
	return getDownloaderWithVideoId(videoId) != null;
}

/**
 * 获取并移除正在下载的视频
 * @param requestId	请求ID
 * @return 视频ID
 */
public String getAndRemoveDownloadVideo(FileDownloader fileDownloader)
{
	String videoId = "";
	synchronized (mDownloadVideoMap)
	{
		videoId = mDownloadVideoMap.remove(fileDownloader);
		if (null != videoId) {
			synchronized(mVideoDownloadMap) {
				mVideoDownloadMap.remove(videoId);
			}
		}
	}
	return videoId;
}

/**
 * 添加下载的视频
 * @param fileDownloader	下载器
 * @param videoId			视频ID
 * @return
 */
public boolean addDownloadVideo(FileDownloader fileDownloader, String videoId) {
	boolean result = false;
	synchronized (mDownloadVideoMap)
	{
		if (null != videoId
			&& !videoId.empty()
			&& null != fileDownloader
			&& null == mDownloadVideoMap.get(fileDownloader))
		{
			mDownloadVideoMap.put(fileDownloader, videoId);
			synchronized(mVideoDownloadMap) {
				mVideoDownloadMap.put(videoId, fileDownloader);
			}
			result = true;
		}
	}
	return result;
}

/**
 * 清除所有下载的video
 */
public ArrayList<FileDownloader> clearAllRequestVideo() {
	ArrayList<FileDownloader> list = null;
	synchronized (mDownloadVideoMap)
	{
		if (mDownloadVideoMap.size() > 0) {
			list = new ArrayList<FileDownloader>(mDownloadVideoMap.keySet());
		}
		mDownloadVideoMap.clear();

		synchronized(mVideoDownloadMap) {
			mVideoDownloadMap.clear();
		}
	}
	return list;
}

// 正在下载视频map表(HttpDownloader, videoId)加锁
void LCVideoManager::LockDownloadVideoMap()
{
	if (NULL != m_downloadVideoMapLock) {
		m_downloadVideoMapLock->Lock();
	}
}

// 正在下载视频map表(HttpDownloader, videoId)解锁
void LCVideoManager::UnlockDownloadVideoMap()
{
	if (NULL != m_downloadVideoMapLock) {
		m_downloadVideoMapLock->Unlock();
	}
}

// 正在下载视频map表(videoId, FileDownloader)加锁
void LCVideoManager::LockVideoDownloadMap()
{
	if (NULL != m_videoDownloadMapLock) {
		m_videoDownloadMapLock->Lock();
	}
}

// 正在下载视频map表(videoId, FileDownloader)解锁
void LCVideoManager::UnlockVideoDownloadMap()
{
	if (NULL != m_videoDownloadMapLock) {
		m_videoDownloadMapLock->Unlock();
	}
}

// 正在下载视频图片map表(RequestId, videoId)（记录下载未成功的视频ID，下载成功则移除）加锁
void LCVideoManager::LockRequestVideoPhotoMap()
{
	if (NULL != m_requestVideoPhotoMapLock) {
		m_requestVideoPhotoMapLock->Lock();
	}
}

// 正在下载视频图片map表(RequestId, videoId)（记录下载未成功的视频ID，下载成功则移除）解锁
void LCVideoManager::UnlockRequestVideoPhotoMap()
{
	if (NULL != m_requestVideoPhotoMapLock) {
		m_requestVideoPhotoMapLock->Unlock();
	}
}

// 正在下载视频图片map表(videoId, RequestId)加锁
void LCVideoManager::LockVideoPhotoRequestMap()
{
	if (NULL != m_videoPhotoRequestMapLock) {
		m_videoPhotoRequestMapLock->Lock();
	}
}

// 正在下载视频图片map表(videoId, RequestId)解锁
void LCVideoManager::UnlockVideoPhotoRequestMap()
{
	if (NULL != m_videoPhotoRequestMapLock) {
		m_videoPhotoRequestMapLock->Unlock();
	}
}
