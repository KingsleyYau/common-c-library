/*
 * LoginErrorItem.h
 *
 *  Created on: 2015-3-6
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef LOGINERRORITEM_H_
#define LOGINERRORITEM_H_

#include <string>
using namespace std;

#include <json/json/json.h>

#include "../RequestAuthorizationDefine.h"

#include "../RequestEnumDefine.h"

class LoginErrorItem {
public:
	void Parse(Json::Value root) {
		if( root.isObject() ) {
			/* 电子邮箱 */
			if( root[AUTHORIZATION_EMAIL].isString() ) {
				email = root[AUTHORIZATION_EMAIL].asString();
			}
			/* 用户first name */
			if( root[AUTHORIZATION_FIRST_NAME].isString() ) {
				firstname = root[AUTHORIZATION_FIRSTNAME].asString();
			}
			if( root[AUTHORIZATION_LAST_NAME].isString() ) {
				lastname = root[AUTHORIZATION_LASTNAME].asString();
			}

			if( root[AUTHORIZATION_PHOTOURL].isString() ) {
				photoURL = root[AUTHORIZATION_PHOTOURL].asString();
			}
		}
	}

	LoginErrorItem() {
		email = "";
		firstname = "";
		lastname = "";
		photoURL = "";
	}
	virtual ~LoginErrorItem() {

	}

	/**
	 * 登录失败回调
	 * @param email				电子邮箱
	 * @param firstname			用户first name
	 * @param lastname			用户last name
	 * @param photoURL			头像URL
	 */

	string email;
	string firstname;
	string lastname;
	string photoURL;
};

#endif /* LOGINERRORITEM_H_ */
