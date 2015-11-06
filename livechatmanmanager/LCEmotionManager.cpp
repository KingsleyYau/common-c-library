/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCEmotionManager.h
 *   desc: LiveChat高级表情管理类
 */

#include "LCEmotionManager.h"
#include "LCEmotionDownloader.h"
#include "LCMessageItem.h"
#include <PngHandler.h>
#include <string>

using namespace std;

static const char* g_imgPath = "img/";
static const char* g_imgExt = ".png";
static const char* g_playImgPath = "pad/";
static const char* g_playBigPath = "_b";
static const char* g_playMidPath = "_m";
static const char* g_playSmallPath = "_s";
static const char* g_playSubPath = "_%d";
static const char* g_playExt = ".png";
static const char* g_localImgPath = "img/";

LCEmotionManager::LCEmotionManager()
{
	m_emotionConfigReqId = HTTPREQUEST_INVALIDREQUESTID;
	m_downloadPath = "";
	m_dirPath = "";
	m_host = "";
	m_callback = NULL;

	m_emotionMapLock = IAutoLock::CreateAutoLock();
	m_toSendMapLock = IAutoLock::CreateAutoLock();
	m_imgDownloadMapLock = IAutoLock::CreateAutoLock();
	m_playDownloadMapLock = IAutoLock::CreateAutoLock();
}

LCEmotionManager::~LCEmotionManager()
{
	IAutoLock::ReleaseAutoLock(m_emotionMapLock);
	IAutoLock::ReleaseAutoLock(m_toSendMapLock);
	IAutoLock::ReleaseAutoLock(m_imgDownloadMapLock);
	IAutoLock::ReleaseAutoLock(m_playDownloadMapLock);
}

// 设置本地缓存目录路径
bool LCEmotionManager::SetDirPath(const string& dirPath, const string& logPath)
{
	bool result = false;
	if (!dirPath.empty() && !logPath.empty())
	{
		result = true;

		// 记录高级表情本地缓存访目录路径
		m_dirPath = dirPath;
		if ('/' != m_dirPath.at(m_dirPath.length()-1)
			|| '\\' != m_dirPath.at(m_dirPath.length()-1))
		{
			m_dirPath += "/";
		}

		// 创建高级表情image文件目录
		result = MakeDir(GetImageDir());

		// 设置 ImageHandler打log目录
		if (result)
		{
			ImageHandler.SetLogPath(logPath);
		}
	}
	return result;
}

// 初始化
bool LCEmotionManager::Init(const string& host
							, LCEmotionManagerCallback* callback)
{
	bool result = false;
	if (!host.empty()
		&& NULL != callback)
	{
		result = true;

		// 其它
		if (result)
		{
			m_host = host;
			if ('/' != m_host.at(m_host.length()-1)
					|| '\\' != m_host.at(m_host.length()-1))
			{
				m_host += "/";
			}

			m_callback = callback;

			// 从文件中读取配置
			if (GetConfigItemWithFile()) {
				// 设置下载路径
				SetDownloadPath(m_configItem.path);
				// 添加高级表情
				addEmotionsWithConfigItem(m_configItem);
			}
		}
	}
	return result;
}

// 设置下载路径
bool LCEmotionManager::SetDownloadPath(const string& downloadPath)
{
	bool result = false;
	if (!downloadPath.empty()) {
		m_downloadPath = downloadPath;
		if ('/' != m_downloadPath.at(m_downloadPath.length()-1)
				&& '\\' != m_downloadPath.at(m_downloadPath.length()-1))
		{
			m_downloadPath += "/";
		}

		if ('/' != m_downloadPath.at(0)
				|| '\\' != m_downloadPath.at(0))
		{
			m_downloadPath = m_downloadPath.substr(1);
		}
		result = true;
	}
	return result;
}


// ------------------- 高级表情配置操作 --------------------
// 从文件中获取配置item
bool LCEmotionManager::GetConfigItemWithFile() {
	bool result = false;
    try {
    	String key = "OtherEmotionConfigItem_" + WebSiteManager.newInstance(null).GetWebSite().getSiteId();
        SharedPreferences mSharedPreferences = mContext.getSharedPreferences("base64", Context.MODE_PRIVATE);
        String personBase64 = mSharedPreferences.getString(key, "");
        byte[] base64Bytes = Base64.decode(personBase64.getBytes(), Base64.DEFAULT);
        ByteArrayInputStream bais = new ByteArrayInputStream(base64Bytes);
        ObjectInputStream ois = new ObjectInputStream(bais);
        m_configItem = (OtherEmotionConfigItem) ois.readObject();
        if (null != m_configItem) {
        	result = true;
        }
    } catch (Exception e) {
        e.printStackTrace();
    }
	return result;
}

