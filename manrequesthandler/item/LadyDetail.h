/*
 * LadyDetail.h
 *
 *  Created on: 2015-3-2
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef LADYDETAIL_H_
#define LADYDETAIL_H_

#include <string>
using namespace std;

#include "../common/StringHandle.h"

#include <json/json/json.h>

#include "../RequestLadyDefine.h"

#include "VideoItem.h"

class LadyDetail {
public:
	void Parse(Json::Value root) {
		if( root.isObject() ) {
			if( root[LADY_WOMAN_ID].isString() ) {
				womanid = root[LADY_WOMAN_ID].asString();
			}

			if( root[LADY_FIRST_NAME].isString() ) {
				firstname = root[LADY_FIRST_NAME].asString();
			}

			if( root[LADY_COUNTRY].isString() ) {
				country = root[LADY_COUNTRY].asString();
			}

			if( root[LADY_PROVINCE].isString() ) {
				province = root[LADY_PROVINCE].asString();
			}

			if( root[LADY_AGE].isInt() ) {
				age = root[LADY_AGE].asInt();
			}

			if( root[LADY_ZODIAC].isString() ) {
				zodiac = root[LADY_ZODIAC].asString();
			}

			if( root[LADY_WEIGHT].isString() ) {
				weight = root[LADY_WEIGHT].asString();
			}

			if( root[LADY_HEIGHT].isString() ) {
				height = root[LADY_HEIGHT].asString();
			}

			if( root[LADY_SMOKE].isString() ) {
				smoke = root[LADY_SMOKE].asString();
			}

			if( root[LADY_DRINK].isString() ) {
				drink = root[LADY_DRINK].asString();
			}

			if( root[LADY_ENGLISH].isString() ) {
				english = root[LADY_ENGLISH].asString();
			}

			if( root[LADY_RELIGION].isString() ) {
				religion = root[LADY_RELIGION].asString();
			}

			if( root[LADY_EDUCATION].isString() ) {
				education = root[LADY_EDUCATION].asString();
			}

			if( root[LADY_PROFESSION].isString() ) {
				profession = root[LADY_PROFESSION].asString();
			}

			if( root[LADY_CHILDREN].isString() ) {
				children = root[LADY_CHILDREN].asString();
			}

			if( root[LADY_MARRY].isString() ) {
				marry = root[LADY_MARRY].asString();
			}

			if( root[LADY_RESUME].isString() ) {
				resume = root[LADY_RESUME].asString();
			}

			if( root[LADY_AGE_RANGE_FROM].isInt() ) {
				age1 = root[LADY_AGE_RANGE_FROM].asInt();
			}

			if( root[LADY_AGE_RANGE_TO].isInt() ) {
				age2 = root[LADY_AGE_RANGE_TO].asInt();
			}

			if( root[LADY_ISONLINE].isInt() ) {
				isonline = (root[LADY_ISONLINE].asInt() == 0)?false:true;
			}

			if( root[LADY_ISFAVORITE].isInt() ) {
				isfavorite = (root[LADY_ISFAVORITE].asInt() == 0)?false:true;
			}

			if( root[LADY_LAST_UPDATE].isString() ) {
				last_update = root[LADY_LAST_UPDATE].asString();
			}

			if( root[LADY_SHOW_LOVECALL].isInt() ) {
				show_lovecall = root[LADY_SHOW_LOVECALL].asInt();
			}

			if( root[LADY_PHOTO_URL].isString() ) {
				photoURL = root[LADY_PHOTO_URL].asString();
			}

			if( root[LADY_MIN_PHOTO_URL].isString() ) {
				photoMinURL = root[LADY_MIN_PHOTO_URL].asString();
			}

			if( root[LADY_PHOTOURLLIST].isString() ) {
				string listString = root[LADY_PHOTOURLLIST].asString();
				photoList = StringHandle::split(listString, ",");
			}

			if( root[LADY_THUMBURLLIST].isString() ) {
				string listString = root[LADY_THUMBURLLIST].asString();
				thumbList = StringHandle::split(listString, ",");
			}

			if( root[LADY_VIDEOURLLIST].isArray() ) {
				for(int i = 0; i < root[LADY_VIDEOURLLIST].size(); i++ ) {
					VideoItem item;
					item.Parse(root[LADY_VIDEOURLLIST].get(i, Json::Value::null));
					videoList.push_back(item);
				}
			}
		}
	}

	LadyDetail() {		womanid = "";
		firstname = "";
		country = "";
		province = "";
		birthday = "";
		age = 0;
		zodiac = "";
		weight = "";
		height  = "";
		smoke = "";
		drink = "";
		english = "";
		religion = "";
		education = "";
		profession = "";
		children = "";
		marry = "";
		resume = "";
		age1 = 0;
		age2 = 0;
		isonline = false;
		isfavorite = false;
		last_update = "";
		show_lovecall = 0;
		photoURL = "";
		photoMinURL = "";
	}
	virtual ~LadyDetail() {

	}

	/**
	 * 查询女士详细信息回调
	 * @param womanid			女士ID
	 * @param firstname			女士first name
	 * @param country			国家
	 * @param province			省份
	 * @param birthday			出生日期
	 * @param age				年龄
	 * @param zodiac			星座
	 * @param weight			体重
	 * @param height			身高
	 * @param smoke				吸烟情况
	 * @param drink				喝酒情况
	 * @param english			英语能力
	 * @param religion			宗教情况
	 * @param education			教育情况
	 * @param profession		职业
	 * @param children			子女状况
	 * @param marry				婚姻状况
	 * @param resume			个人简介
	 * @param age1				期望的起始年龄
	 * @param age2				期望的结束年龄
	 * @param isonline			是否在线
	 * @param isfavorite		是否收藏
	 * @param last_update		最后更新时间
	 * @param show_lovecall		是否显示Love Call功能
	 * @param photoURL			女士头像URL
	 * @param photoMinURL		女士小头像URL(100*133)
	 * @param thumbList			拇指图URL列表
	 * @param photoList			图片URL列表
	 * @param videoList			视频列表
	 */

	string womanid;
	string firstname;
	string country;
	string province;
	string birthday;
	int age;
	string zodiac;
	string weight;
	string height;
	string smoke;
	string drink;
	string english;
	string religion;
	string education;
	string profession;
	string children;
	string marry;
	string resume;
	int age1;
	int age2;
	bool isonline;
	bool isfavorite;
	string last_update;
	int show_lovecall;

	string photoURL;
	string photoMinURL;

	list<string> thumbList;
	list<string> photoList;
	list<VideoItem> videoList;
};

#endif /* LADYDETAIL_H_ */
