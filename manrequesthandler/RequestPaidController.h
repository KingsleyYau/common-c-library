/*
 * RequestPaidController.h
 *
 *  Created on: 2016-06-21
 *      Author: Samson
 */

#ifndef REQUEPAIDCONTROLLER_H_
#define REQUEPAIDCONTROLLER_H_

#include "RequestBaseController.h"

#include <list>
using namespace std;

#include <json/json/json.h>

#include "RequestPaidDefine.h"

typedef void (*OnPay)(long requestId, bool success, const string& code, const string& orderNo, const string& productId);
typedef void (*OnCallback)(long requestId, bool success, const string& code);

typedef struct RequestPaidControllerCallback {
	OnPay onPay;
    OnCallback onCallback;
} RequestPaidControllerCallback;

class RequestPaidController : public RequestBaseController, public IHttpRequestManagerCallback {
public:
	RequestPaidController(HttpRequestManager* pHttpRequestManager, RequestPaidControllerCallback callback/*, CallbackManager* pCallbackManager*/);
	virtual ~RequestPaidController();

	/**
	 * 7.1.获取订单信息
	 * @param manId         男士ID
	 * @param sid           跨服务器唯一标识
	 * @param number        信用点套餐ID
	 * @return				请求唯一标识
	 */
	long GetPaymentOrder(const string& manId, const string& sid, const string& number);
    
    /**
     * 7.2.验证订单信息
     * @param manId         男士ID
     * @param sid           跨服务器唯一标识
     * @param receipt       AppStore支付成功返回的recetip参数（BASE64）
     * @param orderNo       订单号
     * @param code          AppStore支付完成返回的状态码
     * @return				请求唯一标识
     */
    long CheckPayment(const string& manId, const string& sid, const string& receipt, const string& orderNo, int code);

protected:
	void onSuccess(long requestId, string path, const char* buf, int size);
	void onFail(long requestId, string path);
    
private:
    bool Parsing(const char* buf, int size, string &code, Json::Value &data);

private:
	RequestPaidControllerCallback mRequestPaidControllerCallback;
};

#endif /* REQUEPAIDCONTROLLER_H_ */
