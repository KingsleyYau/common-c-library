/*
 * JniIntToType.h
 *
 *  Created on: 2015-04-14
 *      Author: Samson.Fan
 * Description: 处理int/jint与枚举type之间的转换
 */

#include "ILiveChatClient.h"

// LCC_ERR_TYPE(处理结果) 转换
static const int LccErrTypeArray[] = {
		LCC_ERR_FAIL,			// 服务器返回失败结果
		LCC_ERR_SUCCESS,		// 成功
		// 服务器返回错误
		LCC_ERR_UNBINDINTER,	// 女士的翻译未将其绑定
		LCC_ERR_SIDEOFFLINE,	// 对方不在线（聊天）
		LCC_ERR_NOMONEY,		// 帐号余额不足
		LCC_ERR_INVALIDUSER,	// 用户不存在（登录）
		LCC_ERR_INVAILDPWD,		// 密码错误（登录）
		LCC_ERR_BLOCKUSER,		// 对方为黑名单用户（聊天）
		LCC_ERR_EMOTIONERR,		// 高级表情异常（聊天）
		LCC_ERR_VOICEERR,		// 语音异常（聊天）
		// 客户端定义的错误
		LCC_ERR_PROTOCOLFAIL,	// 协议解析失败（服务器返回的格式不正确）
		LCC_ERR_CONNECTFAIL,	// 连接服务器失败/断开连接
		LCC_ERR_CHECKVERFAIL,	// 检测版本号失败（可能由于版本过低导致）
		LCC_ERR_LOGINFAIL,		// 登录失败
		LCC_ERR_SVRBREAK,		// 服务器踢下线
		LCC_ERR_SETOFFLINE,		// 不能把在线状态设为"离线"，"离线"请使用Logout()
};
LCC_ERR_TYPE IntToLccErrType(int value)
{
	return (LCC_ERR_TYPE)(value < _countof(LccErrTypeArray) ? LccErrTypeArray[value] : LccErrTypeArray[0]);
}
int LccErrTypeToInt(LCC_ERR_TYPE type)
{
	int value = 0;
	int i = 0;
	for (i = 0; i < _countof(LccErrTypeArray); i++)
	{
		if (type == LccErrTypeArray[i]) {
			value = i;
			break;
		}
	}
	return value;
}

// USER_SEX_TYPE(性别) 转换
static const int UserSexTypeArray[] = {
	USER_SEX_UNKNOW,	// 未知
	USER_SEX_FEMALE,	// 女士
	USER_SEX_MALE		// 男士
};
USER_SEX_TYPE IntToUserSexType(int value)
{
	return (USER_SEX_TYPE)(value < _countof(UserSexTypeArray) ? UserSexTypeArray[value] : UserSexTypeArray[0]);
}
int UserSexTypeToInt(USER_SEX_TYPE type)
{
	int value = 0;
	int i = 0;
	for (i = 0; i < _countof(UserSexTypeArray); i++)
	{
		if (type == UserSexTypeArray[i]) {
			value = i;
			break;
		}
	}
	return value;
}

// CLIENT_TYPE 转换
static const int ClientTypeArray[] = {
		CLIENT_UNKNOW,	// 未知
		CLIENT_PC,		// PC端
		CLIENT_PC_JAVA,	// PC Java端
		CLIENT_PC_APP,	// PC app端
		CLIENT_PC_PAD,	// PC pad端
		CLIENT_ANDROID,	// Android客户端
		CLIENT_IPHONE,	// IPhone客户端
		CLIENT_IPAD,	// IPad客户端
		CLIENT_WEB		// Web端
};
CLIENT_TYPE IntToClientType(int value)
{
	return (CLIENT_TYPE)(value < _countof(ClientTypeArray) ? ClientTypeArray[value] : ClientTypeArray[0]);
}
int ClientTypeToInt(CLIENT_TYPE type)
{
	int value = 0;
	int i = 0;
	for (i = 0; i < _countof(ClientTypeArray); i++)
	{
		if (type == ClientTypeArray[i]) {
			value = i;
			break;
		}
	}
	return value;
}

