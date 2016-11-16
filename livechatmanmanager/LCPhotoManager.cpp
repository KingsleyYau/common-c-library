/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCPhotoManager.h
 *   desc: LiveChat图片管理类
 */

#include "LCPhotoManager.h"
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
    m_tempDirPath = "";

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

// 设置本地缓存目录路径
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
		}
        
        // 创建目录
        result = MakeDir(m_dirPath);
	}
	return result;
}

// 设置临时本地缓存目录路径
bool LCPhotoManager::SetTempDirPath(const string& dirPath)
{
    bool result = false;
    
    if (!dirPath.empty())
    {
        m_tempDirPath = dirPath;
        if (m_tempDirPath.at(m_tempDirPath.length() -1) != '/'
            && m_tempDirPath.at(m_tempDirPath.length() -1) != '\\')
        {
            m_tempDirPath += "/";
        }
        
        // 创建目录
        result = MakeDir(m_tempDirPath);
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

// 获取图片本地临时缓存文件路径
string LCPhotoManager::GetPhotoTempPath(LCMessageItem* item, GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType)
{
    string path = "";
    if (item->m_msgType == LCMessageItem::MT_Photo && NULL != item->GetPhotoItem()
        && !item->GetPhotoItem()->m_photoId.empty() && !m_dirPath.empty())
    {
        path = GetPhotoTempPath(item->GetPhotoItem()->m_photoId, modeType, sizeType);
    }
    return path;
}

// 获取图片本地临时缓存文件路径(全路径)
string LCPhotoManager::GetPhotoTempPath(const string& photoId, GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType)
{
    string path = "";
    string photoPath = GetPhotoPath(photoId, modeType, sizeType);
    if (!photoPath.empty()) {
        path = photoPath + ".tmp";
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

// --------------------- LCPhotoItem管理 --------------------------
// ---- PhotoMap管理 ----
// 获取/生成PhotoItem
LCPhotoItem* LCPhotoManager::GetPhotoItem(const string& photoId, LCMessageItem* msgItem)
{
    LCPhotoItem* item = NULL;
    m_photoMap.lock();
    PhotoMap::iterator iter = m_photoMap.find(photoId);
    if (iter != m_photoMap.end()) {
        item = (*iter).second;
    }
    else {
        item = new LCPhotoItem;
        if (NULL != item) {
            item->Init(photoId, "", "", "", "", "", "", "", false);
            m_photoMap.insert(PhotoMap::value_type(photoId, item));
        }
    }
    m_photoMap.unlock();
    
    if (NULL != item && NULL != msgItem) {
        // 设置MessageItem的PhotoItem
        msgItem->SetPhotoItem(item);
        // 添加绑定关系
        BindPhotoIdWithMsgItem(item->m_photoId, msgItem);
    }
    
    return item;
}

// 更新/生成PhotoItem(以返回的PhotoItem为准)
LCPhotoItem* LCPhotoManager::UpdatePhotoItem(LCPhotoItem* photoItem, LCMessageItem* msgItem)
{
    LCPhotoItem* item = NULL;
    m_photoMap.lock();
    PhotoMap::iterator iter = m_photoMap.find(photoItem->m_photoId);
    if (iter != m_photoMap.end()) {
        item = (*iter).second;
        item->Update(photoItem);
        
        delete photoItem;
    }
    else {
        m_photoMap.insert(PhotoMap::value_type(photoItem->m_photoId, photoItem));
        item = photoItem;
    }
    
    if (NULL != item) {
        msgItem->SetPhotoItem(item);
    }
    m_photoMap.unlock();
    
    return item;
}

// 清除PhotoMap
void LCPhotoManager::ClearPhotoMap()
{
    m_photoMap.lock();
    for (PhotoMap::iterator iter = m_photoMap.begin();
         iter != m_photoMap.end();
         iter++)
    {
        delete (*iter).second;
    }
    m_photoMap.clear();
    m_photoMap.unlock();
}

// ---- PhotoMsgMap管理 ----
// 绑定关联
bool LCPhotoManager::BindPhotoIdWithMsgItem(const string& photoId, LCMessageItem* msgItem)
{
    bool result = false;
    
    // 把MessageItem添加至引用列表
    m_photoBindMap.lock();
    PhotoMsgMap::iterator photoIter = m_photoBindMap.find(photoId);
    if (photoIter != m_photoBindMap.end()) {
        // photoId不存在，更新列表
        bool isExist = false;
        for (LCMessageList::iterator msgIter = (*photoIter).second.begin();
             msgIter != (*photoIter).second.end();
             msgIter++)
        {
            if ((*msgIter) == msgItem) {
                isExist = true;
                break;
            }
        }
        
        // MessageItem不存在于列表则添加
        if (!isExist) {
            (*photoIter).second.push_back(msgItem);
        }
        
        result = isExist;
    }
    else {
        // photoId不存在，插入列表
        LCMessageList msgList;
        msgList.push_back(msgItem);
        m_photoBindMap.insert(PhotoMsgMap::value_type(photoId, msgList));
        
        result = true;
    }
    m_photoBindMap.unlock();
    
    return result;
}

// 获取关联的MessageList
LCMessageList LCPhotoManager::GetMsgListWithBindMap(const string& photoId)
{
    LCMessageList msgList;
    
    m_photoBindMap.lock();
    PhotoMsgMap::const_iterator photoIter = m_photoBindMap.find(photoId);
    if (photoIter != m_photoBindMap.end()) {
        msgList = (*photoIter).second;
    }
    m_photoBindMap.unlock();
    
    return msgList;
}

// 清除关联
void LCPhotoManager::ClearBindMap()
{
    m_photoBindMap.lock();
    m_photoBindMap.clear();
    m_photoBindMap.unlock();
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
		FileLog("LiveChatManager", "LCPhotoManager::DownloadPhoto() IsProcessStatus result:%d, photoId:%s", result, item->GetPhotoItem()->m_photoId.c_str());
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

void LCPhotoManager::onSuccess(LCPhotoDownloader* downloader, GETPHOTO_PHOTOSIZE_TYPE sizeType, LCMessageItem* item)
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
    
    // 获取photoID关联的MessageList
    LCMessageList msgList;
    if (NULL != item && NULL != item->GetPhotoItem()) {
        msgList = GetMsgListWithBindMap(item->GetPhotoItem()->m_photoId);
    }

	// 回调
	if (NULL != m_callback) {
		m_callback->OnDownloadPhoto(true, sizeType, "", "", msgList);
	}
}

void LCPhotoManager::onFail(LCPhotoDownloader* downloader, GETPHOTO_PHOTOSIZE_TYPE sizeType, const string& errnum, const string& errmsg, LCMessageItem* item)
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
    
    // 获取photoID关联的MessageList
    LCMessageList msgList;
    if (NULL != item && NULL != item->GetPhotoItem()) {
        msgList = GetMsgListWithBindMap(item->GetPhotoItem()->m_photoId);
    }

	// 回调
	if (NULL != m_callback) {
		m_callback->OnDownloadPhoto(false, sizeType, errnum, errmsg, msgList);
	}
}

// --------------------------- Request Photo（正在请求） -------------------------
// 添加到正在请求的map表
bool LCPhotoManager::AddRequestItem(long requestId, LCMessageItem* item)
{
	bool result = false;
	if (NULL != item
		&& LCMessageItem::MT_Photo == item->m_msgType
		&& NULL != item->GetPhotoItem())
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
void LCPhotoManager::ClearAllRequestItems()
{
	m_requestMap.lock();
	for (RequestMap::const_iterator iter = m_requestMap.begin();
		iter != m_requestMap.end();
		iter++)
	{
        long requestId = (*iter).first;
        m_requestMgr->StopRequest(requestId, true);
	}
    m_requestMap.clear();
	m_requestMap.unlock();
}

// --------------------------- Temp Photo Manage（临时图片管理） -------------------------
// 复制文件到临时目录（返回是否复制成功）
bool LCPhotoManager::CopyPhotoToTempDir(const string& srcPhotoPath, string& dstPhotoPath)
{
    bool result = false;
    
    if (!m_tempDirPath.empty()) {
        // 获取临时文件名
        string fileName = GetTempPhotoFileName(srcPhotoPath);
        if (!fileName.empty()) {
            // 复制文件至临时文件目录
            string filePath = m_tempDirPath + fileName;
            result = CopyFile(srcPhotoPath, filePath);
            
            // 赋值
            if (result) {
                dstPhotoPath = filePath;
            }
        }
    }
    
    return result;
}

// 清除临时目录的图片文件
void LCPhotoManager::RemoveAllTempPhotoFile()
{
    if (!m_tempDirPath.empty())
    {
        CleanDir(m_tempDirPath);
    }
}

// 生成临时文件名
string LCPhotoManager::GetTempPhotoFileName(const string& srcPhotoPath) const
{
    string fileName = "";
    
    if (!srcPhotoPath.empty())
    {
        char md5[128] = {0};
        GetFileMD5String(srcPhotoPath.c_str(), md5);
        fileName = md5;
    }
    
    return fileName;
}
