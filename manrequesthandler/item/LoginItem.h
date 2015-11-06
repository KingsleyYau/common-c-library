/*
 * LoginItem.h
 *
 *  Created on: 2015-3-6
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef LOGINITEM_H_
#define LOGINITEM_H_

#include <string>
using namespace std;

#include <json/json/json.h>

#include "../RequestAuthorizationDefine.h"

#include "../RequestEnumDefine.h"

class LoginItem {
public:
	void Parse(Json::Value root) {
		if( root.isObject() ) {
			/* 用户id */
			if( root[AUTHORIZATION_MAN_ID].isString() ) {
				manid = root[AUTHORIZATION_MAN_ID].asString();
			}
			/* 电子邮箱 */
			if( root[AUTHORIZATION_EMAIL].isString() ) {
				email = root[AUTHORIZATION_EMAIL].asString();
			}
			/* 用户first name */
			if( root[AUTHORIZATION_FIRSTNAME].isString() ) {
				firstname = root[AUTHORIZATION_FIRSTNAME].asString();
			}
			if( root[AUTHORIZATION_LASTNAME].isString() ) {
				lastname = root[AUTHORIZATION_LASTNAME].asString();
			}

			if( root[AUTHORIZATION_PHOTOURL].isString() ) {
				photoURL = root[AUTHORIZATION_PHOTOURL].asString();
			}

			if( root[AUTHORIZATION_REG_STEP].isString() ) {
				reg_step = root[AUTHORIZATION_REG_STEP].asString();
			}

			if( root[AUTHORIZATION_COUNTRY].isString() ) {
				string strCountry = root[AUTHORIZATION_COUNTRY].asString();
				country = GetCountryCode(strCountry);
			}

			if( root[AUTHORIZATION_TELEPHONE].isString() ) {
				telephone = root[AUTHORIZATION_TELEPHONE].asString();
			}

			if( root[AUTHORIZATION_TELEPHONE_VERIFY].isInt() ) {
				telephone_verify = root[AUTHORIZATION_TELEPHONE_VERIFY].asInt();
			}

			if( root[AUTHORIZATION_TELEPHONE_CC].isString() ) {
				string strCountry = root[AUTHORIZATION_TELEPHONE_CC].asString();
				telephone_cc = GetCountryCode(strCountry);
			}

			if( root[AUTHORIZATION_SESSIONID].isString() ) {
				sessionid = root[AUTHORIZATION_SESSIONID].asString();
			}

			if( root[AUTHORIZATION_GA_UID].isString() ) {
				ga_uid = root[AUTHORIZATION_GA_UID].asString();
			}

			if ( root[AUTHORIZATION_CSMAIL_TICKETID].isString() ) {
				ticketid = root[AUTHORIZATION_CSMAIL_TICKETID].asString();
			}

			if( root[AUTHORIZATION_PHOTOSEND].isString() ) {
				string strValue = root[AUTHORIZATION_PHOTOSEND].asString();
				photosend = (atoi(strValue.c_str()) == 0)?"false":"true";
			}

			if( root[AUTHORIZATION_PHOTORECEIVED].isString() ) {
				string strValue = root[AUTHORIZATION_PHOTORECEIVED].asString();
				photoreceived = (atoi(strValue.c_str()) == 0)?"false":"true";
			}

			if( root[AUTHORIZATION_VIDEORECEIVED].isString() ) {
				string strValue = root[AUTHORIZATION_VIDEORECEIVED].asString();
				videoreceived = (atoi(strValue.c_str()) == 0)?"false":"true";
			}

			if( root[AUTHORIZATION_PREMIT].isString() ) {
				string strValue = root[AUTHORIZATION_PREMIT].asString();
				if( strValue.compare("Y") == 0 ) {
					premit = true;
				} else {
					premit = false;
				}
			}

			if( root[AUTHORIZATION_PERMISSION].isObject() ) {
				Json::Value permission = root[AUTHORIZATION_PERMISSION];

				if( permission[AUTHORIZATION_LADYPROFILE].isInt() ) {
					ladyprofile = permission[AUTHORIZATION_LADYPROFILE].asInt();
				}

				if( permission[AUTHORIZATION_LIVECHAT].isInt() ) {
					livechat = permission[AUTHORIZATION_LIVECHAT].asInt();
				}

				if( permission[AUTHORIZATION_ADMIRER].isInt() ) {
					admirer = permission[AUTHORIZATION_ADMIRER].asInt();
				}

				if( permission[AUTHORIZATION_BPEMF].isInt() ) {
					bpemf = permission[AUTHORIZATION_BPEMF].asInt();
				}
			}
		}
	}

	LoginItem() {
		manid = "";
		email = "";
		firstname = "";
		lastname = "";
		photoURL = "";
		reg_step = "";
		country = GetOtherCountryCode();
		telephone = "";
		telephone_verify = false;
		telephone_cc = GetOtherCountryCode();
		sessionid = "";
		ga_uid = "";
		ticketid = "";

		photosend = true;
		photoreceived = true;
		videoreceived = true;

		premit = true;
		ladyprofile = false;
		livechat = false;
		admirer = false;
		bpemf = false;
	}
	virtual ~LoginItem() {

	}

	/**
	 * 登录成功回调
	 * @param manid				用户id
	 * @param email				电子邮箱
	 * @param firstname			用户first name
	 * @param lastname			用户last name
	 * @param photoURL			头像URL
	 * @param reg_step			已进行的注册步骤数
	 * @param country			国家
	 * @param telephone			手机号码
	 * @param telephone_verify	手机是否已认证
	 * @param telephone_cc		国家区号
	 * @param sessionid			跨服务器的唯一标识
	 * @param ga_uid			Google Analytics UserID参数
	 * @param ticketid			客服邮件强制阅读的事项ID
	 * @param photosend			私密照片发送权限
	 * @param photoreceived		私密照片接收权限
	 * @param videoreceived		微视频接收权限（true：允许，false：不能）
	 * @param premit			帐号可用状态
	 * @param ladyprofile		女士详细信息风控标识（true：有风控，false：无）
	 * @param livechat			LiveChat详细风控标识（true：有风控，false：无）
	 * @param admirer			意向信风控标识（true：有风控，false：无）
	 * @param bpemf				EMF风控标识（true：有风控，false：无）
	 */

	string manid;
	string email;
	string firstname;
	string lastname;
	string photoURL;
	string reg_step;
	int country;
	string telephone;
	bool telephone_verify;
	int telephone_cc;
	string sessionid;
	string ga_uid;
	string ticketid;
	bool photosend;
	bool photoreceived;
	bool premit;
	bool ladyprofile;
	bool livechat;
	bool admirer;
	bool bpemf;
	bool videoreceived;
};

#endif /* LOGINITEM_H_ */