// USER_STATUS_TYPE(用户在线状态) 转换
static const int UserStatusTypeArray[] = {
	USTATUS_UNKNOW,				// 未知
	USTATUS_OFFLINE_OR_HIDDEN,	// 离线或隐身
	USTATUS_ONLINE,				// 在线
};
USER_STATUS_TYPE IntToUserStatusType(int value)
{
	return (USER_STATUS_TYPE)(value < _countof(UserStatusTypeArray) ? UserStatusTypeArray[value] : UserStatusTypeArray[0]);
}
int UserStatusTypeToInt(USER_STATUS_TYPE type)
{
	int value = 0;
	int i = 0;
	for (i = 0; i < _countof(UserStatusTypeArray); i++)
	{
		if (type == UserStatusTypeArray[i]) {
			value = i;
			break;
		}
	}
	return value;
}

// TRY_TICKET_EVENT(试聊事件) 转换
static const int TryTicketTypeArray[] = {
	TTE_UNKNOW,		// 未知
	TTE_NORMAL,		// 正常使用
	TTE_USED,		// 已使用券
	TTE_PAID,		// 已付费
	TTE_NOTICKET,	// 没有券
	TTE_OFFLINE		// 对方已离线
};
TRY_TICKET_EVENT IntToTryTicketEventType(int value)
{
	return (TRY_TICKET_EVENT)(value < _countof(TryTicketTypeArray) ? TryTicketTypeArray[value] : TryTicketTypeArray[0]);
}
int TryTicketEventTypeToInt(TRY_TICKET_EVENT type)
{
	int value = 0;
	int i = 0;
	for (i = 0; i < _countof(TryTicketTypeArray); i++)
	{
		if (type == TryTicketTypeArray[i]) {
			value = i;
			break;
		}
	}
	return value;
}

// TALK_MSG_TYPE(聊天消息类型) 转换
static const int TalkMsgTypeArray[] = {
	TMT_UNKNOW,			// 未知
	TMT_FREE,			// 免费
	TMT_CHARGE,			// 收费
	TMT_CHARGE_FREE,	// 试聊券
};
TALK_MSG_TYPE IntToTalkMsgType(int value)
{
	return (TALK_MSG_TYPE)(value < _countof(TalkMsgTypeArray) ? TalkMsgTypeArray[value] : TalkMsgTypeArray[0]);
}
int TalkMsgTypeToInt(TALK_MSG_TYPE type)
{
	int value = 0;
	int i = 0;
	for (i = 0; i < _countof(TalkMsgTypeArray); i++)
	{
		if (type == TalkMsgTypeArray[i]) {
			value = i;
			break;
		}
	}
	return value;
}

// TALK_EVENT_TYPE(聊天事件类型) 转换
static const int TalkEventTypeArray[] = {
	TET_UNKNOW,			// 未知
	TET_ENDTALK,		// 结束聊天
	TET_STARTCHARGE,	// 开始收费
	TET_STOPCHARGE,		// 暂停收费
	TET_NOMONEY,		// 余额不足
	TET_VIDEONOMONEY,	// 视频余额不足
	TET_TARGETNOTFOUND	// 目标不存在
};
TALK_EVENT_TYPE IntToTalkEventType(int value)
{
	return (TALK_EVENT_TYPE)(value < _countof(TalkEventTypeArray) ? TalkEventTypeArray[value] : TalkEventTypeArray[0]);
}
int TalkEventTypeToInt(TALK_EVENT_TYPE type)
{
	int value = 0;
	int i = 0;
	for (i = 0; i < _countof(TalkEventTypeArray); i++)
	{
		if (type == TalkEventTypeArray[i]) {
			value = i;
			break;
		}
	}
	return value;
}

