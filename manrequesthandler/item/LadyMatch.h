/*
 * LadyMatch.h
 *
 *  Created on: 2015-3-2
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef LADYMATCH_H_
#define LADYMATCH_H_

#include <string>
using namespace std;


#include <json/json/json.h>

#include "../RequestLadyDefine.h"

class LadyMatch {
public:
	void Parse(Json::Value root) {
		if( root.isObject() ) {
			if( root[LADY_AGE_RANGE_FROM].isString() ) {
				age1 = atoi(root[LADY_AGE_RANGE_FROM].asString().c_str());
			}

			if( root[LADY_AGE_RANGE_TO].isString() ) {
				age2 = atoi(root[LADY_AGE_RANGE_TO].asString().c_str());
			}

			if( root[LADY_M_MARRY].isString() ) {
				marry = atoi(root[LADY_M_MARRY].asString().c_str());
			}

			if( root[LADY_M_CHILDREN].isString() ) {
				children = atoi(root[LADY_M_CHILDREN].asString().c_str());
			}

			if( root[LADY_M_EDUCATION].isString() ) {
				education = atoi(root[LADY_M_EDUCATION].asString().c_str());
			}
		}
	}

	LadyMatch() {		age1 = 0;
		age2 = 0;
		marry = 0;
		children = 0;
		education = 0;
	}

	virtual ~LadyMatch() {

	}

	int age1;
	int age2;
	int marry;
	int children;
	int education;
};

#endif /* LADYMATCH_H_ */
