/*
 * EMFError.h
 *
 *  Created on: 2015-04-20
 *      Author: Samson.Fan
 */

#ifndef EMFERROR_H_
#define EMFERROR_H_

#include <string>
#include <list>
using namespace std;

#include "../RequestEMFDefine.h"
#include <json/json/json.h>
#include <common/CommonFunc.h>

// 发送邮件 EMF_SENDMSG_PATH(/emf/sendmsg)
class EMFSendMsgErrorItem
{
public:
	EMFSendMsgErrorItem()
	{
		money = "";
	}
	virtual ~EMFSendMsgErrorItem() {}

public:
	bool Parsing(const Json::Value& data)
	{
		bool result = false;
		if (data[EMF_SENDMSG_MONEY].isString()) {
			money = data[EMF_SENDMSG_MONEY].asString();
		}

		if (!money.empty()) {
			result = true;
		}
		return result;
	}

public:
	string	money;	// 余额
};


#endif /* EMFERROR_H_ */
