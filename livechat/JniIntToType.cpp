/*
 * JniIntToType.cpp
 *
 *  Created on: 2015-04-14
 *      Author: Samson.Fan
 * Description: 处理int/jint与枚举type之间的转换
 */

#include "JniIntToType.h"
#include <common/CommonFunc.h>
#include <common/CheckMemoryLeak.h>

// LCC_ERR_TYPE(处理结果) 转换
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

// CHILDREN_TYPE(子女类型) 转换
CHILDREN_TYPE IntToChildrenType(int value)
{
	return (CHILDREN_TYPE)(value < _countof(ChildrenTypeArray) ? ChildrenTypeArray[value] : ChildrenTypeArray[0]);
}
int ChildrenTypeToInt(CHILDREN_TYPE type)
{
	int value = 0;
	int i = 0;
	for (i = 0; i < _countof(ChildrenTypeArray); i++)
	{
		if (type == ChildrenTypeArray[i]) {
			value = i;
			break;
		}
	}
	return value;
}

// USER_TYPE(用户类型) 转换
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

// AUTO_CHARGE_TYPE(自动充值类型) 转换
TAUTO_CHARGE_TYPE IntToAutoChargeType(int value)
{
	return (TAUTO_CHARGE_TYPE)(value < _countof(AutoChargeArray) ? AutoChargeArray[value] : AutoChargeArray[0]);
}
int AutoChargeTypeToInt(TAUTO_CHARGE_TYPE type)
{
	int value = 0;
	int i = 0;
	for (i = 0; i < _countof(AutoChargeArray); i++)
	{
		if (type == AutoChargeArray[i]) {
			value = i;
			break;
		}
	}
	return value;
}