// TALK_EMF_NOTICE_TYPE(邮件更新通知类型) 转换
static const int TalkEmfNoticeTypeArray[] = {
	TENT_UNKNOW,	// 未知
	TENT_EMF,		// EMF邮件
	TENT_ADMIRER	// Admirer邮件
};
TALK_EMF_NOTICE_TYPE IntToTalkEmfNoticeType(int value)
{
	return (TALK_EMF_NOTICE_TYPE)(value < _countof(TalkEmfNoticeTypeArray) ? TalkEmfNoticeTypeArray[value] : TalkEmfNoticeTypeArray[0]);
}
int TalkEmfNoticeTypeToInt(TALK_EMF_NOTICE_TYPE type)
{
	int value = 0;
	int i = 0;
	for (i = 0; i < _countof(TalkEmfNoticeTypeArray); i++)
	{
		if (type == TalkEmfNoticeTypeArray[i]) {
			value = i;
			break;
		}
	}
	return value;
}

// KICK_OFFLINE_TYPE(服务器踢下线类型) 转换
static const int KickOfflineTypeArray[] = {
	KOT_UNKNOW,			// 未知
	KOT_MAINTAIN,		// 服务器维护
	KOT_TIMEOUT,		// 心跳包超时
	KOT_OTHER_LOGIN		// 用户在其它地方登录
};
KICK_OFFLINE_TYPE IntToKickOfflineType(int value)
{
	return (KICK_OFFLINE_TYPE)(value < _countof(KickOfflineTypeArray) ? KickOfflineTypeArray[value] : KickOfflineTypeArray[0]);
}
int KickOfflineTypeToInt(KICK_OFFLINE_TYPE type)
{
	int value = 0;
	int i = 0;
	for (i = 0; i < _countof(KickOfflineTypeArray); i++)
	{
		if (type == KickOfflineTypeArray[i]) {
			value = i;
			break;
		}
	}
	return value;
}

// MARRY_TYPE(婚姻类型) 转换
static const int MarryTypeArray[] = {
	UT_UNKNOW,		// 未知
	MT_NOTMARRIY,	// 未婚
	MT_MARRIED,		// 已婚
};
MARRY_TYPE IntToMarryType(int value)
{
	return (MARRY_TYPE)(value < _countof(MarryTypeArray) ? MarryTypeArray[value] : MarryTypeArray[0]);
}
int MarryTypeToInt(MARRY_TYPE type)
{
	int value = 0;
	int i = 0;
	for (i = 0; i < _countof(MarryTypeArray); i++)
	{
		if (type == MarryTypeArray[i]) {
			value = i;
			break;
		}
	}
	return value;
}

// USER_TYPE(用户类型) 转换
static const int UserTypeArray[] = {
	UT_UNKNOW,		// 未知
	UT_WOMAN,		// 女士
	UT_MAN,			// 男士
	UT_TRANS,		// 翻译
};
USER_TYPE IntToUserType(int value)
{
	return (USER_TYPE)(value < _countof(UserTypeArray) ? UserTypeArray[value] : UserTypeArray[0]);
}
int UserTypeToInt(USER_TYPE type)
{
	int value = 0;
	int i = 0;
	for (i = 0; i < _countof(UserTypeArray); i++)
	{
		if (type == UserTypeArray[i]) {
			value = i;
			break;
		}
	}
	return value;
}

// DEVICE_TYPE(设备类型) 转换
static const int DeviceTypeArray[] = {
	DEVICE_TYPE_WEB,			// Web
	DEVICE_TYPE_WAP,			// WAP
	DEVICE_TYPE_WAP_ANDROID,	// WAP Android
	DEVICE_TYPE_WAP_IPHONE,		// WAP iPhone
	DEVICE_TYPE_APP_ANDROID,	// App Android
	DEVICE_TYPE_APP_IPHONE,		// App iPhone
	DEVICE_TYPE_APP_IPAD,		// App iPad
	DEVICE_TYPE_APP_PC,			// App PC
	DEVICE_TYPE_UNKNOW,			// 未知
};
DEVICE_TYPE IntToDeviceType(int value)
{
	return (DEVICE_TYPE)(value < _countof(DeviceTypeArray) ? DeviceTypeArray[value] : DeviceTypeArray[0]);
}
int DeviceTypeToInt(DEVICE_TYPE type)
{
	int value = 0;
	int i = 0;
	for (i = 0; i < _countof(DeviceTypeArray); i++)
	{
		if (type == DeviceTypeArray[i]) {
			value = i;
			break;
		}
	}
	return value;
}
