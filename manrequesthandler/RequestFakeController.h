/*
 * RequestFakeController.h
 *
 *  Created on: 2015-2-27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef REQUESTFAKECONTROLLER_H_
#define REQUESTFAKECONTROLLER_H_

#include "RequestBaseController.h"

#include <list>
using namespace std;

#include <json/json/json.h>

#include "RequestFakeDefine.h"

#include "item/CheckServerItem.h"

class IRequestFakeControllerCallback
{
public:
    IRequestFakeControllerCallback() {}
    virtual ~IRequestFakeControllerCallback() {}
public:
    virtual void OnCheckServer(long requestId, bool success, CheckServerItem item, string errnum, string errmsg) = 0;
};

class RequestFakeController : public RequestBaseController, public IHttpRequestManagerCallback {
public:
	RequestFakeController(HttpRequestManager* pHttpRequestManager, IRequestFakeControllerCallback* pRequestFakeControllerCallback/*, CallbackManager* pCallbackManager*/);
	virtual ~RequestFakeController();

    /**
     * 2.1.检查真假服务器
     * @param email				账号
     * @return					请求唯一标识
     */
    long CheckServer(
    		string email
    		);

protected:
	void onSuccess(long requestId, string path, const char* buf, int size);
	void onFail(long requestId, string path);

private:
	IRequestFakeControllerCallback* mpRequestFakeControllerCallback;
};

#endif /* RequestFakeController_H_ */
