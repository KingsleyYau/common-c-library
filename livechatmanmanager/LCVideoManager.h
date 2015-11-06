/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCVideoManager.h
 *   desc: LiveChat视频管理器
 */

#pragma once

#include <map>
#include <string>
using namespace std;

class IAutoLock;
class HttpDownloader;
class LCVideoManager
{
private:
	typedef map<HttpDownloader*, string>	DownloadVideoMap;		// 正在下载视频map表(HttpDownloader, videoId)（记录下载未成功的视频ID，下载成功则移除）
	typedef map<string, HttpDownloader*>	VideoDownloadMap;		// 正在下载视频map表(videoId, FileDownloader)
	typedef map<long, string>				RequestVideoPhotoMap;	// 正在下载视频图片map表(RequestId, videoId)（记录下载未成功的视频ID，下载成功则移除）
	typedef map<string, long>				VideoPhotoRequestMap;	// 正在下载视频图片map表(videoId, RequestId)

public:
	LCVideoManager();
	virtual ~LCVideoManager();

public:
	// 初始化
	bool Init(const string& dirPath);

	// --------------------------- 获取视频本地缓存路径 -------------------------
public:
	// 获取视频图片本地缓存文件路径(全路径)
	string GetVideoPhotoPath(const string& userId
							, const string& videoId
							, const string& inviteId
							, VIDEO_PHOTO_TYPE type);
	// 获取视频图片临时文件路径
	string GetVideoPhotoTempPath(const string& userId
								, const string& videoId
								, const string& inviteId
								, VIDEO_PHOTO_TYPE type);
	// 获取视频本地缓存文件路径(全路径)
	string GetVideoPath(const string& userId, const string& videoId, const string& inviteId);
	// 获取视频临时文件路径
	string GetVideoTempPath(const string& userId, const string& videoId, const string& inviteId);
	// 下载完成的临时文件转换成正式文件
	bool TempFileToDesFile(const string& tempPath, const string& desPath);
	// 清除所有视频文件
	void RemoveAllVideoFile();

	// --------------------------- Video消息处理 -------------------------
public:
	// 根据videoId获取消息列表里的所有视频消息
	LCMessageList GetMessageItem(const string& videoId, LCUserItem* userItem);
	// 合并视频消息记录（把女士发出及男士已购买的视频记录合并为一条聊天记录）
	void CombineMessageItem(LCUserItem* userItem);

	// --------------------------- 正在下载的视频图片对照表 -------------------------
public:
	// 获取正在下载的视频图片RequestId
	long GetRequestIdWithVideoPhotoId(const string& videoId);
	// 判断视频图片是否在下载
	bool IsVideoPhotoRequest(const string& videoId);
	// 获取并移除正在下载视频图片的视频Id
	string GetAndRemoveRequestVideoPhoto(long requestId);
	// 添加下载的视频图片
	bool AddRequestVideoPhoto(long requestId, const string& videoId);
	// 清除所有下载的视频图片
	list<long> ClearAllRequestVideoPhoto();

	// --------------------------- 正在下载的视频对照表 -------------------------
public:

private:
	// 正在下载视频map表(HttpDownloader, videoId)加锁
	void LockDownloadVideoMap();
	// 正在下载视频map表(HttpDownloader, videoId)解锁
	void UnlockDownloadVideoMap();

	// 正在下载视频map表(videoId, FileDownloader)加锁
	void LockVideoDownloadMap();
	// 正在下载视频map表(videoId, FileDownloader)解锁
	void UnlockVideoDownloadMap();

	// 正在下载视频图片map表(RequestId, videoId)（记录下载未成功的视频ID，下载成功则移除）加锁
	void LockRequestVideoPhotoMap();
	// 正在下载视频图片map表(RequestId, videoId)（记录下载未成功的视频ID，下载成功则移除）解锁
	void UnlockRequestVideoPhotoMap();

	// 正在下载视频图片map表(videoId, RequestId)加锁
	void LockVideoPhotoRequestMap();
	// 正在下载视频图片map表(videoId, RequestId)解锁
	void UnlockVideoPhotoRequestMap();

private:
	string	m_dirPath;		// 本地缓存目录路径

	DownloadVideoMap		m_downloadVideoMap;			// 正在下载视频map表(HttpDownloader, videoId)（记录下载未成功的视频ID，下载成功则移除）
	IAutoLock*				m_downloadVideoMapLock;		// 正在下载视频map表锁(HttpDownloader, videoId)

	VideoDownloadMap		m_videoDownloadMap;			// 正在下载视频map表(videoId, FileDownloader)
	IAutoLock*				m_videoDownloadMapLock;		// 正在下载视频map表锁(videoId, FileDownloader)

	RequestVideoPhotoMap	m_requestVideoPhotoMap;		// 正在下载视频图片map表(RequestId, videoId)（记录下载未成功的视频ID，下载成功则移除）
	IAutoLock*				m_requestVideoPhotoMapLock;	// 正在下载视频图片map表锁(RequestId, videoId)

	VideoPhotoRequestMap	m_videoPhotoRequestMap;		// 正在下载视频图片map表(videoId, RequestId)
	IAutoLock*				m_videoPhotoRequestMapLock;	// 正在下载视频图片map表锁(videoId, RequestId)
};
