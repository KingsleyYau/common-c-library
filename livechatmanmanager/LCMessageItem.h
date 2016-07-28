/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCMessageItem.h
 *   desc: LiveChat消息item
 */

#pragma once

#include "LCTextItem.h"
#include "LCWarningItem.h"
#include "LCEmotionItem.h"
#include "LCVoiceItem.h"
#include "LCPhotoItem.h"
#include "LCVideoItem.h"
#include "LCSystemItem.h"
#include "LCCustomItem.h"
#include <livechat/ILiveChatClient.h>
#include <string>
#include <list>
using namespace std;

class LCEmotionManager;
class LCVoiceManager;
class LCPhotoManager;
class LCVideoManager;
class LCUserItem;
class Record;
class LCMessageItem
{
public:
	// 消息类型定义
	typedef enum  {
		MT_Unknow,		// 未知类型
		MT_Text,		// 文本消息
		MT_Warning,		// 警告消息
		MT_Emotion,		// 高级表情
		MT_Voice,		// 语音
		MT_Photo,		// 私密照
		MT_Video,		// 微视频
		MT_System,		// 系统消息
		MT_Custom,		//自定义消息
	} MessageType;

	// 消息发送方向 类型
	typedef enum  {
		SendType_Unknow,		// 未知类型
		SendType_Send,			// 发出
		SendType_Recv,			// 接收
		SendType_System,		// 系统
	} SendType;

	// 处理状态
	typedef enum {
		StatusType_Unprocess,		// 未处理
		StatusType_Processing,		// 处理中
		StatusType_Fail,			// 发送失败
		StatusType_Finish,			// 发送完成/接收成功
	} StatusType;

	// 处理（发送/接收/购买）结果
	class ProcResult 
	{
	public:
		ProcResult() {
			SetSuccess();
		}
		virtual ~ProcResult() {}
	public:
		void SetSuccess() {
			m_errType = LCC_ERR_SUCCESS;
			m_errNum = "";
			m_errMsg = "";
		}
		void SetResult(LCC_ERR_TYPE errType, const string& errNum, const string& errMsg) {
			m_errType = errType;
			m_errNum = errNum;
			m_errMsg = errMsg;
		}
	public:
		LCC_ERR_TYPE	m_errType;	// 处理结果类型
		string	m_errNum;			// 处理结果代码
		string	m_errMsg;			// 处理结果描述
	};

public:
	LCMessageItem();
	virtual ~LCMessageItem();

public:
	// 初始化
	bool Init(
			int msgId
			, SendType sendType
			, const string& fromId
			, const string& toId
			, const string& inviteId
			, StatusType statusType);
	// 获取生成时间
	static long GetCreateTime();
	// 更新生成时间
	void RenewCreateTime();
	// 设置服务器当前数据库时间
	static void SetDbTime(long dbTime);
	// 把服务器时间转换为手机时间
	static long GetLocalTimeWithServerTime(long serverTime);
	// 使用Record初始化MessageItem
	bool InitWithRecord(
					int msgId
					, const string& selfId
					, const string& userId
					, const string& inviteId
					, const Record& record
					, LCEmotionManager* emotionMgr
					, LCVoiceManager* voiceMgr
					, LCPhotoManager* photoMgr
					, LCVideoManager* videoMgr);
	// 设置语音item
	void SetVoiceItem(LCVoiceItem* theVoiceItem);
	// 获取语音item
	LCVoiceItem* GetVoiceItem() const;
	// 设置图片item
	void SetPhotoItem(LCPhotoItem* thePhotoItem);
	// 获取图片item
	LCPhotoItem* GetPhotoItem() const;
	// 设置视频item
	void SetVideoItem(lcmm::LCVideoItem* theVideoItem);
	// 获取视频item
	lcmm::LCVideoItem* GetVideoItem() const;
	// 设置文本item
	void SetTextItem(LCTextItem* theTextItem);
	// 获取文本item
	LCTextItem* GetTextItem() const;
	// 设置warning item
	void SetWarningItem(LCWarningItem* theWarningItem);
	// 获取warning item
	LCWarningItem* GetWarningItem() const;
	// 设置高级表情item
	void SetEmotionItem(LCEmotionItem* theEmotionItem);
	// 获取高级表情item
	LCEmotionItem* GetEmotionItem() const;
	// 设置系统消息item
	void SetSystemItem(LCSystemItem* theSystemItem);
	// 获取系统消息item
	LCSystemItem* GetSystemItem() const;
	// 设置自定义消息item
	void SetCustomItem(LCCustomItem* theCustomItem);
	// 获取自定义消息item
	LCCustomItem* GetCustomItem() const;
	// 判断子消息item（如：语音、图片、视频等）是否正在处理
	bool IsSubItemProcssign() const;

	// 设置用户item
	void SetUserItem(LCUserItem* theUserItem);
	// 获取用户item
	LCUserItem* GetUserItem() const;
	// 重置所有成员变量
	void Clear();

	// 排序函数
	static bool Sort(const LCMessageItem* item1, const LCMessageItem* item2);
    
    // 判断是否聊天消息
    bool IsChatMessage();

public:
	int 			m_msgId;		// 消息ID
	SendType 		m_sendType;		// 消息发送方向
	string			m_fromId;		// 发送者ID
	string 			m_toId;			// 接收者ID
	string 			m_inviteId;		// 邀请ID
	long			m_createTime;	// 接收/发送时间
	StatusType 		m_statusType;	// 处理状态
	MessageType		m_msgType;		// 消息类型
	ProcResult		m_procResult;	// 处理(发送/接收/购买)结果

private:
	LCTextItem*		m_textItem;		// 文本item
	LCWarningItem*	m_warningItem;	// 警告item
	LCEmotionItem*	m_emotionItem;	// 高级表情ID
	LCVoiceItem*	m_voiceItem;	// 语音item
	LCPhotoItem*	m_photoItem;	// 图片item
	lcmm::LCVideoItem*	m_videoItem;	// 微视频item
	LCSystemItem*	m_systemItem;	// 系统消息item
	LCCustomItem*	m_customItem;	// 自定义消息item

	LCUserItem*		m_userItem;		// 用户item

	static long		m_dbTime;		// 服务器数据库当前时间
};
typedef list<LCMessageItem*>	LCMessageList;

