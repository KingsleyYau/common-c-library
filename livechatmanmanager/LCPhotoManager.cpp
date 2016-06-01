/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCPhotoManager.h
 *   desc: LiveChat图片管理类
 */

#include "LCPhotoManager.h"
#include "LCMessageItem.h"
#include "LCUserItem.h"
#include <httpclient/HttpRequestDefine.h>
#include <manrequesthandler/RequestLiveChatDefine.h>
#include <common/CommonFunc.h>
#include <common/md5.h>
#include <common/IAutoLock.h>
#include <common/KLog.h>
#include <common/CheckMemoryLeak.h>

LCPhotoManager::LCPhotoManager()
{
	m_requestMgr = NULL;
	m_requestController = NULL;

	m_dirPath = "";

	m_sendingMapLock = IAutoLock::CreateAutoLock();
	if (NULL != m_sendingMapLock) {
		m_sendingMapLock->Init();
	}
}

LCPhotoManager::~LCPhotoManager()
{
	IAutoLock::ReleaseAutoLock(m_sendingMapLock);

	ClearAllDownload();
}

// 初始化
bool LCPhotoManager::Init(LCPhotoManagerCallback* callback)
{
	bool result = false;
	if (NULL != callback)
	{
		m_callback = callback;
		result = true;
	}
	return result;
}

// 初始化
bool LCPhotoManager::SetDirPath(const string& dirPath)
{
	bool result = false;

	if (!dirPath.empty())
	{
		m_dirPath = dirPath;
		if (m_dirPath.at(m_dirPath.length() -1) != '/'
			&& m_dirPath.at(m_dirPath.length() -1) != '\\')
		{
			m_dirPath += "/";

			// 创建目录
			result = MakeDir(m_dirPath);
		}
	}
	return result;
}

// 设置http接口参数
bool LCPhotoManager::SetHttpRequest(HttpRequestManager* requestMgr, RequestLiveChatController* requestController)
{
	bool result = false;
	if (NULL != requestMgr
		&& NULL != requestController)
	{
		m_requestMgr = requestMgr;
		m_requestController = requestController;
		result = true;
	}
	return result;
}

// 获取图片本地缓存文件路径
string LCPhotoManager::GetPhotoPath(LCMessageItem* item, GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType)
{
	string path = "";
	if (item->m_msgType == LCMessageItem::MT_Photo && NULL != item->GetPhotoItem()
			&& !item->GetPhotoItem()->m_photoId.empty() && !m_dirPath.empty())
	{
		path = GetPhotoPath(item->GetPhotoItem()->m_photoId, modeType, sizeType);
	}
	return path;
}

// 获取图片本地缓存文件路径(全路径)
string LCPhotoManager::GetPhotoPath(const string& photoId, GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType)
{
	string path = "";
	if (!photoId.empty())
	{
		path = GetPhotoPathWithMode(photoId, modeType);
		path += "_";
		path += GETPHOTO_PHOTOSIZE_PROTOCOL[sizeType];
	}
	return path;
}

// 获取图片指定类型路径(非全路径)
string LCPhotoManager::GetPhotoPathWithMode(const string& photoId, GETPHOTO_PHOTOMODE_TYPE modeType)
{
	string path = "";
	if (!photoId.empty())
	{
		char cModeType[32] = {0};
		snprintf(cModeType, sizeof(cModeType), "%d", modeType);

		char cFileName[128] = {0};
		GetMD5String(photoId.c_str(), cFileName);

		path = m_dirPath;
		path += cFileName;
		path += "_";
		path += cModeType;
	}
	return path;
}

// 下载完成设置文件路径
bool LCPhotoManager::SetPhotoFilePath(LCMessageItem* item, GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType)
{
	bool result = false;
	if (item->m_msgType == LCMessageItem::MT_Photo && NULL != item->GetPhotoItem())
	{
		LCPhotoItem* photoItem = item->GetPhotoItem();
		string path = GetPhotoPath(photoItem->m_photoId, modeType, sizeType);
		if (IsFileExist(path))
		{
			result = true;
			LCPhotoItem::ProcessStatus status = LCPhotoItem::GetProcessStatus(modeType, sizeType);
			switch (status)
			{
			case LCPhotoItem::DownloadShowFuzzyPhoto:
				photoItem->m_showFuzzyFilePath = path;
				break;
			case LCPhotoItem::DownloadThumbFuzzyPhoto:
				photoItem->m_thumbFuzzyFilePath = path;
				break;
			case LCPhotoItem::DownloadShowSrcPhoto:
				photoItem->m_showSrcFilePath = path;
				break;
			case LCPhotoItem::DownloadThumbSrcPhoto:
				photoItem->m_thumbSrcFilePath = path;
				break;
			case LCPhotoItem::DownloadSrcPhoto:
				photoItem->m_srcFilePath = path;
				break;
			default:
				result = false;
				break;
			}
		}
	}
	return result;
}

