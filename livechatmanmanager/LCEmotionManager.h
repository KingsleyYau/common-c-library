/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCEmotionManager.h
 *   desc: LiveChat高级表情管理类
 */

#pragma once

#include <httpclient/HttpDownloader.h>
#include <common/IAutoLock.h>
#include <manrequesthandler/RequestOtherController.h>
#include <manrequesthandler/item/Other.h>
#include "LCEmotionDownloader.h"

#include <map>
#include <list>
#include <common/list_lock.h>

using namespace std;

class LCEmotionManager;
class LCEmotionManagerCallback
{
public:
	LCEmotionManagerCallback() {};
	virtual ~LCEmotionManagerCallback() {};
public:
	virtual void OnDownloadEmotionImage(bool result, LCEmotionItem* emotionItem) = 0;
	virtual void OnDownloadEmotionPlayImage(bool result, LCEmotionItem* emotionItem) = 0;
};

class LCMessageItem;
class LCEmotionDownloader;
class LCEmotionManager : private LCEmotionDownloaderCallback
{
private:
	// <emotionId, item>
	typedef map<string, LCEmotionItem*> EmotionMap;
	// <msgId, LCMessageItem>
	typedef map<int, LCMessageItem*> ToSendMap;
	// <emotionId, downloader>
	typedef map<string, LCEmotionDownloader*> EmotionDownloaderMap;
private:
	// 已下载完成的downloader（待释放）
	typedef struct _tagFinishDownloaderItem {
		LCEmotionDownloader*	downloader;
		long long finishTime;

		_tagFinishDownloaderItem()
		{
			downloader = NULL;
			finishTime = 0;
		}

		_tagFinishDownloaderItem(const _tagFinishDownloaderItem& item)
		{
			downloader = item.downloader;
			finishTime = item.finishTime;
		}
	} FinishDownloaderItem;
	typedef list_lock<FinishDownloaderItem>	FinishDownloaderList;	// 已下载完成的downloader（待释放）

public:
	LCEmotionManager();
	virtual ~LCEmotionManager();

public:
	// 设置本地缓存目录路径
	bool SetDirPath(const string& dirPath, const string& logPath);
	// 初始化
	bool Init(const string& host, LCEmotionManagerCallback* callback);
	// 设置http认证帐号
	void SetAuthorization(const string& httpUser, const string& httpPassword);
private:
	// 设备下载路径
	bool SetDownloadPath(const string& downloadPath);

	// ------------------- 高级表情配置操作 --------------------
public:
	// 判断版本是否比当前配置版本新
	bool IsVerNewThenConfigItem(int version) const;
	// 更新配置
	bool UpdateConfigItem(const OtherEmotionConfigItem& configItem);
	// 获取配置item
	OtherEmotionConfigItem GetConfigItem() const;
private:
	// 从文件中获取配置item
	bool GetConfigItemWithFile();
	// 把配置item保存到文件
	bool SaveConfigItemToFile();

	// ------------------- 高级表情ID的map表操作 --------------------
public:
	// 获取/添加高级表情item
	LCEmotionItem* GetEmotion(const string& emotionId);
private:
	// 添加配置item的高级表情至map
	void AddEmotionsWithConfigItem(const OtherEmotionConfigItem& configItem);
	// 批量添加高级表情至map
	void AddEmotions(const OtherEmotionConfigItem::EmotionList& emotionList);
	// 添加高级表情至map
	bool AddEmotion(LCEmotionItem* item);
	// 清除所有高级表情item
	void RemoveAllEmotionItem();
	// 高级表情ID map加锁
	void LockEmotionMap();
	// 高级表情ID map解锁
	void UnlockEmotionMap();

	// ------------------- 高级表情发送map表操作 --------------------
public:
	// 获取并移除待发送的item
	LCMessageItem* GetAndRemoveSendingItem(int msgId);
	// 添加待发送item
	bool AddSendingItem(LCMessageItem* item);
	// 清除所有待发送item
	void RemoveAllSendingItems();
private:
	// 高级表情发送map加锁
	void LockToSendMap();
	// 高级表情发送map解锁
	void UnlockToSendMap();

