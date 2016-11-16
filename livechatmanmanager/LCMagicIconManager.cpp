/*
 * author: Alex shum
 *   date: 2016-09-08
 *   file: LCMagicIconManager.cpp
 *   desc: LiveChat小高级表情管理类
 */

#include "LCMagicIconManager.h"
#include "LCMagicIconItem.h"
#include "LCMagicIconDownloader.h"
#include "LCMessageItem.h"
#include <imghandle/PngHandler.h>
#include <common/CommonFunc.h>
#include <string>
#include <common/CheckMemoryLeak.h>

using namespace std;

static const char* g_imgSubPath    = "img/";
static const char* g_thumbSubPath  = "thumb/";
static const char* g_normalSuffix  = ".png";
static const char* g_thumSuffix    = "-192.png";

LCMagicIconManager::LCMagicIconManager()
{
	m_magicIconConfigReqId = HTTPREQUEST_INVALIDREQUESTID;
	m_downloadPath = "";
	m_dirPath = "";
	m_host = "";
	m_callback = NULL;
	m_httpUser = "";
	m_httpPassword = "";

	m_magicIconMapLock = IAutoLock::CreateAutoLock();
	if (NULL != m_magicIconMapLock) {
		m_magicIconMapLock->Init();
	}

	m_toSendMapLock = IAutoLock::CreateAutoLock();
	if (NULL != m_toSendMapLock) {
		m_toSendMapLock->Init();
	}

	m_sourceImgDownloadMapLock = IAutoLock::CreateAutoLock();
	if (NULL != m_sourceImgDownloadMapLock) {
		m_sourceImgDownloadMapLock->Init();
	}

	m_thumbImgDownloadMapLock = IAutoLock::CreateAutoLock();
	if (NULL != m_thumbImgDownloadMapLock) {
		m_thumbImgDownloadMapLock->Init();
	}
}


LCMagicIconManager::~LCMagicIconManager()
{
	IAutoLock::ReleaseAutoLock(m_magicIconMapLock);
	m_magicIconMapLock = NULL;

	IAutoLock::ReleaseAutoLock(m_toSendMapLock);
	m_toSendMapLock = NULL;

	IAutoLock::ReleaseAutoLock(m_sourceImgDownloadMapLock);
	m_sourceImgDownloadMapLock = NULL;

	IAutoLock::ReleaseAutoLock(m_thumbImgDownloadMapLock);
	m_thumbImgDownloadMapLock = NULL;

	RemoveAllMagicIconItem();
	ClearAllDownloader();
}

// 设置本地缓存目录路径
bool LCMagicIconManager::SetDirPath(const string& dirPath, const string& logPath)
{
	bool result = false;
	if (!dirPath.empty() && !logPath.empty())
	{
		result = true;

		// 记录小高级表情本地缓存访目录路径
		m_dirPath = dirPath;
		if ('/' != m_dirPath.at(m_dirPath.length()-1)
			|| '\\' != m_dirPath.at(m_dirPath.length()-1))
		{
			m_dirPath += "/";
		}

		// 创建小高级表情image文件目录
		result = MakeDir(GetImageDir());
	}
	return result;
}

// 初始化
bool LCMagicIconManager::Init(const string& host
							, LCMagicIconManagerCallback* callback)
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
				// 添加小高级表情
                AddMagicIconWithConfigItem(m_configItem);
			}
		}
	}
	return result;
}

// 设置http认证帐号
void LCMagicIconManager::SetAuthorization(const string& httpUser, const string& httpPassword)
{
	m_httpUser = httpUser;
	m_httpPassword = httpPassword;
}

// 设置下载路径
bool LCMagicIconManager::SetDownloadPath(const string& downloadPath)
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


// ------------------- 小高级表情配置操作 --------------------
// 从文件中获取配置item
bool LCMagicIconManager::GetConfigItemWithFile() {
	bool result = false;
//    try {
//    	String key = "MagicIconConfigItem_" + WebSiteManager.newInstance(null).GetWebSite().getSiteId();
//        SharedPreferences mSharedPreferences = mContext.getSharedPreferences("base64", Context.MODE_PRIVATE);
//        String personBase64 = mSharedPreferences.getString(key, "");
//        byte[] base64Bytes = Base64.decode(personBase64.getBytes(), Base64.DEFAULT);
//        ByteArrayInputStream bais = new ByteArrayInputStream(base64Bytes);
//        ObjectInputStream ois = new ObjectInputStream(bais);
//        m_configItem = (MagicIconConfigItem) ois.readObject();
//        if (null != m_configItem) {
//        	result = true;
//        }
//    } catch (Exception e) {
//        e.printStackTrace();
//    }
	return result;
}

