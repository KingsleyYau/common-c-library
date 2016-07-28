/*
 * RequestAuthorizationDefine.h
 *
 *  Created on: 2015-2-27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef REQUESTAUTHORIZATIONDEFINE_H_
#define REQUESTAUTHORIZATIONDEFINE_H_

#include "RequestDefine.h"

/* ########################	认证相关 模块  ######################## */

/* 字段 */

/* facebook */
#define AUTHORIZATION_FACEBOOK_TOKEN 	"ftoken"
#define AUTHORIZATION_BIRTHDAY			"birthday"

/* 注册 */
#define AUTHORIZATION_MAN_ID			"manid"
#define AUTHORIZATION_EMAIL 			"email"
#define AUTHORIZATION_PASSWORD 			"passwd1"
#define AUTHORIZATION_GENDER 			"gender"
#define AUTHORIZATION_FIRST_NAME 		"first_name"
#define AUTHORIZATION_LAST_NAME 		"last_name"
#define AUTHORIZATION_IS_REG 			"is_reg"
#define AUTHORIZATION_REG_STEP 			"reg_step"
#define AUTHORIZATION_COUNTRY 			"country"
#define AUTHORIZATION_BIRTHDAY_Y		"birthday_y"
#define AUTHORIZATION_BIRTHDAY_M 		"birthday_m"
#define AUTHORIZATION_BIRTHDAY_D 		"birthday_d"
#define AUTHORIZATION_WEEKLY_MAIL		"weeklymail"
#define AUTHORIZATION_MODEL 			"model"
#define AUTHORIZATION_DEVICEID 			"deviceId"
#define AUTHORIZATION_MANUFACTURER 		"manufacturer"
#define AUTHORIZATION_UTMREFERRER 		"utm_referrer"
#define AUTHORIZATION_LOGIN				"login"
#define AUTHORIZATION_ERRNO				"errno"
#define	AUTHORIZATION_ERRTEXT			"errtext"
#define AUTHORIZATION_PREVCODE			"prevcode"

/* 登录 */
#define AUTHORIZATION_PASSWORD2 		"password"
#define AUTHORIZATION_CHECKCODE			"checkcode"

#define AUTHORIZATION_VERSIONCODE		"versioncode"
#define AUTHORIZATION_FIRSTNAME 		"firstname"
#define AUTHORIZATION_LASTNAME 			"lastname"

#define AUTHORIZATION_PHOTOURL			"photoURL"
#define AUTHORIZATION_TELEPHONE			"telephone"
#define AUTHORIZATION_TELEPHONE_VERIFY	"telephone_verify"
#define AUTHORIZATION_TELEPHONE_CC		"telephone_cc"
#define AUTHORIZATION_SESSIONID			"sessionid"
#define AUTHORIZATION_GA_UID			"ga_uid"
#define AUTHORIZATION_CSMAIL_TICKETID	"csmail_ticketid"
#define AUTHORIZATION_PHOTOSEND			"photosend"
#define AUTHORIZATION_PHOTORECEIVED		"photoreceived"
#define AUTHORIZATION_VIDEORECEIVED		"videoreceived"
#define AUTHORIZATION_PREMIT			"premit"

#define AUTHORIZATION_PERMISSION		"permission"
#define AUTHORIZATION_LADYPROFILE		"ladyprofile"
#define AUTHORIZATION_LIVECHAT			"livechat"
#define AUTHORIZATION_ADMIRER			"admirer"
#define AUTHORIZATION_BPEMF				"bpemf"

#define AUTHORIZATION_RECHARGECREDIT	"recharge_credit"
#define AUTHORIZATION_GAACTIVITY		"ga_activity"
#define AUTHORIZATION_ADOVERVIEW		"ad_overview"
#define AUTHORIZATION_ADTIMESTAMP		"ad_timestamp"

/* 找回密码 */
#define AUTHORIZATION_SENDMAIL			"sendmail"

/* 手机短信验证 */
#define AUTHORIZATION_VERIFY_CODE		"verify_code"
#define AUTHORIZATION_V_TYPE			"v_type"
#define AUTHORIZATION_DEVICE_ID 		"device_id"

/* 固定电话获取认证短信 */
#define AUTHORIZATION_LANDLINE_CC		"landline_cc"
#define AUTHORIZATION_LANDLINE_AC		"landline_ac"
#define AUTHORIZATION_LANDLINE			"landline"

/* 字段  end*/

/* 接口路径定义 */

/**
 * 2.1.Facebook注册及登录
 */
#define FACEBOOK_LOGIN_PATH "/member/facebooklogin"

/**
 * 2.2.注册帐号
 */
#define REGISTER_PATH "/member/joincheck"

/**
 * 2.3.获取验证码
 */
#define CEHCKCODE_PATH "/member/checkcode"

/**
 * 2.4.登录
 */
#define LOGIN_PATH "/member/logincheck"

/**
 * 2.5.找回密码
 */
#define FINDPASSWORD_PATH "/member/find_pw"

/**
 * 2.6.手机获取认证短信
 */
#define GET_SMS_PATH "/member/sms_get"

/**
 * 2.7.手机短信认证
 */
#define VERIFY_SMS_PATH "/member/sms_verify"

/**
 * 2.8.固定电话获取认证短信
 */
#define GET_FIXED_PHONE_PATH "/member/sms_get"

/**
 * 2.9.固定电话获取认证短信
 */
#define VERIFY_FIXED_PHONE_PATH "/member/sms_verify"

/* 接口路径定义  end */

#endif /* REQUESTAUTHORIZATIONDEFINE_H_ */