	// ------------------- 路径处理（包括URL及本地路径） --------------------
public:
	// 获取高级表情图片目录
	string GetImageDir();
	// 获取缩略图路径
	string GetImagePath(const string& emotionId);
	// 获取高级表情图片下载URL
	string GetImageURL(const string& emotionId);
	// 获取播放大图的下载URL
	string GetPlayBigImageUrl(const string& emotionId);
	// 获取播放大图路径
	string GetPlayBigImagePath(const string& emotionId);
	// 获取播放大图的子图路径(带%s，需要转换)
	string GetPlayBigSubImagePath(const string& emotionId);
	// 获取播放中图的下载URL
	string GetPlayMidImageUrl(const string& emotionId);
	// 获取播放中图路径
	string GetPlayMidImagePath(const string& emotionId);
	// 获取播放中图的子图路径(带%s，需要转换)
	string GetPlayMidSubImagePath(const string& emotionId);
	// 获取播放小图的下载URL
	string GetPlaySmallImageUrl(const string& emotionId);
	// 获取播放小图路径
	string GetPlaySmallImagePath(const string& emotionId);
	// 获取播放小图的子图路径(带%s，需要转换)
	string GetPlaySmallSubImagePath(const string& emotionId);
private:
	// 删除所有高级表情文件
	void DeleteAllEmotionFile();

	// ------------ 下载image ------------
public:
	// 开始下载image
	bool StartDownloadImage(LCEmotionItem* item);
	// 停止下载image
	bool StopDownloadImage(LCEmotionItem* item);
	// 停止所有image下载
	void StopAllDownloadImage();
	// 释放所有待释放Downloader(已下载完成，包括成功/失败)
	void ClearAllDownloader();
	// 清除超过一段时间已下载完成的downloader
	void ClearFinishDownloaderWithTimer();
private:
	// 下载image map加锁
	void LockImgDownloadMap();
	// 下载image map解锁
	void UnlockImgDownloadMap();
	// 添加downloader到待释放列表(已下载完成，包括成功/失败)
	void AddToFinishDownloaderList(LCEmotionDownloader* downloader);

	// ------------ 下载播放image ------------
public:
	// 开始下载播放image
	bool StartDownloadPlayImage(LCEmotionItem* item);
	// 停止下载播放image
	bool StopDownloadPlayImage(LCEmotionItem* item);
	// 停止下载所有播放image
	bool StopAllDownloadPlayImage();
	// 下载播放image map加锁
	void LockPlayDownloadMap();
	// 下载播放image map解锁
	void UnlockPlayDownloadMap();

	// ------------- LCEmotionDownloaderCallback -------------
private:
	virtual void onSuccess(LCEmotionDownloader* downloader, LCEmotionDownloader::EmotionFileType fileType, LCEmotionItem* item);
	virtual void onFail(LCEmotionDownloader* downloader, LCEmotionDownloader::EmotionFileType fileType, LCEmotionItem* item);

private:
	EmotionMap	m_emotionMap;		// 高级表情ID与item的map表
	IAutoLock*	m_emotionMapLock;	// 高级表情map表锁

	ToSendMap	m_toSendMap;		// 正在发送map表（msgId, item）
	IAutoLock*	m_toSendMapLock;	// 正在发送map表锁

	string 		m_dirPath;			// 本地缓存目录路径
	string		m_host;				// http下载host

	string		m_downloadPath;		// 文件下载目录路径
	EmotionDownloaderMap	m_imgDownloadMap;		// 高级表情图片下载map表
	IAutoLock*				m_imgDownloadMapLock;	// 高级表情图片下载map表锁
	EmotionDownloaderMap	m_playImgDownloadMap;	// 高级表情播放图片下载map表
	IAutoLock*				m_playDownloadMapLock;	// 高级表情播放图片下载map表锁
	string		m_httpUser;			// http认证帐号
	string		m_httpPassword;		// http认证密码
	FinishDownloaderList	m_finishDownloaderList;		// 待释放下载器列表

	OtherEmotionConfigItem	m_configItem;			// 高级表情配置item
	LCEmotionManagerCallback*	m_callback;	// 回调

public:
	long 		m_emotionConfigReqId;	// GetEmotionConfig的RequestId
};