// 把配置item保存到文件
bool LCMagicIconManager::SaveConfigItemToFile()
{
	bool result = false;

//	try {
//		String key = "MagicIconConfigItem_" + WebSiteManager.newInstance(null).GetWebSite().getSiteId();
//		SharedPreferences mSharedPreferences = mContext.getSharedPreferences("base64", Context.MODE_PRIVATE);
//		ByteArrayOutputStream baos = new ByteArrayOutputStream();
//		ObjectOutputStream oos;
//		oos = new ObjectOutputStream(baos);
//		oos.writeObject(m_configItem);
//		String personBase64 = new String(Base64.encode(baos.toByteArray(), Base64.DEFAULT));
//		SharedPreferences.Editor editor = mSharedPreferences.edit();
//		editor.putString(key, personBase64);
//		result = editor.commit();
//	} catch (IOException e) {
//		// TODO Auto-generated catch block
//		e.printStackTrace();
//	}

	return result;
}

// 判断版本是否比当前配置版本新
bool LCMagicIconManager::IsVerNewThenConfigItem(long updateTime) const
{
    bool result = true;
    result = (updateTime != m_configItem.maxupdatetime);
    return result;
    
}

// 更新配置
bool LCMagicIconManager::UpdateConfigItem(const MagicIconConfig& configItem)
{
	// 停止图片文件下载
	StopAllDownloadThumbImage();
	// 删除本地缓存目录下所有文件
	DeleteAllMagicIconFile();
	// 删除所有高级表情item
	RemoveAllMagicIconItem();

	m_configItem = configItem;

	// 保存配置
	SaveConfigItemToFile();
	// 设置下载路径
	SetDownloadPath(m_configItem.path);
	// 添加高级表情
	AddMagicIconWithConfigItem(m_configItem);

	return true;
}

// 获取配置item
MagicIconConfig LCMagicIconManager::GetConfigItem() const
{
	return m_configItem;
}

// ------------------- 小高级表情ID的map表操作 --------------------
// 添加配置item的小高级表情至map
void LCMagicIconManager::AddMagicIconWithConfigItem(const MagicIconConfig& configItem)
{
    for (MagicIconConfig::MagicIconList::const_iterator iter = configItem.magicIconList.begin(); iter != configItem.magicIconList.end(); iter++) {
        if (!(*iter).iconId.empty()) {
            GetMagicIcon((*iter).iconId);
        }
    }
    
}

//// 批量添加高级表情至map
//void LCEmotionManager::AddEmotions(const OtherEmotionConfigItem::EmotionList& emotionList)
//{
//	for (OtherEmotionConfigItem::EmotionList::const_iterator iter = emotionList.begin();
//			iter != emotionList.end();
//			iter++)
//	{
//		if (!(*iter).fileName.empty())
//		{
//			GetEmotion((*iter).fileName);
//		}
//	}
//}

// 添加小高级表情至map
bool LCMagicIconManager::AddMagicIconItem(LCMagicIconItem* item)
{
	bool result = false;
	if (NULL != item && !item->m_magicIconId.empty())
	{
		LockMagicIconMap();

		// add to map
		MagicIconMap::const_iterator magicIconIter = m_magicIconMap.find(item->m_magicIconId);
		if (m_magicIconMap.end() == magicIconIter)
		{
			// 判断image文件是否存在，若存在则赋值
			string imagePath = GetMagicIconImagLocalPath(item->m_magicIconId);
			if (IsFileExist(imagePath))
			{
				item->m_sourcePath = imagePath;
			}

			m_magicIconMap.insert(MagicIconMap::value_type(item->m_magicIconId, item));
			result = true;
		}

		UnlockMagicIconMap();
	}
	return result;
}

// 获取/添加小高级表情item
LCMagicIconItem* LCMagicIconManager::GetMagicIcon(const string& magicIconId)
{
	LCMagicIconItem* item = NULL;

	// 获取已有的高级表情item
	LockMagicIconMap();
	MagicIconMap::const_iterator iter = m_magicIconMap.find(magicIconId);
	if (iter != m_magicIconMap.end()) {
		item = (*iter).second;
	}
	UnlockMagicIconMap();

	// 若不存在则创建
	if (NULL == item) {
		item = new LCMagicIconItem;
		item->Init(magicIconId
				, GetMagicIconThumbLocalPath(magicIconId)
				, GetMagicIconImagLocalPath(magicIconId)
				);
		AddMagicIconItem(item);
	}
	return item;
}