// 把配置item保存到文件
bool LCEmotionManager::SaveConfigItemToFile()
{
	bool result = false;

	try {
		String key = "OtherEmotionConfigItem_" + WebSiteManager.newInstance(null).GetWebSite().getSiteId();
		SharedPreferences mSharedPreferences = mContext.getSharedPreferences("base64", Context.MODE_PRIVATE);
		ByteArrayOutputStream baos = new ByteArrayOutputStream();
		ObjectOutputStream oos;
		oos = new ObjectOutputStream(baos);
		oos.writeObject(m_configItem);
		String personBase64 = new String(Base64.encode(baos.toByteArray(), Base64.DEFAULT));
		SharedPreferences.Editor editor = mSharedPreferences.edit();
		editor.putString(key, personBase64);
		result = editor.commit();
	} catch (IOException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	}

	return result;
}

// 判断版本是否比当前配置版本新
bool LCEmotionManager::IsVerNewThenConfigItem(int version) const
{
	return (version != m_configItem.version);
}

// 更新配置
bool LCEmotionManager::UpdateConfigItem(const OtherEmotionConfigItem& configItem)
{
	// 停止图片文件下载
	StopAllDownloadImage();
	// 停止3gp文件下载
	StopAllDownload3gp();
	// 删除本地缓存目录下所有文件
	DeleteAllEmotionFile();
	// 删除所有高级表情item
	removeAllEmotionItem();

	m_configItem = configItem;

	// 保存配置
	SaveConfigItemToFile();
	// 设置下载路径
	SetDownloadPath(m_configItem.path);
	// 添加高级表情
	addEmotionsWithConfigItem(m_configItem);

	return true;
}

// 获取配置item
OtherEmotionConfigItem LCEmotionManager::GetConfigItem() const
{
	return m_configItem;
}

// ------------------- 高级表情ID的map表操作 --------------------
// 添加配置item的高级表情至map
void LCEmotionManager::AddEmotionsWithConfigItem(const OtherEmotionConfigItem& configItem)
{
	addEmotions(configItem.manEmotionList);
	addEmotions(configItem.ladyEmotionList);
}

// 批量添加高级表情至map
void LCEmotionManager::AddEmotions(const OtherEmotionConfigItem::EmotionList& emotionList)
{
	for (OtherEmotionConfigItem::EmotionList iter = emotionList.begin();
			iter != emotionList.end();
			iter++)
	{
		if (!(*iter)->fileName.empty())
		{
			GetEmotion(emotionItem->fileName);
		}
	}
}

// 添加高级表情至map
bool LCEmotionManager::AddEmotion(LCEmotionItem* item)
{
	boolean result = false;
	if (NULL != item && !item->emotionId.empty())
	{
		LockEmotionMap();

		// add to map
		EmotionMap::const_iterator emotionIter = m_emotionMap.find(item->emotionId);
		if (m_emotionMap.end() != emotionIter)
		{
			// 判断image文件是否存在，若存在则赋值
			string imagePath = GetImagePath((*emotionIter).second->emotionId);
			if (IsFileExist(imagePath))
			{
				item->m_imagePath = imagePath;
			}

			m_emotionMap.insert(EmotionMap::value_type(item->emotionId, item));
			result = true;
		}

		UnlockEmotionMap();
	}
	return result;
}

// 获取/添加高级表情item
LCEmotionItem* LCEmotionManager::GetEmotion(const string& emotionId)
{
	LCEmotionItem* item = NULL;

	// 获取已有的高级表情item
	LockEmotionMap();
	EmotionMap::const_iterator iter = m_emotionMap.find(emotionId);
	if (iter != m_emotionMap.end()) {
		item = (*iter).second;
	}
	UnlockEmotionMap();

	// 若不存在则创建
	if (NULL == item) {
		item = new LCEmotionItem;
		item->Init(emotionId
				, GetImagePath(emotionId)
				, GetPlayBigImagePath(emotionId)
				, GetPlayBigSubImagePath(emotionId)
				);
		AddEmotion(item);
	}
	return item;
}

