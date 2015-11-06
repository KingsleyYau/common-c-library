/*
 * RecordMutiple.h
 *
 *  Created on: 2015-3-2
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef RecordMutiple_H_
#define RecordMutiple_H_

#include <string>
using namespace std;

#include <json/json/json.h>

#include "../RequestLiveChatDefine.h"

#include "Record.h"

class RecordMutiple {
public:
	void Parse(Json::Value root) {
		if( root.isObject() ) {
		}
	}

	RecordMutiple() {		inviteId = "";
	}

	virtual ~RecordMutiple() {

	}

	/**
	 * 5.5.批量查询聊天记录回调
	 * @param inviteId			邀请ID
	 * @param recordList		聊天记录
	 */

	string inviteId;
	list<Record> recordList;
};

#endif /* RECORDMUTIPLE_H_ */