// 清除所有小高级表情item
void LCMagicIconManager::RemoveAllMagicIconItem()
{
	LockMagicIconMap();

	// 删除 LCEmotionItem object
	for (MagicIconMap::iterator iter = m_magicIconMap.begin();
			iter != m_magicIconMap.end();
			iter++)
	{
		delete (*iter).second;
	}
	// 清空map表
	m_magicIconMap.clear();

	UnlockMagicIconMap();
}

// 小高级表情ID map加锁
void LCMagicIconManager::LockMagicIconMap()
{
	if (NULL != m_magicIconMapLock) {
		m_magicIconMapLock->Lock();
	}
}

// 小高级表情ID map解锁
void LCMagicIconManager::UnlockMagicIconMap()
{
	if (NULL != m_magicIconMapLock) {
		m_magicIconMapLock->Unlock();
	}
}

// ------------------- 路径处理（包括URL及本地路径） --------------------
// 获取小高级表情图片目录
string LCMagicIconManager::GetImageDir()
{
	return m_dirPath + g_imgSubPath;
}

string LCMagicIconManager::GetMagicIconImagLocalPath(const string& magicIconId)
{
    return GetImageDir() + magicIconId + g_normalSuffix;
}

string LCMagicIconManager::GetMagicIconThumbLocalPath(const string& magicIconId)
{
    return GetImageDir() + magicIconId + g_thumSuffix;
}

string LCMagicIconManager::GetMagicIconImgUrl(const string& magicIconId)
{
    return m_host + m_downloadPath + g_imgSubPath + magicIconId + g_normalSuffix;
}

string LCMagicIconManager::GetMagicIconThumbUrl(const string& magicIconId)
{
    return m_host + m_downloadPath + g_thumbSubPath + magicIconId + g_thumSuffix;
}

// 删除所有小高级表情文件
void LCMagicIconManager::DeleteAllMagicIconFile()
{
	// 删除image目录文件
	string strImageDir = GetImageDir();
	CleanDir(strImageDir);
}

// ------------------- 小高级表情发送map表操作 --------------------
// 获取并移除待发送的item
LCMessageItem* LCMagicIconManager::GetAndRemoveSendingItem(int msgId)
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
bool LCMagicIconManager::AddSendingItem(LCMessageItem* item)
{
	bool result = false;

	LockToSendMap();

	if (item->m_msgType == LCMessageItem::MT_MagicIcon
		&& NULL != item->GetMagicIconItem()
		&& m_toSendMap.end() == m_toSendMap.find(item->m_msgId))
	{
		m_toSendMap.insert(ToSendMap::value_type(item->m_msgId, item));
		result = true;
	}

	UnlockToSendMap();

	return result;
}

// 清除所有待发送item
void LCMagicIconManager::RemoveAllSendingItems()
{
	LockToSendMap();

	m_toSendMap.clear();

	UnlockToSendMap();
}

// 小高级表情发送map加锁
void LCMagicIconManager::LockToSendMap()
{
	if (NULL != m_toSendMapLock) {
		m_toSendMapLock->Lock();
	}
}

// 小高级表情发送map解锁
void LCMagicIconManager::UnlockToSendMap()
{
	if (NULL != m_toSendMapLock) {
		m_toSendMapLock->Unlock();
	}
}

// ------------ 下载原图image ------------
// 开始下载原图image
bool LCMagicIconManager::StartDownloadSourceImage(LCMagicIconItem* item)
{
	bool result = false;
	LockSourceImgDownloadMap();

	MagicIconDownloaderMap::iterator iter = m_sourceImgDownloadMap.find(item->m_magicIconId);
	if (iter == m_sourceImgDownloadMap.end() && !item->m_magicIconId.empty())
	{
		LCMagicIconDownloader* loader = new LCMagicIconDownloader;
		if (NULL != loader) {
			FileLog("LiveChatManager", "LCMagicIconManager::StartDownloadImage() magicIconId:%s"
					, item->m_magicIconId.c_str());
			result = loader->Start(
					GetMagicIconImgUrl(item->m_magicIconId)
					, GetMagicIconImagLocalPath(item->m_magicIconId)
					, LCMagicIconDownloader::SOURCE
					, item
					, this
					, m_httpUser
					, m_httpPassword);

			FileLog("LiveChatManager", "LCMagicIconManager::StartDownloadImage() magicIconId:%s, result:%d"
					, item->m_magicIconId.c_str(), result);
			if (result) {
				m_sourceImgDownloadMap.insert(MagicIconDownloaderMap::value_type(item->m_magicIconId, loader));
			}
			else {
				delete loader;
			}
		}
	}

	UnlockSourceImgDownloadMap();

	return result;
}