// 清除所有高级表情item
void LCEmotionManager::RemoveAllEmotionItem()
{
	LockEmotionMap();

	// 删除 LCEmotionItem object
	for (EmotionMap::iterator iter = m_emotionMap.begin();
			iter != m_emotionMap.end();
			iter++)
	{
		delete (*iter).second;
	}
	// 清空map表
	m_emotionMap.clear();

	UnlockEmotionMap();
}

// 高级表情ID map加锁
void LCEmotionManager::LockEmotionMap()
{
	if (NULL != m_emotionMapLock) {
		m_emotionMapLock->Lock();
	}
}

// 高级表情ID map解锁
void LCEmotionManager::UnlockEmotionMap()
{
	if (NULL != m_emotionMapLock) {
		m_emotionMapLock->Unlock();
	}
}

// ------------------- 路径处理（包括URL及本地路径） --------------------
// 获取高级表情图片目录
string LCEmotionManager::GetImageDir()
{
	return m_dirPath + g_localImgPath;
}

// 获取缩略图路径
string LCEmotionManager::GetImagePath(const string& emotionId)
{
	return GetImageDir() + emotionId;
}

// 获取高级表情图片下载URL
string LCEmotionManager::GetImageURL(const string& emotionId)
{
	return m_host + m_downloadPath + g_imgPath + emotionId + g_imgExt;
}

// 获取播放大图的下载URL
string LCEmotionManager::GetPlayBigImageUrl(const string& emotionId)
{
	return m_host + m_downloadPath + g_playImgPath + emotionId + g_playBigPath + g_playExt;
}

// 获取播放大图路径
string LCEmotionManager::GetPlayBigImagePath(const string& emotionId)
{
	return GetImageDir() + emotionId + g_playBigPath;
}

// 获取播放大图的子图路径(带%s，需要转换)
string LCEmotionManager::GetPlayBigSubImagePath(const string& emotionId)
{
	return GetImageDir() + emotionId + g_playBigPath + g_playSubPath;
}

// 获取播放中图的下载URL
string LCEmotionManager::GetPlayMidImageUrl(const string& emotionId)
{
	return m_host + m_downloadPath + g_playImgPath + emotionId + g_playMidPath + g_playExt;
}

// 获取播放中图路径
string LCEmotionManager::GetPlayMidImagePath(const string& emotionId)
{
	return getImageDir() + emotionId + g_playMidPath;
}

// 获取播放中图的子图路径(带%s，需要转换)
string LCEmotionManager::GetPlayMidSubImagePath(const string& emotionId)
{
	return GetImageDir() + emotionId + m_playMidPath + m_playSubPath;
}

// 获取播放小图的下载URL
string LCEmotionManager::GetPlaySmallImageUrl(const string& emotionId)
{
	return m_host + m_downloadPath + g_playImgPath + emotionId + g_playSmallPath + g_playExt;
}

// 获取播放小图路径
string LCEmotionManager::GetPlaySmallImagePath(const string& emotionId)
{
	return getImageDir() + emotionId + g_playSmallPath;
}

// 获取播放小图的子图路径(带%s，需要转换)
string LCEmotionManager::GetPlaySmallSubImagePath(const string& emotionId)
{
	return GetImageDir() + emotionId + g_playSmallPath + g_playSubPath;
}

// 删除所有高级表情文件
void LCEmotionManager::DeleteAllEmotionFile()
{
	// 删除image目录文件
	string strImageDir = GetImageDir();
	CleanDir(strImageDir);
}

// ------------------- 高级表情发送map表操作 --------------------
// 获取并移除待发送的item
LCMessageItem* LCEmotionManager::GetAndRemoveSendingItem(int msgId)
{
	LCMessageItem* item = NULL;

	LockToSendMap();

	ToSendMap::iterator iter = m_toSendMap.find(msgId);
	if (iter != m_toSendMap.end()) {
		item = (*iter).second;

		m_toSendMap.erase(iter);
	}

	UnlockToSendMap();

	return item;
}