// 复制文件至缓冲目录(用于发送图片消息)
bool LCPhotoManager::CopyPhotoFileToDir(LCPhotoItem* item, const string& srcFilePath)
{
	bool result = false;
	if (IsFileExist(srcFilePath))
	{
		string desFilePath = GetPhotoPath(item->m_photoId, GMT_CLEAR, GPT_ORIGINAL);
		if (CopyFile(srcFilePath, desFilePath))
		{
			// 原图路径
			item->m_srcFilePath = desFilePath;
			result = true;
		}
	}
	return result;
}

// 删除朦胧图片
bool LCPhotoManager::RemoveFuzzyPhotoFile(LCPhotoItem* item)
{
	bool result = false;

	if (!item->m_photoId.empty())
	{
		// 大图
		string largeFile = GetPhotoPath(item->m_photoId, GMT_FUZZY, GPT_LARGE);
		RemoveFile(largeFile);

		// 中图
		string middleFile = GetPhotoPath(item->m_photoId, GMT_FUZZY, GPT_MIDDLE);
		RemoveFile(middleFile);

		// 小图
		string smallFile = GetPhotoPath(item->m_photoId, GMT_FUZZY, GPT_SMALL);
		RemoveFile(smallFile);

		// 原图
		string originalFile = GetPhotoPath(item->m_photoId, GMT_FUZZY, GPT_ORIGINAL);
		RemoveFile(originalFile);

		result = true;
	}

	return result;
}

// 清除所有图片
void LCPhotoManager::RemoveAllPhotoFile()
{
	if (!m_dirPath.empty())
	{
		CleanDir(m_dirPath);
	}
}

// 合并图片消息记录（把女士发出及男士已购买的图片记录合并为一条聊天记录）
void LCPhotoManager::CombineMessageItem(LCUserItem* userItem)
{
	if (NULL != userItem)
	{
		userItem->LockMsgList();
		if (!userItem->m_msgList.empty())
		{
			// 女士发送图片列表
			LCMessageList womanPhotoList;
			// 男士发送图片列表
			LCMessageList manPhotoList;
			// 找出所有男士和女士发出的图片消息
			for (LCMessageList::iterator iter = userItem->m_msgList.begin();
					iter != userItem->m_msgList.end();
					iter++)
			{
				LCMessageItem* item = (*iter);
				if (item->m_msgType == LCMessageItem::MT_Photo
					&& NULL != item->GetPhotoItem()
					&& !item->GetPhotoItem()->m_photoId.empty())
				{
					if (item->m_sendType == LCMessageItem::SendType_Recv) {
						womanPhotoList.push_back(item);
					}
					else if (item->m_sendType == LCMessageItem::SendType_Send) {
						manPhotoList.push_back(item);
					}
				}
			}

			// 合并男士购买的图片消息
			if (!manPhotoList.empty() && !womanPhotoList.empty())
			{
				for (LCMessageList::iterator manIter = manPhotoList.begin();
						manIter != manPhotoList.end();
						manIter++)
				{
					LCMessageItem* manItem = (*manIter);
					for (LCMessageList::iterator womanIter = womanPhotoList.begin();
							womanIter != womanPhotoList.end();
							womanIter++)
					{
						LCMessageItem* womanItem = (*womanIter);
						LCPhotoItem* manPhotoItem = manItem->GetPhotoItem();
						LCPhotoItem* womanPhotoItem = womanItem->GetPhotoItem();
						if (manPhotoItem->m_photoId == womanPhotoItem->m_photoId
							&& manPhotoItem->m_sendId == womanPhotoItem->m_sendId)
						{
							// 男士发出的图片ID与女士发出的图片ID一致，需要合并
							userItem->m_msgList.remove(manItem);
							womanPhotoItem->m_charge = true;
						}
					}
				}
			}
		}
		userItem->UnlockMsgList();
	}
}

// --------------------- sending（正在发送） --------------------------
// 获取指定票根的item并从待发送map表中移除
LCMessageItem* LCPhotoManager::GetAndRemoveSendingItem(int msgId)
{
	LCMessageItem* item = NULL;

	LockSendingMap();
	SendingMap::iterator iter = m_sendingMap.find(msgId);
	if (iter != m_sendingMap.end())
	{
		item = (*iter).second;
		m_sendingMap.erase(iter);
	}
	UnlockSendingMap();

	return item;
}