// 停止下载原图image
bool LCMagicIconManager::StopDownloadSourceImage(LCMagicIconItem* item)
{
	bool result = false;
	LCMagicIconDownloader* loader = NULL;

	// 加锁
	LockSourceImgDownloadMap();

	// 找到并移除downloader
	MagicIconDownloaderMap::iterator iter = m_sourceImgDownloadMap.find(item->m_magicIconId);
	if (iter != m_sourceImgDownloadMap.end())
	{
		loader = (*iter).second;
		m_sourceImgDownloadMap.erase(iter);
	}

	// 解锁
	UnlockSourceImgDownloadMap();

	// 释放downloader
	if (NULL != loader)
	{
		loader->Stop();
		delete loader;

		result = true;
	}

	return result;
}

// 停止所有原图image下载
void LCMagicIconManager::StopAllDownloadSourceImage()
{
	// 加锁
	LockSourceImgDownloadMap();

	// 删除所有下载object
	for (MagicIconDownloaderMap::iterator iter = m_sourceImgDownloadMap.begin();
			iter != m_sourceImgDownloadMap.end();
			iter++)
	{
		LCMagicIconDownloader* loader = (*iter).second;
		if (NULL != loader) {
			loader->Stop();
		}
	}
	// 清空map表
	m_sourceImgDownloadMap.clear();

	// 解锁
	UnlockSourceImgDownloadMap();
}

// 下载原图image map加锁
void LCMagicIconManager::LockSourceImgDownloadMap()
{
	if (NULL != m_sourceImgDownloadMapLock) {
		m_sourceImgDownloadMapLock->Lock();
	}
}

// 下载原图image map解锁
void LCMagicIconManager::UnlockSourceImgDownloadMap()
{
	if (NULL != m_sourceImgDownloadMapLock) {
		m_sourceImgDownloadMapLock->Unlock();
	}
}

// 添加downloader到待释放列表(已下载完成，包括成功/失败)
void LCMagicIconManager::AddToFinishDownloaderList(LCMagicIconDownloader* downloader)
{
	m_finishDownloaderList.lock();

	FinishDownloaderItem item;
	item.finishTime = getCurrentTime();
	item.downloader = downloader;
	m_finishDownloaderList.push_back(item);

	m_finishDownloaderList.unlock();
}

// 释放所有待释放Downloader(已下载完成，包括成功/失败)
void LCMagicIconManager::ClearAllDownloader()
{
	m_finishDownloaderList.lock();

	for (FinishDownloaderList::iterator iter = m_finishDownloaderList.begin();
		iter != m_finishDownloaderList.end();
		iter++)
	{
		delete (*iter).downloader;
	}
	m_finishDownloaderList.clear();

	m_finishDownloaderList.unlock();
}

// 清除超过一段时间已下载完成的downloader
void LCMagicIconManager::ClearFinishDownloaderWithTimer()
{
	// 清除间隔时间（秒）
	static const int stepSecond = 1 * 1000;

	m_finishDownloaderList.lock();
	while (true)
	{
		FinishDownloaderList::iterator iter = m_finishDownloaderList.begin();
		if (iter != m_finishDownloaderList.end())
		{
			if (getCurrentTime() - (*iter).finishTime >= stepSecond)
			{
				// 超过限制时间
				delete (*iter).downloader;
				m_finishDownloaderList.erase(iter);
				continue;
			}
			else {
				break;
			}
		}
		else {
			break;
		}
	}
	m_finishDownloaderList.unlock();
}

// ------------ 下载拇子图image ------------
// 开始下载拇子图image
bool LCMagicIconManager::StartDownloadThumbImage(LCMagicIconItem* item)
{
	bool result = false;

	// 加锁
	LockThumbImgDownloadMap();

	// 创建downloader并开始下载
	MagicIconDownloaderMap::iterator iter = m_thumbImgDownloadMap.find(item->m_magicIconId);
	if (iter == m_thumbImgDownloadMap.end() && !item->m_magicIconId.empty())
	{
		LCMagicIconDownloader* loader = new LCMagicIconDownloader;
		if (NULL != loader)
		{
			result = loader->Start(
					GetMagicIconThumbUrl(item->m_magicIconId)
					, GetMagicIconThumbLocalPath(item->m_magicIconId)
					, LCMagicIconDownloader::THUMB
					, item
					, this
					, m_httpUser
					, m_httpPassword);

			if (result) {
				m_thumbImgDownloadMap.insert(MagicIconDownloaderMap::value_type(item->m_magicIconId, loader));
			}
			else {
				delete loader;
			}
		}
	}

	// 解锁
	UnlockThumbImgDownloadMap();
	return result;
}