// 添加待发送item
bool LCEmotionManager::AddSendingItem(LCMessageItem* item)
{
	bool result = false;

	LockToSendMap();

	if (item->msgType == MessageType.Emotion
		&& NULL != item->GetEmotionItem()
		&& m_toSendMap.end() == m_toSendMap.find(item->msgId))
	{
		m_toSendMap.insert(ToSendMap::value_type(item->msgId, item));
		result = true;
	}

	UnlockToSendMap();

	return result;
}

// 清除所有待发送item
void LCEmotionManager::RemoveAllSendingItems()
{
	LockToSendMap();

	m_toSendMap.clear();

	UnlockToSendMap();
}

// 高级表情发送map加锁
void LCEmotionManager::LockToSendMap()
{
	if (NULL != m_toSendMapLock) {
		m_toSendMapLock->Lock();
	}
}

// 高级表情发送map解锁
void LCEmotionManager::UnlockToSendMap()
{
	if (NULL != m_toSendMapLock) {
		m_toSendMapLock->Unlock();
	}
}

// ------------ 下载image ------------
// 开始下载image
bool LCEmotionManager::StartDownloadImage(LCEmotionItem* item)
{
	bool result = false;
	LockImgDownloadMap();

	if (!item->m_emotionId.empty()) {
		EmotionDownloaderMap::iterator iter = m_imgDownloadMap.find(item->m_emotionId);
		if (iter == m_imgDownloadMap.end()) {
			LCEmotionDownloader* loader = new LCEmotionDownloader;
			if (NULL != loader) {
				result = loader->Start(
						GetImageURL(item->m_emotionId)
						, GetImagePath(item->m_emotionId)
						, LCEmotionDownloader.fimage
						, item
						, this);

				if (result) {
					m_imgDownloadMap.insert(EmotionDownloaderMap::value_type(item->m_emotionId, item));
				}
				else {
					delete loader;
				}
			}
		}
	}

	UnlockImgDownloadMap();

	return result;
}

// 停止下载image
bool LCEmotionManager::StopDownloadImage(LCEmotionItem* item)
{
	bool result = false;
	LCEmotionDownloader* loader = NULL;

	// 加锁
	LockImgDownloadMap();

	// 找到并移除downloader
	EmotionDownloaderMap::iterator iter = m_imgDownloadMap.find(item->m_emotionId);
	if (iter != m_imgDownloadMap.end())
	{
		loader = (*iter).second;
		m_imgDownloadMap.erase(iter);
	}

	// 解锁
	UnlockImgDownloadMap();

	// 释放downloader
	if (NULL != loader)
	{
		loader->Stop();
		delete loader;

		result = true;
	}

	return result;
}

// 停止所有image下载
bool LCEmotionManager::StopAllDownloadImage()
{
	bool result = false;

	// 加锁
	LockImgDownloadMap();

	// 删除所有下载object
	for (EmotionDownloaderMap::iterator iter = m_imgDownloadMap.begin();
			iter != m_imgDownloadMap.end();
			iter++)
	{
		LCEmotionDownloader* loader = (*iter).second;
		if (NULL != loader) {
			loader->Stop();
			delete loader;
		}
	}
	// 清空map表
	m_imgDownloadMap.clear();

	// 解锁
	UnlockImgDownloadMap();
	return result;
}

// 下载image map加锁
void LCEmotionManager::LockImgDownloadMap()
{
	if (NULL != m_imgDownloadMapLock) {
		m_imgDownloadMapLock->Lock();
	}
}

// 下载image map解锁
void LCEmotionManager::UnlockImgDownloadMap()
{
	if (NULL != m_imgDownloadMapLock) {
		m_imgDownloadMapLock->Unlock();
	}
}

// ------------ 下载播放image ------------
// 开始下载播放image
bool LCEmotionManager::StartDownloadPlayImage(LCEmotionItem* item)
{
	bool result = false;

	// 加锁
	LockPlayDownloadMap();

	// 创建downloader并开始下载
	EmotionDownloaderMap::iterator iter = m_playImgDownloadMap.find(item->m_emotionId);
	if (iter != m_playImgDownloadMap.end() && !item->m_emotionId.empty())
	{
		LCEmotionDownloader* loader = new LCEmotionDownloader;
		result = loader.Start(
				GetPlayBigImageUrl(item->m_emotionId)
				, GetPlayBigImagePath(item->m_emotionId)
				, LCEmotionDownloader::fplaybigimage
				, item
				, this);

		if (result) {
			m_playImgDownloadMap.insert(EmotionDownloaderMap::value_type(item->m_emotionId, loader));
		}
		else {
			delete loader;
		}
	}

	// 解锁
	UnlockPlayDownloadMap();
	return result;
}

