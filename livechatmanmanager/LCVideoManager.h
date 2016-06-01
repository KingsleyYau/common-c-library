/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCVideoManager.h
 *   desc: LiveChat视频管理器
 */

#pragma once

#include "LCMessageItem.h"
#include "LCVideoItem.h"
#include <common/dualmap_lock.h>
#include <string>
#include "LCUserManager.h"
#include "LCVideoPhotoDownloader.h"
#include "LCVideoDownloader.h"
using namespace std;


class LCVideoManagerCallback;
class HttpDownloader;
class LCVideoManager : private LCVideoPhotoDownloaderCallback
								, LCVideoDownloaderCallback
{
private:
	typedef dualmap_lock<string, LCVideoPhotoDownloader*>	DownloadVideoPhotoMap;	// 正在下载视频图片map表(videoId, downloader)
	typedef dualmap_lock<string, LCVideoDownloader*>		DownloadVideoMap;		// 正在下载视频map表(videoId, downloader)
	typedef dualmap_lock<LCMessageItem*, long>				VideoFeeMap;			// 正在付费视频map(message, requestId)
private:
	// ---- 已下载完成的视频图片downloader（待释放）----
	typedef struct _tagFinishVideoPhotoDownloaderItem {
		LCVideoPhotoDownloader*	downloader;
		long long finishTime;

		_tagFinishVideoPhotoDownloaderItem()
		{
			downloader = NULL;
			finishTime = 0;
		}

		_tagFinishVideoPhotoDownloaderItem(const _tagFinishVideoPhotoDownloaderItem& item)
		{
			downloader = item.downloader;
			finishTime = item.finishTime;
		}
	} FinishVideoPhotoDownloaderItem;
	typedef list_lock<FinishVideoPhotoDownloaderItem>	FinishVideoPhotoDownloaderList;	// 已下载完成的视频图片downloader（待释放）

	// ---- 已下载完成的视频downloader（待释放） ----
	typedef struct _tagFinishVideoDownloaderItem {
		LCVideoDownloader*	downloader;
		long long finishTime;

		_tagFinishVideoDownloaderItem()
		{
			downloader = NULL;
			finishTime = 0;
		}

		_tagFinishVideoDownloaderItem(const _tagFinishVideoDownloaderItem& item)
		{
			downloader = item.downloader;
			finishTime = item.finishTime;
		}
	} FinishVideoDownloaderItem;
	typedef list_lock<FinishVideoDownloaderItem>	FinishVideoDownloaderList;	// 已下载完成的视频downloader（待释放）

public:
	LCVideoManager();
	virtual ~LCVideoManager();

public:
	// 初始化
	bool Init(LCVideoManagerCallback* callback, LCUserManager* userMgr);
	// 设置本地缓存目录
	bool SetDirPath(const string& dirPath);
	// 设置http接口参数
	bool SetHttpRequest(HttpRequestManager* requestMgr, RequestLiveChatController* requestController);
	// 设置http认证帐号
	void SetAuthorization(const string& httpUser, const string& httpPassword);

	// --------------------------- 获取视频本地缓存路径 -------------------------
public:
	// 获取视频图片本地缓存文件路径(全路径)
	string GetVideoPhotoPath(const string& userId
							, const string& videoId
							, const string& inviteId
							, VIDEO_PHOTO_TYPE type);
	// 获取视频图片文件路径（仅已存在）
	string GetVideoPhotoPathWithExist(const string& userId
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
	// 获取视频文件路径(仅已存在)
	string GetVideoPathWithExist(const string& userId, const string& videoId, const string& inviteId);
	// 获取视频临时文件路径
	string GetVideoTempPath(const string& userId, const string& videoId, const string& inviteId);

	// 下载完成的临时文件转换成正式文件
	bool TempFileToDesFile(const string& tempPath, const string& desPath);
	// 清除所有视频文件
	void RemoveAllVideoFile();

	// --------------------------- Video消息处理 -------------------------
public:
	// 合并视频消息记录（把女士发出及男士已购买的视频记录合并为一条聊天记录）
	void CombineMessageItem(LCUserItem* userItem);
private:
	// 根据videoId获取对应视频的消息列表
	LCMessageList GetMessageItem(const string& userId, const string& videoId);

	// --------------------------- 视频图片 -------------------------
public:
	// 开始下载视频图片
	bool DownloadVideoPhoto(const string& userId, const string& sId, const string& womanId, const string& videoId, const string& inviteId, VIDEO_PHOTO_TYPE type);
	// 判断视频图片是否正在下载
	bool IsDownloadVideoPhoto(const string& videoId);
	// 停止下载视频图片
	bool StopDownloadVideoPhoto(const string& videoId);
	// 清除超过一段时间已下载完成的视频图片下载器
	void ClearFinishVideoPhotoDownloaderWithTimer();
	// 停止并清除所有视频图片下载
	void ClearAllDownloadVideoPhoto();

	// 接口请求回调
	void OnGetVideoPhoto(long requestId, bool success, const string& errnum, const string& errmsg, const string& filePath);
private:
	// 清除所有已下载完成的视频图片下载器
	void ClearAllFinishVideoPhotoDownloader();
	
	// -- LCVideoPhotoDownloaderCallback --
	virtual void onFinish(LCVideoPhotoDownloader* downloader, bool success, const string& errnum, const string& errmsg);

	// --------------------------- 视频 -------------------------
public:
	// 开始下载视频
	bool DownloadVideo(const string& userId, const string& sid, const string& womanId, const string& videoId, const string& inviteId, const string& videoUrl);
	// 判断视频是否正在下载
	bool IsDownloadVideo(const string& videoId);
	// 停止下载视频
	bool StopDownloadVideo(const string& videoId);
	// 清除超过一段时间已下载完成的视频图片下载器
	void ClearFinishVideoDownloaderWithTimer();
	// 停止并清除所有视频下载
	void ClearAllDownloadVideo();
	
private:
	// 清除所有已下载完成的视频下载器
	void ClearAllFinishVideoDownloader();

	// -- LCVideoDownloaderCallback --
	virtual void onFinish(LCVideoDownloader* downloader, bool success);

	// --------------------------- 付费 -------------------------
public:
	// 添加正在付费视频
	bool AddPhotoFee(LCMessageItem* item, long requestId);
	// 判断视频是否正在付费
	bool IsPhotoFee(LCMessageItem* item);
	// 移除正在付费视频
	LCMessageItem* RemovePhotoFee(long requestId);
	// 清除所有正在付费的视频
	void ClearAllPhotoFee();

private:
	LCVideoManagerCallback*	m_callback;	// callback
	LCUserManager*			m_userMgr;	// 用户管理器

	HttpRequestManager*			m_requestMgr;			// http管理器
	RequestLiveChatController*	m_requestController;	// http处理器

	string	m_httpUser;			// http认证帐号
	string	m_httpPassword;		// http认证密码

	string	m_dirPath;					// 本地缓存目录路径

	DownloadVideoPhotoMap		m_downloadVideoPhotoMap;		// 正在下载视频图片map表
	FinishVideoPhotoDownloaderList	m_finishVideoPhotoDownloaderList;	// 待释放的视频图片下载器列表

	DownloadVideoMap			m_downloadVideoMap;				// 正在下载视频map表
	FinishVideoDownloaderList	m_finishVideoDownloaderList;	// 待释放的视频下载器列表

	VideoFeeMap		m_videoFeeMap;		// 正在付费视频map表
};

class LCVideoManagerCallback
{
public:
	LCVideoManagerCallback() {};
	virtual ~LCVideoManagerCallback() {};

public:
	// 视频图片下载完成回调
	virtual void OnDownloadVideoPhoto(
					bool success
					, const string& errnum
					, const string& errmsg
					, const string& womanId
					, const string& inviteId
					, const string& videoId
					, VIDEO_PHOTO_TYPE type
					, const string& filePath
					, const LCMessageList& msgList) = 0;
	// 视频下载完成回调
	virtual void OnDownloadVideo(bool success, const string& userId, const string& videoId, const string& inviteId, const string& filePath, const LCMessageList& msgList) = 0;
};
