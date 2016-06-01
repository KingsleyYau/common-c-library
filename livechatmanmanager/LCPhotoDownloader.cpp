/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCPhotoDownloader.h
 *   desc: LiveChat图片下载类
 */

#include "LCPhotoDownloader.h"
#include <httpclient/HttpRequestDefine.h>
#include "LCMessageItem.h"
#include "LCPhotoManager.h"
#include "LCPhotoItem.h"
#include "LCUserItem.h"
#include <common/CheckMemoryLeak.h>

LCPhotoDownloader::LCPhotoDownloader()
{
	m_photoMgr = NULL;
	m_requestMgr = NULL;
	m_requestController = NULL;
	m_callback = NULL;
	m_requestId = HTTPREQUEST_INVALIDREQUESTID;
	m_item = NULL;
	m_filePath = "";
}

LCPhotoDownloader::~LCPhotoDownloader()
{
	Stop();
}

bool LCPhotoDownloader::Init(HttpRequestManager* requestMgr
							, RequestLiveChatController* requestController
							, LCPhotoManager* photoMgr)
{
	bool result = false;
	if (NULL != photoMgr
		&& NULL != requestMgr
		&& NULL != requestController)
	{
		m_photoMgr = photoMgr;
		m_requestMgr = requestMgr;
		m_requestController = requestController;

		result = true;
	}

	FileLog("LiveChatManager", "LCPhotoDownloader::Init() requestMgr:%p, requestController:%p, photoMgr:%p, result:%d"
			, requestMgr, requestController, photoMgr, result);

	return result;
}

bool LCPhotoDownloader::StartGetPhoto(
			LCPhotoDownloaderCallback* callback
			, LCMessageItem* item
			, const string& userId
			, const string& sid
			, GETPHOTO_PHOTOSIZE_TYPE sizeType
			, GETPHOTO_PHOTOMODE_TYPE modeType)
{
	bool result = false;
	if (NULL != m_photoMgr
		&& NULL != m_requestMgr
		&& NULL != m_requestController
		&& NULL != callback
		&& NULL != item
		&& NULL != item->GetUserItem()
		&& !item->GetUserItem()->m_userId.empty()
		&& !userId.empty()
		&& !sid.empty())
	{
		string filePath = m_photoMgr->GetPhotoPath(item, modeType, sizeType);
		GETPHOTO_TOFLAG_TYPE toFlag = (LCMessageItem::SendType_Recv == item->m_sendType ? GTT_MANGETWOMAN : GTT_MANGETSELF);
		m_requestId = m_requestController->GetPhoto(toFlag, item->GetUserItem()->m_userId, userId, sid, item->GetPhotoItem()->m_photoId, sizeType, modeType, filePath);
		result = (HTTPREQUEST_INVALIDREQUESTID != m_requestId);

		if (result)
		{
			m_callback = callback;
			m_item = item;
			m_sizeType = sizeType;
			m_modeType = modeType;
			m_filePath = filePath;
		}

		FileLog("LiveChatManager", "LCPhotoDownloader::StartGetPhoto() requestId:%d, result:%d", m_requestId, result);
	}
	else {
		FileLog("LiveChatManager", "LCPhotoDownloader::StartGetPhoto() param error!");
	}

	return result;
}

bool LCPhotoDownloader::Stop()
{
	bool result = false;
	if (NULL != m_requestMgr) {
		result = m_requestMgr->StopRequest(m_requestId, true);
	}
	return result;
}

long LCPhotoDownloader::GetRequestId() const
{
	return m_requestId;
}

void LCPhotoDownloader::OnGetPhoto(long requestId, bool success, const string& errnum, const string& errmsg, const string& filePath)
{
	FileLog("LiveChatManager", "LCPhotoDownloader::OnGetPhoto() requestId:%d, m_requestId:%d, success:%d, errnum:%s, errmsg:%s, filePath:%s"
			, requestId, m_requestId, success, errnum.c_str(), errmsg.c_str(), filePath.c_str());

	if (m_requestId == requestId)
	{
		// 重置下载状态
		m_item->GetPhotoItem()->RemoveProcessStatus(m_modeType, m_sizeType);

		if (success) {
			// 获取图片成功
			if (m_photoMgr->SetPhotoFilePath(m_item, m_modeType, m_sizeType))
			{
				// 设置文件路径成功
				m_item->m_statusType = LCMessageItem::StatusType_Finish;
				m_callback->onSuccess(this, m_item);

				FileLog("LiveChatManager", "LCPhotoDownloader::OnGetPhoto() SetPhotoFilePath ok");
			}
			else
			{
				// 设置文件路径失败
				m_item->m_statusType = LCMessageItem::StatusType_Fail;
				m_callback->onFail(this, errnum, errmsg, m_item);

				FileLog("LiveChatManager", "LCPhotoDownloader::OnGetPhoto() SetPhotoFilePath fail");
			}
		}
		else {
			// 获取图片失败
			m_item->m_statusType = LCMessageItem::StatusType_Fail;
			m_callback->onFail(this, errnum, errmsg, m_item);
		}
	}
}
