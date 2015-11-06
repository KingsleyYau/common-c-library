/*
 * Advert.h
 *
 *  Created on: 2015-04-22
 *      Author: Samson.Fan
 */

#ifndef ADVERT_H_
#define ADVERT_H_

#include <string>
#include <list>
using namespace std;

#include <json/json/json.h>

#include <common/CommonFunc.h>

#include "../RequestAdvertDefine.h"

// 主界面广告
class AdMainAdvertItem {
public:
	AdMainAdvertItem()
	{
		advertId = "";
		image = "";
		width = 0;
		height = 0;
		adurl = "";
		openType = AD_OT_HIDE;
		isShow = false;
		valid = 0;
	}
	virtual ~AdMainAdvertItem() {}

public:
	bool Parsing(const Json::Value& data)
	{
		if (data.isObject()) {
			if (data[ADVERT_ADVERTID].isString()) {
				advertId = data[ADVERT_ADVERTID].asString();
			}
			if (data[ADVERT_IMAGE].isString()) {
				image = data[ADVERT_IMAGE].asString();
			}
			if (data[ADVERT_WIDTH].isIntegral()) {
				width = data[ADVERT_WIDTH].asInt();
			}
			if (data[ADVERT_HEIGHT].isIntegral()) {
				height = data[ADVERT_HEIGHT].asInt();
			}
			if (data[ADVERT_ADURL].isString()) {
				adurl = data[ADVERT_ADURL].asString();
			}
			if (data[ADVERT_OPENTYPE].isIntegral()) {
				openType = GetAdvertOpenTypWithInt(data[ADVERT_OPENTYPE].asInt());
			}
			if (data[ADVERT_ISSHOW].isIntegral()) {
				isShow = data[ADVERT_ISSHOW].asInt() == 1 ? true : false;
			}
			if (data[ADVERT_VALID].isIntegral()) {
				valid = data[ADVERT_VALID].asInt();
			}
		}

		return true;
	}
public:
	string	advertId;		// 广告ID
	string	image;			// 图片URL
	int		width;			// 图片宽度
	int 	height;			// 图片高度
	string	adurl;			// 点击打开的URL
	AdvertOpenType openType;	// URL打开方式
	bool	isShow;			// 是否显示
	long	valid;			// 有效时间
};

// 女士列表广告
class AdWomanListAdvertItem {
public:
	AdWomanListAdvertItem()
	{
		advertId = "";
		image = "";
		width = 0;
		height = 0;
		adurl = "";
		openType = AD_OT_HIDE;
	}
	virtual ~AdWomanListAdvertItem() {}

public:
	bool Parsing(const Json::Value& data)
	{
		if (data.isObject()) {
			if (data[ADVERT_ADVERTID].isString()) {
				advertId = data[ADVERT_ADVERTID].asString();
			}
			if (data[ADVERT_IMAGE].isString()) {
				image = data[ADVERT_IMAGE].asString();
			}
			if (data[ADVERT_WIDTH].isIntegral()) {
				width = data[ADVERT_WIDTH].asInt();
			}
			if (data[ADVERT_HEIGHT].isIntegral()) {
				height = data[ADVERT_HEIGHT].asInt();
			}
			if (data[ADVERT_ADURL].isString()) {
				adurl = data[ADVERT_ADURL].asString();
			}
			if (data[ADVERT_OPENTYPE].isIntegral()) {
				openType = GetAdvertOpenTypWithInt(data[ADVERT_OPENTYPE].asInt());
			}
		}

		return true;
	}
public:
	string	advertId;		// 广告ID
	string	image;			// 图片URL
	int		width;			// 图片宽度
	int 	height;			// 图片高度
	string	adurl;			// 点击打开的URL
	AdvertOpenType openType;	// URL打开方式
};

// Push广告
class AdPushAdvertItem {
public:
	AdPushAdvertItem()
	{
		pushId = "";
		message = "";
		adurl = "";
		openType = AD_OT_HIDE;
	}
	virtual ~AdPushAdvertItem() {}

public:
	bool Parsing(const Json::Value& data)
	{
		bool result = false;
		if (data.isObject()) {
			if (data[ADVERT_PUSHID].isString()) {
				pushId = data[ADVERT_PUSHID].asString();
			}
			if (data[ADVERT_MESSAGE].isString()) {
				message = data[ADVERT_MESSAGE].asString();
			}
			if (data[ADVERT_ADURL].isString()) {
				adurl = data[ADVERT_ADURL].asString();
			}
			if (data[ADVERT_OPENTYPE].isIntegral()) {
				openType = GetAdvertOpenTypWithInt(data[ADVERT_OPENTYPE].asInt());
			}

			if (!pushId.empty()) {
				result = true;
			}
		}

		return result;
	}
public:
	string	pushId;		// push ID
	string	message;	// push消息显示内容
	string	adurl;			// 点击打开的URL
	AdvertOpenType openType;	// URL打开方式
};
typedef list<AdPushAdvertItem> AdPushAdvertList;

#endif /* ADVERT_H_ */
