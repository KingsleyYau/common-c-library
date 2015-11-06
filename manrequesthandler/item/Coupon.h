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
				status = root[COMMON_RESULT].asInt() + 1;
				result = true;
			}
		}
		return result;
	}

	Coupon() {		status = 0;
	}
	virtual ~Coupon() {

	}

	/**
	 * 5.1.查询是否符合试聊条件回调
	 * @param status			试聊状态
	 */
	int status;

};

#endif /* COUPON_H_ */
