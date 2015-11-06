/*
 * RequestSettingController.h
 *
 *  Created on: 2015-2-27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef REQUESTSETTINGCONTROLLER_H_
#define REQUESTSETTINGCONTROLLER_H_

#include "RequestBaseController.h"

#include <list>
using namespace std;

#include <json/json/json.h>

#include "RequestSettingDefine.h"

typedef void (*OnChangePassword)(long requestId, bool success, string errnum, string errmsg);

typedef struct RequestSettingControllerCallback {
	OnChangePassword onChangePassword;
} RequestSettingControllerCallback;

class RequestSettingController : public RequestBaseController, public IHttpRequestManagerCallback {
public:
	RequestSettingController(HttpRequestManager* pHttpRequestManager, RequestSettingControllerCallback callback/*, CallbackManager* pCallbackManager*/);
	virtual ~RequestSettingController();

	/**
	 * 3.1.修改密码
	 * @param oldPassword	新密码
	 * @param newPassword	旧密码
	 * @param callback
	 * @return				请求唯一标识
	 */
	long ChangePassword(string oldPassword, string newPassword);

protected:
	void onSuccess(long requestId, string path, const char* buf, int size);
	void onFail(long requestId, string path);

private:
	RequestSettingControllerCallback mRequestSettingControllerCallback;
};

#endif /* REQUESTSETTINGCONTROLLER_H_ */
