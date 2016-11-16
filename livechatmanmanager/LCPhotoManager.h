/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCPhotoManager.h
 *   desc: LiveChat图片管理类
 */

#pragma once

#include <string>
#include <common/list_lock.h>
#include <map>
#include <common/dualmap_lock.h>
#include <manrequesthandler/RequestLiveChatDefine.h>
#include "LCPhotoDownloader.h"
#include "LCMessageItem.h"
using namespace std;

class HttpRequestManager;
class RequestLiveChatController;
class LCPhotoManagerCallback;
class LCMessageItem;
class LCUserItem;
class IAutoLock;
class LCPhotoItem;
class LCPhotoManager : public LCPhotoDownloaderCallback
{
private:
	typedef map<int, LCMessageItem*>		SendingMap;		// 正在发送消息map表(msgId, LCMessageItem)
	typedef map_lock<long, LCMessageItem*>	RequestMap;		// 正在请求map表(requestId, LCMessageItem)
	typedef dualmap_lock<long, LCPhotoDownloader*>	DownloadMap;	// 正在下载map表(requestId, downloader)
    typedef map_lock<string, LCPhotoItem*>  PhotoMap;       // Photo map表(PhotoID, LCPhotoItem)
    typedef map_lock<string, LCMessageList> PhotoMsgMap;    // PhotoID被哪些MessageItem引用(PhotoID, LCMessageList)

private:
	// 已下载完成的downloader（待释放）
	typedef struct _tagFinishDownloaderItem {
		LCPhotoDownloader*	downloader;
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
	LCPhotoManager();
	virtual ~LCPhotoManager();

public:
	// 初始化
	bool Init(LCPhotoManagerCallback* callback);
	// 设置本地缓存目录
	bool SetDirPath(const string& dirPath);
    // 设置临时本地缓存目录路径
    bool SetTempDirPath(const string& dirPath);
	// 设置http接口参数
	bool SetHttpRequest(HttpRequestManager* requestMgr, RequestLiveChatController* requestController);
	// 获取图片本地缓存文件路径
	string GetPhotoPath(LCMessageItem* item, GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType);
	// 获取图片本地缓存文件路径(全路径)
	string GetPhotoPath(const string& photoId, GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType);
    // 获取图片本地临时缓存文件路径
    string GetPhotoTempPath(LCMessageItem* item, GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType);
    // 获取图片本地临时缓存文件路径(全路径)
    string GetPhotoTempPath(const string& photoId, GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType);
    // 下载完成设置文件路径
	bool SetPhotoFilePath(LCMessageItem* item, GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType);
	// 复制文件至缓冲目录(用于发送图片消息)
	bool CopyPhotoFileToDir(LCPhotoItem* item, const string& srcFilePath);
	// 删除朦胧图片
	bool RemoveFuzzyPhotoFile(LCPhotoItem* item);
	// 清除所有图片
	void RemoveAllPhotoFile();
	// 合并图片消息记录（把女士发出及男士已购买的图片记录合并为一条聊天记录）
	void CombineMessageItem(LCUserItem* userItem);
    
    // --------------------- LCPhotoItem管理 --------------------------
public:
    // ---- PhotoMap管理 ----
    // 获取/生成PhotoItem
    LCPhotoItem* GetPhotoItem(const string& photoId, LCMessageItem* msgItem);
    // 更新/生成PhotoItem(以返回的PhotoItem为准)
    LCPhotoItem* UpdatePhotoItem(LCPhotoItem* photoItem, LCMessageItem* msgItem);
    // 清除PhotoMap
    void ClearPhotoMap();
    // ---- PhotoMsgMap管理 ----
    // 绑定关联
    bool BindPhotoIdWithMsgItem(const string& photoId, LCMessageItem* msgItem);
    // 获取关联的MessageList
    LCMessageList GetMsgListWithBindMap(const string& photoId);
    // 清除关联
    void ClearBindMap();

	// --------------------- sending（正在发送） --------------------------
public:
	// 获取指定票根的item并从待发送map表中移除
	LCMessageItem* GetAndRemoveSendingItem(int msgId);
	// 添加指定票根的item到待发送map表中
	bool AddSendingItem(LCMessageItem* item);
	// 清除所有待发送表map表的item
	void ClearAllSendingItems();

	// --------------------------- Download Photo（正在下载 ） -------------------------
public:
	// 开始下载
	bool DownloadPhoto(
			LCMessageItem* item
			, const string& userId
			, const string& sid
			, GETPHOTO_PHOTOSIZE_TYPE sizeType
			, GETPHOTO_PHOTOMODE_TYPE modeType);
	// 下载完成的回调(IRequestLiveChatControllerCallback)
	void OnGetPhoto(long requestId, bool success, const string& errnum, const string& errmsg, const string& filePath);
	// 清除超过一段时间已下载完成的downloader
	void ClearFinishDownloaderWithTimer();
	// 清除下载
	void ClearAllDownload();
private:
	// 清除所有已下载完成的downloader
	void ClearAllFinishDownloader();
	// LCPhotoDownloaderCallback
	virtual void onSuccess(LCPhotoDownloader* downloader, GETPHOTO_PHOTOSIZE_TYPE sizeType, LCMessageItem* item);
	virtual void onFail(LCPhotoDownloader* downloader, GETPHOTO_PHOTOSIZE_TYPE sizeType, const string& errnum, const string& errmsg, LCMessageItem* item);

	// --------------------------- Request Photo（正在请求） -------------------------
public:
	// 添加到正在请求的map表
	bool AddRequestItem(long requestId, LCMessageItem* item);
	// 获取并移除正在请求的map表
	LCMessageItem* GetAndRemoveRequestItem(long requestId);
	// 获取并清除所有正在的请求
	void ClearAllRequestItems();

private:
	// 获取图片指定类型路径(非全路径)
	string GetPhotoPathWithMode(const string& photoId, GETPHOTO_PHOTOMODE_TYPE modeType);
	// 正在发送消息map表加锁
	void LockSendingMap();
	// 正在发送消息map表解锁
	void UnlockSendingMap();
    
    // --------------------------- Temp Photo Manage（临时图片管理） -------------------------
public:
    // 复制文件到临时目录（返回是否复制成功）
    bool CopyPhotoToTempDir(const string& srcPhotoPath, string& dstPhotoPath);
    // 清除临时目录的图片文件
    void RemoveAllTempPhotoFile();
    
private:
    // 生成临时文件名
    string GetTempPhotoFileName(const string& srcPhotoPath) const;

public:
	LCPhotoManagerCallback*		m_callback;				// callback
	HttpRequestManager*			m_requestMgr;			// http request管理器
	RequestLiveChatController*	m_requestController;	// LiveChat的http request控制器
	SendingMap		m_sendingMap;			// 正在发送消息map表
	IAutoLock*		m_sendingMapLock;		// 正在发送消息map表锁
	RequestMap		m_requestMap;			// 正在请求map表

	DownloadMap		m_downloadMap;			// 正在下载map表
	FinishDownloaderList	m_finishDownloaderList;	// 已完成下载列表（待释放）

	string			m_dirPath;				// 本地缓存目录路径
    string          m_tempDirPath;          // 临时本地缓存目录路径
    
    PhotoMap        m_photoMap;             // Photo map表
    PhotoMsgMap     m_photoBindMap;         // PhotoID被哪些MessageItem引用
};

class LCPhotoManagerCallback
{
public:
	LCPhotoManagerCallback() {};
	virtual ~LCPhotoManagerCallback() {}
public:
	virtual void OnDownloadPhoto(bool success, GETPHOTO_PHOTOSIZE_TYPE sizeType, const string& errnum, const string& errmsg, const LCMessageList& msgList) = 0;
};