// 停止下载拇子图image
bool LCMagicIconManager::StopDownloadThumbImage(LCMagicIconItem* item)
{
	bool result = false;
	LCMagicIconDownloader* loader = NULL;

	// 加锁
	LockThumbImgDownloadMap();

	// 找到item并移除
	MagicIconDownloaderMap::iterator iter = m_thumbImgDownloadMap.find(item->m_magicIconId);
	if (iter != m_thumbImgDownloadMap.end())
	{
		loader = (*iter).second;
		m_thumbImgDownloadMap.erase(iter);
	}

	// 解锁
	UnlockThumbImgDownloadMap();

	// 停止并删除object
	if (NULL != loader)
	{
		loader->Stop();
		delete loader;
		result = true;
	}

	return result;
}

// 停止下载所有拇子图image
bool LCMagicIconManager::StopAllDownloadThumbImage()
{
	bool result = false;

	// 加锁
	LockThumbImgDownloadMap();

	for (MagicIconDownloaderMap::iterator iter = m_thumbImgDownloadMap.begin();
			iter != m_thumbImgDownloadMap.end();
			iter++)
	{
		LCMagicIconDownloader* loader = (*iter).second;
		if (NULL != loader) {
			loader->Stop();
			delete loader;
		}
	}
	m_thumbImgDownloadMap.clear();

	// 加锁
	UnlockThumbImgDownloadMap();
	return result;
}

// 下载拇子图 map加锁
void LCMagicIconManager::LockThumbImgDownloadMap()
{
	if (NULL != m_thumbImgDownloadMapLock) {
		m_thumbImgDownloadMapLock->Lock();
	}
}

// 下载拇子图 map解锁
void LCMagicIconManager::UnlockThumbImgDownloadMap()
{
	if (NULL != m_thumbImgDownloadMapLock) {
		m_thumbImgDownloadMapLock->Unlock();
	}
}

// ------------- LCMagicIconDownloaderCallback -------------
void LCMagicIconManager::onSuccess(LCMagicIconDownloader* downloader, LCMagicIconDownloader::MagicIconDownloadType downloadType, LCMagicIconItem* item)
{
	if (NULL != m_callback)
	{
		switch (downloadType)
		{
 
		case LCMagicIconDownloader::THUMB: {
			// 回调
			m_callback->OnDownloadMagicIconThumbImage(true, item);
			// 移除出map
			LockThumbImgDownloadMap();
			m_thumbImgDownloadMap.erase(item->m_magicIconId);
			UnlockThumbImgDownloadMap();
		} break;
		case LCMagicIconDownloader::SOURCE: {
			// 回调
			m_callback->OnDownloadMagicIconImage(true, item);
			// 移除出map
			LockSourceImgDownloadMap();
			m_sourceImgDownloadMap.erase(item->m_magicIconId);
			UnlockSourceImgDownloadMap();
		} break;
		default:
			break;
		}
	}

	AddToFinishDownloaderList(downloader);
}

void LCMagicIconManager::onFail(LCMagicIconDownloader* downloader, LCMagicIconDownloader::MagicIconDownloadType downloadType, LCMagicIconItem* item)
{
	if (NULL != m_callback)
	{
		switch (downloadType)
		{
		case LCMagicIconDownloader::THUMB: {
			// 回调
			m_callback->OnDownloadMagicIconThumbImage(false, item);

			// 移除出map
			LockThumbImgDownloadMap();
			m_thumbImgDownloadMap.erase(item->m_magicIconId);
			UnlockThumbImgDownloadMap();
		} break;
		case LCMagicIconDownloader::SOURCE: {
			// 回调
			m_callback->OnDownloadMagicIconImage(false, item);

			// 移除出map
			LockSourceImgDownloadMap();
			m_sourceImgDownloadMap.erase(item->m_magicIconId);
			UnlockSourceImgDownloadMap();
		} break;
		default:
			break;
		}
	}

	AddToFinishDownloaderList(downloader);
}

