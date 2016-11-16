/*
 * RequestAuthorizationController.h
 *
 *  Created on: 2015-2-27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef REQUESTAUTHORIZATIONCONTROLLER_H_
#define REQUESTAUTHORIZATIONCONTROLLER_H_

#include "RequestBaseController.h"

#include <list>
using namespace std;

#include <json/json/json.h>

#include "RequestAuthorizationDefine.h"

#include "item/RegisterItem.h"
#include "item/LoginItem.h"
#include "item/LoginErrorItem.h"
#include "item/LoginFacebookItem.h"

typedef void (*OnLoginWithFacebook)(long requestId, bool success, LoginFacebookItem item, string errnum, string errmsg,
		LoginErrorItem errItem);
typedef void (*OnRegister)(long requestId, bool success, RegisterItem item, string errnum, string errmsg);
typedef void (*OnGetCheckCode)(long requestId, bool success, const char* data, int len, string errnum, string errmsg);
typedef void (*OnLogin)(long requestId, bool success, LoginItem item, string errnum, string errmsg);
typedef void (*OnFindPassword)(long requestId, bool success, string tips, string errnum, string errmsg);
typedef void (*OnGetSms)(long requestId, bool success, string errnum, string errmsg);
typedef void (*OnVerifySms)(long requestId, bool success, string errnum, string errmsg);
typedef void (*OnGetFixedPhone)(long requestId, bool success, string errnum, string errmsg);
typedef void (*OnVerifyFixedPhone)(long requestId, bool success, string errnum, string errmsg);
typedef void (*OnSummitAppToken)(long requestId, bool success, string errnum, string errmsg);
typedef void (*OnUnbindAppToken)(long requestId, bool success, string errnum, string errmsg);

typedef struct RequestAuthorizationControllerCallback {
	OnLoginWithFacebook onLoginWithFacebook;
	OnRegister onRegister;
	OnGetCheckCode onGetCheckCode;
	OnLogin onLogin;
	OnFindPassword onFindPassword;
	OnGetSms onGetSms;
	OnVerifySms onVerifySms;
	OnGetFixedPhone onGetFixedPhone;
	OnVerifyFixedPhone onVerifyFixedPhone;
	OnSummitAppToken onSummitAppToken;
	OnUnbindAppToken onUnbindAppToken;
} RequestAuthorizationControllerCallback;


class RequestAuthorizationController : public RequestBaseController, public IHttpRequestManagerCallback {
public:
	RequestAuthorizationController(HttpRequestManager* pHttpRequestManager, RequestAuthorizationControllerCallback callback/*, CallbackManager* pCallbackManager*/);
	virtual ~RequestAuthorizationController();

    /**
     * 2.1.Facebook注册及登录
     * @param token				Facebook登录返回的token
     * @param email				电子邮箱
     * @param password			密码
     * @param deviceId			设备唯一标识
     * @param versioncode		客户端内部版本号
     * @param model				移动设备型号
     * @param manufacturer		制造厂商
     * @param prevcode			上一步操作的错误代码
     * @param birthday_y		生日的年
     * @param birthday_m		生日的月
     * @param birthday_d		生日的日
     * @param referrer			app推广参数（安装成功app第一次运行时GooglePlay返回）
     * @return					请求唯一标识
     */
    long LoginWithFacebook(
    		string token,
    		string email,
    		string password,
    		string deviceId,
    		string versioncode,
    		string model,
    		string manufacturer,
    		string prevcode,
    		string birthday_y,
    		string birthday_m,
    		string birthday_d,
    		string referrer
    		);

    /**
     * 2.2.注册帐号
     * @param email				电子邮箱
     * @param password			密码
     * @param male				性别, true:男性/false:女性
     * @param first_name		用户first name
     * @param last_name			用户last name
     * @param country			国家区号,参考数组<CountryArray>
     * @param birthday_y		生日的年
     * @param birthday_m		生日的月
     * @param birthday_d		生日的日
     * @param weeklymail		是否接收订阅
     * @param model				移动设备型号
     * @param deviceId			设备唯一标识
     * @param manufacturer		制造厂商
     * @param referrer			app推广参数（安装成功app第一次运行时GooglePlay返回）
     * @return					请求唯一标识
     */
	long Register(
			string email,
			string password,
			bool male,
			string first_name,
			string last_name,
			int country,
			string birthday_y,
			string birthday_m,
			string birthday_d,
			bool weeklymail,
			string model,
			string deviceId,
			string manufacturer,
			string referrer
			);

    /**
     * 2.3.获取验证码
     * @param callback
     * @return					请求唯一标识
     */
	long GetCheckCode();

    /**
     * 2.4.登录
     * @param email				电子邮箱
     * @param password			密码
     * @param checkcode			验证码
     * @param deviceId			设备唯一标识
     * @param versioncode		客户端内部版本号
     * @param model				移动设备型号
     * @param manufacturer		制造厂商
     * @return					请求唯一标识
     */
    long Login(
    		string email,
    		string password,
    		string checkcode,
    		string deviceId,
    		string versioncode,
    		string model,
    		string manufacturer
    		);

    /**
     * 2.5.找回密码
     * @param email			用户注册的邮箱
     * @param checkcode		验证码
     * @return				请求唯一标识
     */
    long FindPassword(string email, string checkcode);

    /**
     * 2.6.手机获取认证短信
     * @param telephone			电话号码
     * @param telephone_cc		国家区号,参考参考数组<CountryArray>
     * @param device_id			设备唯一标识
     * @return					请求唯一标识
     */
    long GetSms(string telephone, int telephone_cc, string device_id);

    /**
     * 2.7.手机短信认证
     * @param verify_code		验证码
     * @param v_type			验证类型
     * @return					请求唯一标识
     */
    long VerifySms(string verify_code, int v_type);

    /**
     * 2.8.固定电话获取认证短信
     * @param landline			电话号码
     * @param telephone_cc		国家区号,参考数组<CountryArray>
     * @param landline_ac		区号
     * @param device_id			设备唯一标识
     * @return					请求唯一标识
     */
    long GetFixedPhone(string landline, int telephone_cc, string landline_ac, string device_id);

    /**
     * 2.9.固定电话短信认证
     * @param verify_code		验证码
     * @return					请求唯一标识
     */
    long VerifyFixedPhone(string verify_code);

    /**
     * 2.11. 添加App token
     * @param deviceId
     * @param tokenId
     */
    long SummitAppToken(string deviceId, string tokenId);

    /**
     * 2.12. 销毁App token
     */
    long UnbindAppToken();

protected:
	void onSuccess(long requestId, string path, const char* buf, int size);
	void onFail(long requestId, string path);

private:
	RequestAuthorizationControllerCallback mRequestAuthorizationControllerCallback;
};

#endif /* RequestAuthorizationController_H_ */
