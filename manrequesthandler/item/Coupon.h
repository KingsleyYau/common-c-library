/*
 * Coupon.h
 *
 *  Created on: 2015-3-2
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef COUPON_H_
#define COUPON_H_

#include <string>
using namespace std;


#include <json/json/json.h>

#include "../RequestLiveChatDefine.h"
#include "../RequestDefine.h"

class Coupon {
public:
	bool Parse(Json::Value root) {
		bool result = false;
		if( root.isObject() ) {
			if( root[COMMON_RESULT].isInt() ) {
				status = (CouponStatus)root[COMMON_RESULT].asInt();
				result = true;
			}
			if(root[COMMON_DATA].isObject()){
				Json::Value dataJson = root[COMMON_DATA];
				if(dataJson[LIVECHAT_TRYCHAT_FREETRIAL].isString()){
					freetrial = atoi(dataJson[LIVECHAT_TRYCHAT_FREETRIAL].asString().c_str());
				}
				if(dataJson[LIVECHAT_TRYCHAT_REFUNDFLAG].isInt()){
					refundflag = dataJson[LIVECHAT_TRYCHAT_REFUNDFLAG].asInt() == 1 ? true:false;
				}
				if(dataJson[LIVECHAT_TRYCHAT_FESTIVALID].isString()){
					festivalid = dataJson[LIVECHAT_TRYCHAT_FESTIVALID].asString();
				}
				if(dataJson[LIVECHAT_TRYCHAT_TIME].isInt()){
					time = dataJson[LIVECHAT_TRYCHAT_TIME].asInt();
				}
				if(dataJson[LIVECHAT_TRYCHAT_COUPONID].isString()){
					couponid = dataJson[LIVECHAT_TRYCHAT_COUPONID].asString();
				}
				if(dataJson[LIVECHAT_TRYCHAT_ENDDATE].isString()){
					enddate = dataJson[LIVECHAT_TRYCHAT_ENDDATE].asString();
				}
			}
		}
		return result;
	}

	Coupon() {		status = CouponStatus_None;
		freetrial = 0;
		refundflag = false;
		festivalid = "";
		time = 0;
		couponid = "";
		enddate = "";
	}
	virtual ~Coupon() {

	}

	/**
	 * 5.1.查询是否符合试聊条件回调
	 * @param status			试聊状态
	 */
	CouponStatus status;
	int freetrial;
	bool refundflag;
	string festivalid;
	int time;
	string couponid;
	string enddate;
};

#endif /* COUPON_H_ */
