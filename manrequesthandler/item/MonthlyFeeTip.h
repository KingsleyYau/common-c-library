/*
 * MonthlyFeeTip.h
 *
 *  Created on: 2016-5-10
 *      Author: Hunter
 */

#ifndef MONTHLYFEETIP_H_
#define MONTHLYFEETIP_H_

#include <string>
#include <list>
using namespace std;


#include <json/json/json.h>

#include "../RequestMonthlyFeeDefine.h"

class MonthlyFeeTip {
public:
	void Parse(Json::Value root) {
		if( root.isObject() ) {
			if( root[MONTHLY_FEE_MEMBER_TYPE].isInt() ) {
				memberType = root[MONTHLY_FEE_MEMBER_TYPE].asInt();
			}

			if( root[MONTHLY_FEE_TITLE_PRICE].isString() ) {
				priceTilte = root[MONTHLY_FEE_TITLE_PRICE].asString();
			}

			if( root[MONTHLY_FEE_TIPS_LIST].isArray() ) {
				for(int i = 0; i < root[MONTHLY_FEE_TIPS_LIST].size(); i++ ) {
					Json::Value element = root[MONTHLY_FEE_TIPS_LIST].get(i, Json::Value::null);
					if(element.isString()){
						tipList.push_back(element.asString());
					}
				}
			}
		}
	}

	MonthlyFeeTip() {
		memberType = 0;
		priceTilte = "";
	}
	virtual ~MonthlyFeeTip() {

	}
	int memberType;
	string priceTilte;
	list<string> tipList;
};

#endif /* MONTHLYFEETIP_H_ */