// 添加指定票根的item到待发送map表中
bool LCPhotoManager::AddSendingItem(LCMessageItem* item)
{
	bool result = false;
	LockSendingMap();
	if (NULL != item
			&& item->m_msgType == LCMessageItem::MT_Photo
			&& NULL != item->GetPhotoItem()
			&& m_sendingMap.end() == m_sendingMap.find(item->m_msgId))
	{
		m_sendingMap.insert(SendingMap::value_type(item->m_msgId, item));
		result = true;
	}
	UnlockSendingMap();
	return result;
}

// 清除所有待发送表map表的item
void LCPhotoManager::ClearAllSendingItems()
{
	LockSendingMap();
	m_sendingMap.clear();
	UnlockSendingMap();
}

// 正在发送消息map表加锁
void LCPhotoManager::LockSendingMap()
{
	if (NULL != m_sendingMapLock) {
		m_sendingMapLock->Lock();
	}
}

// 正在发送消息map表解锁
void LCPhotoManager::UnlockSendingMap()
{
	if (NULL != m_sendingMapLock) {
		m_sendingMapLock->Unlock();
	}
}

// --------------------------- Download Photo（正在下载 ） -------------------------
// 下载图片
bool LCPhotoManager::DownloadPhoto(
			LCMessageItem* item
			, const string& userId
			, const string& sid
			, GETPHOTO_PHOTOSIZE_TYPE sizeType
			, GETPHOTO_PHOTOMODE_TYPE modeType)
{
	bool result = false;

	FileLog("LiveChatManager", "LCPhotoManager::DownloadPhoto() begin!");

	if (NULL != item
		&& NULL != item->GetUserItem()
		&& !item->GetUserItem()->m_userId.empty()
		&& LCMessageItem::MT_Photo == item->m_msgType
		&& NULL != item->GetPhotoItem()
		&& !item->GetPhotoItem()->m_photoId.empty()
		&& !userId.empty()
		&& !sid.empty())
	{
		result = item->GetPhotoItem()->IsProcessStatus(modeType, sizeType);
		FileLog("LiveChatManager", "LCPhotoManager::DownloadPhoto() IsProcessStatus result:%d", result);
		if (!result)
		{
			// 还未处理
			LCPhotoDownloader* downloader = new LCPhotoDownloader;
			if (NULL != downloader)
			{
				result = downloader->Init(m_requestMgr, m_requestController, this);
				FileLog("LiveChatManager", "LCPhotoManager::DownloadPhoto() downloader->Init result:%d", result);
				result = result && downloader->StartGetPhoto(
										this
										, item
										, userId
										, sid
										, sizeType
										, modeType);

				FileLog("LiveChatManager", "LCPhotoManager::DownloadPhoto() downloader->StartGetPhoto result:%d", result);
				if (!result) {
					// 下载启动失败
					delete downloader;
				}
				else {
					bool insertResult = false;
					// 下载启动成功
					m_downloadMap.lock();
					insertResult = m_downloadMap.insertItem(downloader->GetRequestId(), downloader);
					m_downloadMap.unlock();

					FileLog("LiveChatManager", "LCPhotoManager::DownloadPhoto() insertItem requestId:%d, msgId:%d, downloader:%p, insertResult:%d"
							, downloader->GetRequestId(), item->m_msgId, downloader, insertResult);
				}
			}
		}
	}
	else {
		FileLog("LiveChatManager", "LCPhotoManager::DownloadPhoto() param error!");
	}

	FileLog("LiveChatManager", "LCPhotoManager::DownloadPhoto() end");
	return result;
}

// 下载完成的回调(IRequestLiveChatControllerCallback)
void LCPhotoManager::OnGetPhoto(long requestId, bool success, const string& errnum, const string& errmsg, const string& filePath)
{
	FileLog("LiveChatManager", "LCPhotoManager::OnGetPhoto() begin, requestId:%d, success:%d, errnum:%s, errmsg:%s, filePath:%s"
			, requestId, success, errnum.c_str(), errmsg.c_str(), filePath.c_str());

	LCPhotoDownloader* downloader = NULL;
	bool isFind = false;

	// 找出对应的downloader
	m_downloadMap.lock();
	isFind = m_downloadMap.findWithKey(requestId, downloader);
	m_downloadMap.unlock();

	// 回调到downloader
	if (isFind) {
		downloader->OnGetPhoto(requestId, success, errnum, errmsg, filePath);
		FileLog("LiveChatManager", "LCPhotoManager::OnGetPhoto() downloader->OnGetPhoto() ok, requestId:%d", requestId);
	}

	FileLog("LiveChatManager", "LCPhotoManager::OnGetPhoto() end, isFind:%d, requestId:%d, success:%d, errnum:%s, errmsg:%s, filePath:%s"
			, isFind, requestId, success, errnum.c_str(), errmsg.c_str(), filePath.c_str());
}

