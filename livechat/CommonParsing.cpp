/*
 * author: Samson.Fan
 *   date: 2015-06-16
 *   file: CommonParsing.cpp
 *   desc: LiveChatClient公共解析协议
 */

#include "CommonParsing.h"

// UserInfo参数
#define USERID_PARAM		"userId"		// 用户ID
#define USERNAME_PARAM		"userName"		// 用户名称
#define SERVER_PARAM		"server"		// 服务器名称
#define IMGURL_PARAM		"imgUrl"		// 头像URL
#define GENDER_PARAM		"gender"		// 性别
#define AGE_PARAM			"age"			// 年龄
#define WEIGHT_PARAM		"weight"		// 体重
#define HEIGHT_PARAM		"height"		// 身高
#define COUNTRY_PARAM		"country"		// 国家
#define PROVINCE_PARAM		"province"		// 省份
#define VIDEOCHAT_PARAM		"videochat"		// 是否能视频聊天
#define VIDEO_PARAM			"video"			// 视频数目
#define MARRY_PARAM			"marry"			// 是否已婚
#define STATUS_PARAM		"status"		// 在线状态（在线：1、5、7，其它不在线）
#define STATUS_PARAM_ONLINE1	1				// 在线1
#define STATUS_PARAM_ONLINE2	5				// 在线2
#define STATUS_PARAM_ONLINE3	7				// 在线3
#define TYPE_PARAM			"type"			// 用户类型
#define TYPE_PARAM_WOMAN		0				// 女士
#define TYPE_PARAM_MAN			1				// 男士
#define TYPE_PARAM_TRANS		2				// 翻译
#define ORDERVALUE_PARAM	"orderValue"	// 用户分值
#define DEVICETYPE_PARAM	"deviceType"	// 设备类型
#define CLIENTTYPE_PARAM	"clientType"	// 客户端类型
#define CLIENTVERSION_PARAM	"clientVersion"	// 客户端版本号
// 解析UserInfo
bool ParsingUserInfoItem(amf_object_handle handle, UserInfoItem& item)
{
	bool result = false;
	if (!handle.isnull()
		&& handle->type == DT_OBJECT)
	{
		// userId
		amf_object_handle userIdObject = handle->get_child(USERID_PARAM);
		if (!userIdObject.isnull()
			&& userIdObject->type == DT_STRING)
		{
			item.userId = userIdObject->strValue;
		}

		// userName
		amf_object_handle userNameObject = handle->get_child(USERNAME_PARAM);
		if (!userNameObject.isnull()
			&& userNameObject->type == DT_STRING)
		{
			item.userName = userNameObject->strValue;
		}

		// server
		amf_object_handle serverObject = handle->get_child(SERVER_PARAM);
		if (!serverObject.isnull()
			&& serverObject->type == DT_STRING)
		{
			item.server = serverObject->strValue;
		}

		// imgUrl
		amf_object_handle imgUrlObject = handle->get_child(IMGURL_PARAM);
		if (!imgUrlObject.isnull()
			&& imgUrlObject->type == DT_STRING)
		{
			item.imgUrl = imgUrlObject->strValue;
		}

		// gender
		amf_object_handle genderObject = handle->get_child(GENDER_PARAM);
		if (!genderObject.isnull()
			&& genderObject->type == DT_INTEGER)
		{
			item.sexType = GetUserSexType(genderObject->intValue);
		}

		// age
		amf_object_handle ageObject = handle->get_child(AGE_PARAM);
		if (!ageObject.isnull()
			&& ageObject->type == DT_INTEGER)
		{
			item.age = ageObject->intValue;
		}

		// weight
		amf_object_handle weightObject = handle->get_child(WEIGHT_PARAM);
		if (!weightObject.isnull()
			&& weightObject->type == DT_STRING)
		{
			item.weight = weightObject->strValue;
		}

		// height
		amf_object_handle heightObject = handle->get_child(HEIGHT_PARAM);
		if (!heightObject.isnull()
			&& heightObject->type == DT_STRING)
		{
			item.height = heightObject->strValue;
		}

		// country
		amf_object_handle countryObject = handle->get_child(COUNTRY_PARAM);
		if (!countryObject.isnull()
			&& countryObject->type == DT_STRING)
		{
			item.country = countryObject->strValue;
		}

		// province
		amf_object_handle provinceObject = handle->get_child(PROVINCE_PARAM);
		if (!provinceObject.isnull()
			&& provinceObject->type == DT_STRING)
		{
			item.province = provinceObject->strValue;
		}

		// videochat
		amf_object_handle videoChatObject = handle->get_child(VIDEOCHAT_PARAM);
		if (!videoChatObject.isnull()
			&& videoChatObject->type == DT_INTEGER)
		{
			item.videoChat = videoChatObject->intValue==1 ? true : false;
		}

		// video
		amf_object_handle videoObject = handle->get_child(VIDEO_PARAM);
		if (!videoObject.isnull()
			&& videoObject->type == DT_INTEGER)
		{
			item.videoCount = videoObject->intValue;
		}

		// marry
		amf_object_handle marryObject = handle->get_child(MARRY_PARAM);
		if (!marryObject.isnull()
			&& marryObject->type == DT_INTEGER)
		{
			item.marryType = GetMarryType(marryObject->intValue);
		}

		// status
		amf_object_handle statusObject = handle->get_child(STATUS_PARAM);
		if (!statusObject.isnull()
			&& statusObject->type == DT_INTEGER)
		{
			switch (statusObject->intValue)
			{
			case 1:
			case 5:
			case 7:
				item.status = USTATUS_ONLINE;
				break;
			default:
				item.status = USTATUS_OFFLINE_OR_HIDDEN;
				break;
			}
		}

		// type
		amf_object_handle typeObject = handle->get_child(TYPE_PARAM);
		if (!typeObject.isnull()
			&& typeObject->type == DT_INTEGER)
		{
			item.userType = GetUserType(typeObject->intValue);
		}

		// orderValue
		amf_object_handle orderValueObject = handle->get_child(ORDERVALUE_PARAM);
		if (!orderValueObject.isnull()
			&& orderValueObject->type == DT_INTEGER)
		{
			item.orderValue = orderValueObject->intValue;
		}

		// deviceValue
		amf_object_handle deviceTypeObject = handle->get_child(DEVICETYPE_PARAM);
		if (!deviceTypeObject.isnull()
			&& deviceTypeObject->type == DT_INTEGER)
		{
			item.deviceType = (DEVICE_TYPE)deviceTypeObject->intValue;
		}

		// clientValue
		amf_object_handle clientTypeObject = handle->get_child(CLIENTTYPE_PARAM);
		if (!clientTypeObject.isnull()
			&& clientTypeObject->type == DT_INTEGER)
		{
			item.clientType = GetClientType(clientTypeObject->intValue);
		}

		// clientVersion
		amf_object_handle clientVersionObject = handle->get_child(CLIENTVERSION_PARAM);
		if (!clientVersionObject.isnull()
			&& clientVersionObject->type == DT_STRING)
		{
			item.clientVersion = clientVersionObject->strValue;
		}

		// 判断是否解析成功
		if (!item.userId.empty()) {
			result = true;
		}
	}
	return result;
}
