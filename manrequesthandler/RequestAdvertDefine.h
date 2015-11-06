/*
 * RequestAdvertDefine.h
 *
 *  Created on: 2015-04-21
 *      Author: Samson.Fan
 * Description: 广告 协议接口定义
 */

#ifndef REQUESTADVERTDEFINE_H_
#define REQUESTADVERTDEFINE_H_

#include <string>
#include <list>

using namespace std;

/* ########################	广告相关 定义  ######################## */
/// ------------------- 请求参数定义 -------------------
#define ADVERT_REQUEST_DEVICEID		"deviceId"
#define ADVERT_REQUEST_ADVERTID		"advertId"
#define ADVERT_REQUEST_PUSHID		"pushId"
#define ADVERT_REQUEST_EMAIL		"email"
#define ADVERT_REQUEST_SHOWTIMES	"showtimes"
#define ADVERT_REQUEST_CLICKTIMES	"clicktimes"

/// ------------------- 返回参数定义 -------------------
// 接口路径
// 查询浮窗广告
#define ADVERT_MAINADVERT_PATH		"/advert/mainadvert"
// 查询女士列表广告
#define ADVERT_WOMANLISTADVERT_PATH	"/advert/womanlistadvert"
// 查询Push广告
#define ADVERT_PUSHADVERT_PATH		"/advert/pushadvert"
// item
#define ADVERT_ADVERTID		"advertId"
#define ADVERT_IMAGE		"image"
#define ADVERT_WIDTH		"width"
#define ADVERT_HEIGHT		"height"
#define ADVERT_ADURL		"adurl"
#define ADVERT_OPENTYPE		"opentype"
#define ADVERT_ISSHOW		"isshow"
#define ADVERT_VALID		"valid"
#define ADVERT_PUSHID		"pushId"
#define ADVERT_MESSAGE		"message"

// ------ 枚举定义 ------
// 广告URL打开方式
typedef enum {
	AD_OT_HIDE = 0,			// 隐藏打开
	AD_OT_SYSTEMBROWER = 1,	// 系统浏览器打开
	AD_OT_APPBROWER = 2,	// app内嵌浏览器打开
	AD_OT_UNKNOW,			// 未知
	AD_OT_BEGIN = AD_OT_HIDE,	// 有效起始值
	AD_OT_END = AD_OT_UNKNOW,	// 有效结束值
} AdvertOpenType;
inline AdvertOpenType GetAdvertOpenTypWithInt(int value) {
	return (AD_OT_BEGIN <= value && value < AD_OT_END ? (AdvertOpenType)value : AD_OT_UNKNOW);
}

#endif /* REQUESTADVERTDEFINE_H_ */