// 停止下载播放image
bool LCEmotionManager::StopDownloadPlayImage(LCEmotionItem* item)
{
	bool result = false;
	LCEmotionDownloader* loader = NULL;

	// 加锁
	LockPlayDownloadMap();

	// 找到item并移除
	EmotionDownloaderMap::iterator iter = m_playImgDownloadMap.find(item->m_emotionId);
	if (iter != m_playImgDownloadMap.end())
	{
		loader = (*iter).second;
		m_playImgDownloadMap.erase(iter);
	}

	// 解锁
	UnlockPlayDownloadMap();

	// 停止并删除object
	if (NULL != loader)
	{
		loader->Stop();
		delete loader;
		result = true;
	}

	return result;
}

// 停止下载所有播放image
bool LCEmotionManager::StopAllDownloadPlayImage()
{
	bool result = false;

	// 加锁
	LockPlayDownloadMap();

	for (EmotionDownloaderMap::iterator iter = m_playImgDownloadMap.begin();
			iter != m_playImgDownloadMap.end();
			iter++)
	{
		LCEmotionDownloader* loader = (*iter).second;
		if (NULL != loader) {
			loader->Stop();
			delete loader;
		}
	}
	m_playImgDownloadMap.clear();

	// 加锁
	UnlockPlayDownloadMap();
	return result;
}

// 下载播放image map加锁
void LCEmotionManager::LockPlayDownloadMap()
{
	if (NULL != m_playDownloadMapLock) {
		m_playDownloadMapLock->Lock();
	}
}

// 下载播放image map解锁
void LCEmotionManager::UnlockPlayDownloadMap()
{
	if (NULL != m_playDownloadMapLock) {
		m_playDownloadMapLock->Unlock();
	}
}

// ------------- LCEmotionDownloaderCallback -------------
void LCEmotionManager::onSuccess(LCEmotionDownloader::EmotionFileType fileType, LCEmotionItem* item)
{
	if (NULL != m_callback)
	{
		switch (fileType)
		{
		case LCEmotionDownloader::fimage: {
			// 回调
			m_callback->OnDownloadEmotionImage(true, item);

			// 移除出map
			LockImgDownloadMap();
			m_imgDownloadMap.erase(item->m_emotionId);
			UnlockImgDownloadMap();
		} break;
		case LCEmotionDownloader::fplaybigimage: {
			bool result = false;
			if (PngHandler::ConvertEmotionPng(item->m_playBigPath))
			{
				// 裁剪播放图片成功
				result = item->SetPlayBigSubPath(GetPlayBigSubImagePath(item->m_emotionId));
			}

			// 裁剪播放图片失败
			if (!result) {
				// 删除文件
				RemoveFile(item->m_playBigPath);
				// 把路径置空
				item->m_playBigPath = "";
				item->m_playBigPaths.clear();
			}

			// 回调
			m_callback->OnDownloadEmotionPlayImage(result, item);

			// 移除出map
			LockPlayDownloadMap();
			m_playImgDownloadMap.erase(item->m_emotionId);
			UnlockPlayDownloadMap();
		} break;
		default:
			break;
		}
	}
}

void LCEmotionManager::onFail(LCEmotionDownloader::EmotionFileType fileType, LCEmotionItem* item)
{
	if (NULL != m_callback)
	{
		switch (fileType)
		{
		case LCEmotionDownloader::fimage: {
			// 回调
			m_callback->OnDownloadEmotionImage(false, item);

			// 移除出map
			LockImgDownloadMap();
			m_imgDownloadMap.erase(item->m_emotionId);
			UnlockImgDownloadMap();
		} break;
		case LCEmotionDownloader::fplaybigimage: {
			// 回调
			m_callback->OnDownloadEmotionPlayImage(false, item);

			// 移除出map
			LockPlayDownloadMap();
			m_playImgDownloadMap.erase(item->m_emotionId);
			UnlockPlayDownloadMap();
		} break;
		default:
			break;
		}
	}
}

