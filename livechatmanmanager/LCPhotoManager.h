/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCPhotoManager.h
 *   desc: LiveChat图片管理类
 */

#pragma once

#include <string>
#include <list>
#include <map>
using namespace std;

class LCMessageItem;
class IAutoLock;
class LCPhotoItem;
class LCPhotoManager
{
private:
	typedef map<int, LCMessageItem*>	SendingMap;		// 正在发送消息map表(msgId, LCMessageItem)
	typedef map<long, LCMessageItem*>	RequestIdMap;	// 正在上传/下载map表(requestId, LCMessageItem)
	typedef map<int, long>				RequestMsgMap;	// 正在上传/下载map表(msgId, requestId)

public:
	LCPhotoManager();
	virtual ~LCPhotoManager();

public:
	// 初始化
	bool Init(const string& dirPath);
	// 获取图片本地缓存文件路径
	string GetPhotoPath(LCMessageItem* item, GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType);
	// 获取图片本地缓存文件路径(全路径)
	string GetPhotoPath(const string& photoId, GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType);
	// 获取图片临时文件路径
	string GetTempPhotoPath(LCMessageItem* item, GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType);
	// 下载完成的临时文件转换成图片文件
	bool TempToPhoto(LCMessageItem* item, const string& tempPath, GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType);
	// 复制文件至缓冲目录(用于发送图片消息)
	bool CopyPhotoFileToDir(LCPhotoItem* item, const string& srcFilePath);
	// 删除朦胧图片
	bool RemoveFuzzyPhotoFile(LCPhotoItem* item);
	// 清除所有图片
	void RemoveAllPhotoFile();
	// 合并图片消息记录（把女士发出及男士已购买的图片记录合并为一条聊天记录）
	void CombineMessageItem(LCUserItem* userItem);

	// --------------------- sending（正在发送） --------------------------
public:
	// 获取指定票根的item并从待发送map表中移除
	LCMessageItem* GetAndRemoveSendingItem(int msgId);
	// 添加指定票根的item到待发送map表中
	bool AddSendingItem(LCMessageItem* item);
	// 清除所有待发送表map表的item
	void ClearAllSendingItems();

	// --------------------------- Uploading/Download Photo（正在上传/下载 ） -------------------------
public:
	// 获取正在上传/下载的RequestId
	long GetRequestIdWithItem(LCMessageItem* item);
	// 获取并移除正在上传/下载的item
	LCMessageItem* GetAndRemoveRequestItem(long requestId);
	// 添加正在上传/下载的item
	bool AddRequestItem(long requestId, LCMessageItem* item);
	// 清除所有正在上传/下载的item
	list<long> ClearAllRequestItems();

private:
	// 获取图片指定类型路径(非全路径)
	string GetPhotoPathWithMode(const string& photoId, GETPHOTO_PHOTOMODE_TYPE modeType);
	// 正在发送消息map表加锁
	void LockSendingMap();
	// 正在发送消息map表解锁
	void UnlockSendingMap();
	// 正在上传/下载map表加锁(requestId, LCMessageItem)
	void LockRequestIdMap();
	// 正在上传/下载map表解锁(requestId, LCMessageItem)
	void UnlockRequestIdMap();
	// 正在上传/下载map表加锁(msgId, requestId)
	void LockRequestMsgMap();
	// 正在上传/下载map表解锁(msgId, requestId)
	void UnlockRequestMsgMap();

public:
	SendingMap		m_sendingMap;			// 正在发送消息map表
	IAutoLock*		m_sendingMapLock;		// 正在发送消息map表锁
	RequestIdMap	m_requestIdMap;			// 正在上传/下载map表(requestId, LCMessageItem)
	IAutoLock*		m_requestIdMapLock;		// 正在上传/下载map表锁(requestId, LCMessageItem)
	RequestMsgMap	m_requestMsgMap;		// 正在上传/下载map表(msgId, requestId)
	IAutoLock*		m_requestMsgMapLock;	// 正在上传/下载map表锁(msgId, requestId)

	string			m_dirPath;				// 本地缓存目录路径
};
