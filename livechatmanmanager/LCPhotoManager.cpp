/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCPhotoManager.h
 *   desc: LiveChat图片管理类
 */

#include "LCPhotoManager.h"
#include "LCMessageItem.h"
#include <HttpRequestDefine.h>
#include <RequestLiveChatDefine.h>
#include <CommonFunc.h>
#include <md5.h>

LCPhotoManager::LCPhotoManager()
{
	m_dirPath = "";
}

LCPhotoManager::~LCPhotoManager()
{

}

// 初始化
bool LCPhotoManager::Init(const string& dirPath)
{
	m_dirPath = dirPath;
	if (!m_dirPath.empty()
		&& m_dirPath.at(m_dirPath.length() -1) != '/'
		&& m_dirPath.at(m_dirPath.length() -1) != '\\')
	{
		m_dirPath += "/";
	}
	return !m_dirPath.empty();
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

// 获取图片临时文件路径
string LCPhotoManager::GetTempPhotoPath(LCMessageItem* item, GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType)
{
	string path = "";
	if (item->m_msgType == LCMessageItem::MT_Photo && NULL != item->GetPhotoItem()
			&& !item->GetPhotoItem()->m_photoId.empty() && !m_dirPath.empty())
	{
		path = GetPhotoPath(item->GetPhotoItem()->m_photoId, modeType, sizeType);
		path += "_temp";
	}
	return path;
}

// 下载完成的临时文件转换成图片文件
bool LCPhotoManager::TempToPhoto(LCMessageItem* item, const string& tempPath, GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType)
{
	bool result = false;
	if (!tempPath.empty()
			&& item->m_msgType == LCMessageItem::MT_Photo && NULL != item->GetPhotoItem())
	{
		LCPhotoItem* photoItem = item->GetPhotoItem();
		string path = GetPhotoPath(photoItem->m_photoId, modeType, sizeType);
		if (!path.empty()) {
			if (RenameFile(tempPath, path))
			{
				switch (photoItem->m_statusType)
				{
				case DownloadShowFuzzyPhoto:
					photoItem->m_showFuzzyFilePath = path;
					break;
				case DownloadThumbFuzzyPhoto:
					photoItem->m_thumbFuzzyFilePath = path;
					break;
				case DownloadShowSrcPhoto:
					photoItem->m_showSrcFilePath = path;
					break;
				case DownloadThumbSrcPhoto:
					photoItem->m_thumbSrcFilePath = path;
					break;
				case DownloadSrcPhoto:
					photoItem->m_srcFilePath = path;
					break;
				default:
					break;
				}
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
		ClearDir(m_dirPath);
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
						womanPhotoList.add(item);
					}
					else if (item->m_sendType == LCMessageItem::SendType_Send) {
						manPhotoList.add(item);
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
							userItem->m_msgList.erase(manItem);
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

// --------------------------- Uploading/Download Photo（正在上传/下载 ） -------------------------
// 获取正在上传/下载的RequestId
long LCPhotoManager::GetRequestIdWithItem(LCMessageItem* item)
{
	long requestId = HTTPREQUEST_INVALIDREQUESTID;

	LockRequestMsgMap();
	RequestMsgMap::iterator iter = m_requestMsgMap.find(item->m_msgId);
	if (iter != m_requestMsgMap.end())
	{
		requestId = (*iter).second;
	}
	UnlockRequestMsgMap();

	return requestId;
}

// 获取并移除正在上传/下载的item
LCMessageItem* LCPhotoManager::GetAndRemoveRequestItem(long requestId)
{
	LCMessageItem* item = NULL;

	LockRequestIdMap();
	RequestIdMap::iterator iter = m_requestIdMap.find(requestId);
	if (iter != m_requestIdMap.end())
	{
		item = (*iter).second;
		m_requestIdMap.erase(iter);
	}
	UnlockRequestIdMap();

	return item;
}

// 添加正在上传/下载的item
bool LCPhotoManager::AddRequestItem(long requestId, LCMessageItem* item)
{
	bool result = false;

	LockRequestIdMap();
	LockRequestMsgMap();

	if (item->m_msgType == LCMessageItem::MT_Photo
			&& NULL != item->GetPhotoItem()
			&& requestId != HTTPREQUEST_INVALIDREQUESTID)
	{
		if (m_requestIdMap.end() == m_requestIdMap.find(requestId))
		{
			m_requestIdMap.insert(RequestIdMap::value_type(requestId, item));
		}

		if (m_requestMsgMap.end() == m_requestMsgMap.find(item->m_msgId))
		{
			m_requestMsgMap.insert(RequestMsgMap::value_type(item->m_msgId, requestId));
		}

		result = true;
	}
	UnlockRequestMsgMap();
	UnlockRequestIdMap();

	return result;
}

// 清除所有正在上传/下载的item
list<long> LCPhotoManager::ClearAllRequestItems()
{
	list<long> requestIdList;
	LockRequestIdMap();
	LockRequestMsgMap();

	for (RequestMsgMap::iterator iter = m_requestMsgMap.begin();
			iter != m_requestMsgMap.end();
			iter++)
	{
		requestIdList.push_back((*iter).second);
	}
	m_requestMsgMap.clear();
	m_requestIdMap.clear();

	UnlockRequestIdMap();
	UnlockRequestMsgMap();
	return requestIdList;
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

// 正在上传/下载map表加锁(requestId, LCMessageItem)
void LCPhotoManager::LockRequestIdMap()
{
	if (NULL != m_requestIdMapLock) {
		m_requestIdMapLock->Lock();
	}
}

// 正在上传/下载map表解锁(requestId, LCMessageItem)
void LCPhotoManager::UnlockRequestIdMap()
{
	if (NULL != m_requestIdMapLock) {
		m_requestIdMapLock->Unlock();
	}
}

// 正在上传/下载map表加锁(msgId, requestId)
void LCPhotoManager::LockRequestMsgMap()
{
	if (NULL != m_requestMsgMapLock) {
		m_requestMsgMapLock->Lock();
	}
}

// 正在上传/下载map表解锁(msgId, requestId)
void LCPhotoManager::UnlockRequestMsgMap()
{
	if (NULL != m_requestMsgMapLock) {
		m_requestMsgMapLock->Unlock();
	}
}

