/*
 * Lady.h
 *
 *  Created on: 2015-3-2
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef LADY_H_
#define LADY_H_

#include <string>
using namespace std;


#include <json/json/json.h>

#include "../RequestLadyDefine.h"
#include "../RequestEnumDefine.h"

class Lady {
public:
	void Parse(Json::Value root) {
		if( root.isObject() ) {
			if( root[LADY_AGE].isInt() ) {
				age = root[LADY_AGE].asInt();
			}

			if( root[LADY_WOMAN_ID].isString() ) {
				womanid = root[LADY_WOMAN_ID].asString();
			}

			if( root[LADY_FIRST_NAME].isString() ) {
				firstname = root[LADY_FIRST_NAME].asString();
			}

			if( root[LADY_WEIGHT].isString() ) {
				weight = root[LADY_WEIGHT].asString();
			}

			if( root[LADY_HEIGHT].isString() ) {
				height = root[LADY_HEIGHT].asString();
			}

			if( root[LADY_COUNTRY].isString() ) {
				country = root[LADY_COUNTRY].asString();
                countryIndex = GetCountryCode(country);
			}

			if( root[LADY_PROVINCE].isString() ) {
				province = root[LADY_PROVINCE].asString();
			}

			if( root[LADY_PHOTO_URL].isString() ) {
				photoURL = root[LADY_PHOTO_URL].asString();
			}

			if ( root[LADY_ONLINE_STATUS].isInt() ) {
				int status = root[LADY_ONLINE_STATUS].asInt();
				onlineStatus = (LADY_OSTATUS_BEGIN <= status && status <= LADY_OSTATUS_END
										? (LadyOnlineStatus)status : LADY_OSTATUS_DEFAULT);
			}
		}
	}

	Lady() {
		age = 0;
		womanid = "";
		firstname = "";
		weight = "";
		height = "";
		country = "";
        countryIndex = GetOtherCountryCode();
		province = "";
		photoURL = "";
		onlineStatus = LADY_OSTATUS_DEFAULT;
	}
	virtual ~Lady() {

	}

	/**
	 * 女士列表成功回调
	 * @param age			年龄
	 * @param womanid		女士ID
	 * @param firstname		女士first name
	 * @param weight		重量
	 * @param height		高度
	 * @param country		国家
	 * @param province		省份
	 * @param photoURL		图片URL
	 * @param onlineStatus	在线状态
	 */
	int age;
	string womanid;
	string firstname;
	string weight;
	string height;
	string country;
    int countryIndex;
	string province;
	string photoURL;
	LadyOnlineStatus onlineStatus;
};

#endif /* LADY_H_ */