// 清除超过一段时间已下载完成的downloader
void LCPhotoManager::ClearFinishDownloaderWithTimer()
{
	// 清除间隔时间（1秒）
	static const int stepSecond = 1 * 1000;

	m_finishDownloaderList.lock();
	for (FinishDownloaderList::iterator iter = m_finishDownloaderList.begin();
		iter != m_finishDownloaderList.end();
		iter = m_finishDownloaderList.begin())
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
	m_finishDownloaderList.unlock();
}

// 清除下载
void LCPhotoManager::ClearAllDownload()
{
	// 获取downloader并清空map
	list<LCPhotoDownloader*> downloadList;
	m_downloadMap.lock();
	downloadList = m_downloadMap.getValueList();
	m_downloadMap.clear();
	m_downloadMap.unlock();

	// 停止正在下载的downloader
	for (list<LCPhotoDownloader*>::const_iterator iter = downloadList.begin();
		iter != downloadList.end();
		iter++)
	{
		// 停止下载（停止成功会回调到onFail()或onSuccess()，并移到已完成队列）
		(*iter)->Stop();
	}

	// 释放已完成队列
	ClearAllFinishDownloader();
}

// 清除所有已下载完成的downloader
void LCPhotoManager::ClearAllFinishDownloader()
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

void LCPhotoManager::onSuccess(LCPhotoDownloader* downloader, LCMessageItem* item)
{
	// downloader添加到释放列表
	FinishDownloaderItem finishItem;
	finishItem.downloader = downloader;
	finishItem.finishTime = getCurrentTime();
	m_finishDownloaderList.lock();
	m_finishDownloaderList.push_back(finishItem);
	m_finishDownloaderList.unlock();

	// 移除downloaderMap
	m_downloadMap.lock();
	m_downloadMap.eraseWithValue(downloader);
	m_downloadMap.unlock();

	// 回调
	if (NULL != m_callback) {
		m_callback->OnDownloadPhoto(true, "", "", item);
	}
}

void LCPhotoManager::onFail(LCPhotoDownloader* downloader, const string& errnum, const string& errmsg, LCMessageItem* item)
{
	// downloader添加到释放列表
	FinishDownloaderItem finishItem;
	finishItem.downloader = downloader;
	finishItem.finishTime = getCurrentTime();
	m_finishDownloaderList.lock();
	m_finishDownloaderList.push_back(finishItem);
	m_finishDownloaderList.unlock();

	// 移除downloaderMap
	m_downloadMap.lock();
	m_downloadMap.eraseWithValue(downloader);
	m_downloadMap.unlock();

	// 回调
	if (NULL != m_callback) {
		m_callback->OnDownloadPhoto(false, errnum, errmsg, item);
	}
}

// --------------------------- Request Photo（正在请求） -------------------------
// 添加到正在请求的map表
bool LCPhotoManager::AddRequestItem(long requestId, LCMessageItem* item)
{
	bool result = false;
	if (NULL != item
		&& LCMessageItem::MT_Photo == item->m_msgType
		&& NULL != item->GetPhotoItem()
		&& LCMessageItem::StatusType_Processing == item->m_statusType)
	{
		m_requestMap.lock();
		m_requestMap.insert(RequestMap::value_type(requestId, item));
		m_requestMap.unlock();
		result = true;
	}
	return result;
}

// 获取并移除正在请求的map表
LCMessageItem* LCPhotoManager::GetAndRemoveRequestItem(long requestId)
{
	LCMessageItem* item = NULL;
	m_requestMap.lock();
	RequestMap::iterator iter = m_requestMap.find(requestId);
	if (m_requestMap.end() != iter) {
		item = (*iter).second;
	}
	m_requestMap.unlock();
	return item;
}

// 获取并清除所有正在的请求
list<long> LCPhotoManager::ClearAllRequestItems()
{
	list<long> result;
	m_requestMap.lock();
	for (RequestMap::const_iterator iter = m_requestMap.begin();
		iter != m_requestMap.end();
		iter++)
	{
		result.push_back((*iter).first);
	}
	m_requestMap.unlock();
	return result;
}
