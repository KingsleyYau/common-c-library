/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCEmotionManager.h
 *   desc: LiveChat高级表情管理类
 */

#pragma once

#include <HttpDownloader.h>
#include "LCEmotionItem.h"
#include <Other.h>
#include <IAutoLock.h>
#include <RequestOtherController.h>

#include <map>

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

class LCEmotionDownloader;
class LCEmotionManager : private LCEmotionDownloaderCallback
{
private:
	// <emotionId, item>
	typedef map<string, LCEmotionItem*> EmotionMap;
	// <msgId, item>
	typedef map<int, LCEmotionItem*> ToSendMap;
	// <emotionId, downloader>
	typedef map<string, LCEmotionDownloader*> EmotionDownloaderMap;

public:
	LCEmotionManager();
	virtual ~LCEmotionManager();

public:
	// 设置本地缓存目录路径
	bool SetDirPath(const string& dirPath, const string& logPath);
	// 初始化
	bool Init(const string& host, LCEmotionManagerCallback* callback);
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
private:
	// 添加配置item的高级表情至map
	void AddEmotionsWithConfigItem(const OtherEmotionConfigItem& configItem);
	// 批量添加高级表情至map
	void AddEmotions(OtherEmotionConfigItem::EmotionList& emotionList);
	// 添加高级表情至map
	bool AddEmotion(LCEmotionItem* item);
	// 获取/添加高级表情item
	LCEmotionItem* GetEmotion(const string& emotionId);
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
	bool StopAllDownloadImage();
private:
	// 下载image map加锁
	void LockImgDownloadMap();
	// 下载image map解锁
	void UnlockImgDownloadMap();

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
	virtual void onSuccess(LCEmotionDownloader::EmotionFileType fileType, LCEmotionItem* item);
	virtual void onFail(LCEmotionDownloader::EmotionFileType fileType, LCEmotionItem* item);

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

	long 		m_emotionConfigReqId;	// GetEmotionConfig的RequestId
	OtherEmotionConfigItem	m_configItem;			// 高级表情配置item
	LCEmotionManagerCallback	m_callback;	// 回调
};
