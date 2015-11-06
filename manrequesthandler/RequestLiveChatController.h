/*
 * RequestLiveChatController.h
 *
 *  Created on: 2015-2-27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef REQUESTLIVECHATCONTROLLER_H_
#define REQUESTLIVECHATCONTROLLER_H_

#include "RequestBaseController.h"

#include <list>
using namespace std;

#include <json/json/json.h>
#include <xml/tinyxml.h>

#include "RequestLiveChatDefine.h"
#include "RequestOtherDefine.h"

#include "item/Coupon.h"
#include "item/Gift.h"
#include "item/RecordMutiple.h"
#include "item/LCSendPhotoItem.h"
#include "item/LCVideoItem.h"

typedef void (*OnCheckCoupon)(long requestId, bool success, Coupon item, string userId, string errnum, string errmsg);
typedef void (*OnUseCoupon)(long requestId, bool success, string errnum, string errmsg, string userId);
typedef void (*OnQueryChatVirtualGift)(long requestId, bool success, list<Gift> giftList, int totalCount, string path, string version,string errnum, string errmsg);
typedef void (*OnQueryChatRecord)(long requestId, bool success, int dbTime, list<Record> recordList, string errnum, string errmsg, string inviteId);
typedef void (*OnQueryChatRecordMutiple)(long requestId, bool success, int dbTime, list<RecordMutiple> recordMutiList, string errnum, string errmsg);
typedef void (*OnSendPhoto)(long requestId, bool success, const string& errnum, const string& errmsg, const LCSendPhotoItem& item);
typedef void (*OnPhotoFee)(long requestId, bool success, const string& errnum, const string& errmsg);
typedef void (*OnGetPhoto)(long requestId, bool success, const string& errnum, const string& errmsg, const string& filePath);
typedef void (*OnUploadVoice)(long requestId, bool success, const string& errnum, const string& errmsg, const string& voiceId);
typedef void (*OnPalyVoice)(long requestId, bool success, const string& errnum, const string& errmsg, const string& filePath);
typedef void (*OnSendGift)(long requestId, bool success, const string& errnum, const string& errmsg);
typedef void (*OnQueryRecentVideoList)(long requestId, bool success, list<LCVideoItem> itemList, string errnum, string errmsg);
typedef void (*OnGetVideoPhoto)(long requestId, bool success, const string& errnum, const string& errmsg, const string& filePath);
typedef void (*OnGetVideo)(long requestId, bool success, const string& errnum, const string& errmsg, const string& url);
typedef struct RequestLiveChatControllerCallback {
	OnCheckCoupon onCheckCoupon;
	OnUseCoupon onUseCoupon;
	OnQueryChatVirtualGift onQueryChatVirtualGift;
	OnQueryChatRecord onQueryChatRecord;
	OnQueryChatRecordMutiple onQueryChatRecordMutiple;
	OnSendPhoto onSendPhoto;
	OnPhotoFee onPhotoFee;
	OnGetPhoto onGetPhoto;
	OnUploadVoice onUploadVoice;
	OnPalyVoice onPalyVoice;
	OnSendGift onSendGift;
	OnQueryRecentVideoList onQueryRecentVideoList;
	OnGetVideoPhoto onGetVideoPhoto;
	OnGetVideo onGetVideo;
} RequestLiveChatControllerCallback;


class RequestLiveChatController : public RequestBaseController, public IHttpRequestManagerCallback {
public:
	RequestLiveChatController(HttpRequestManager* pHttpRequestManager, RequestLiveChatControllerCallback callback/*, CallbackManager* pCallbackManager*/);
	virtual ~RequestLiveChatController();

	/**
	 * 5.1.查询是否符合试聊条件
	 * @param womanId			女士ID
	 * @return					请求唯一标识
	 */
	long CheckCoupon(string womanId);

	/**
	 * 5.2.使用试聊券
	 * @param womanId			女士ID
	 * @return					请求唯一标识
	 */
	long UseCoupon(string womanId);

	/**
	 * 5.3.获取虚拟礼物列表
	 * @param sessionId			登录成功返回的sessionid
	 * @param userId			登录成功返回的manid
	 * @return					请求唯一标识
	 */
	long QueryChatVirtualGift(string sessionId, string userId);

	/**
	 * 5.4.查询聊天记录
	 * @param inviteId			邀请ID
	 * @return					请求唯一标识
	 */
	long QueryChatRecord(string inviteId);

	/**
	 * 5.5.批量查询聊天记录
	 * @param inviteId			邀请ID数组
	 * @return					请求唯一标识
	 */
	long QueryChatRecordMutiple(list<string> inviteIdList);

	// 发送私密照片
	long SendPhoto(const string& targetId, const string& inviteId, const string& userId, const string& sid, SENDPHOTO_MODE_TYPE modeType, const string& photoId, const string& filePath);

	// 付费获取私密照片
	long PhotoFee(const string& targetId, const string& inviteId, const string& userId, const string& sid, const string& photoId);

	// 获取私密照片文件
	long GetPhoto(GETPHOTO_TOFLAG_TYPE toFlag
			, const string& targetId
			, const string& userId
			, const string& sid
			, const string& photoId
			, GETPHOTO_PHOTOSIZE_TYPE sizeType
			, GETPHOTO_PHOTOMODE_TYPE modeType
			, const string& filePath);

	// 上传语音文件
	long UploadVoice(const string& voiceCode, const string& inviteId, const string& mineId, bool isMan, const string& userId, OTHER_SITE_TYPE siteType, const string& fileType, int voiceLen, const string& filePath);

	// 获取语音文件
	long PlayVoice(const string& voiceId, OTHER_SITE_TYPE siteType, const string& filePath);

	/**
	 * 6.11.发送虚拟礼物
	 * @param womanId		女士ID
	 * @param vg_id			虚拟礼物ID
	 * @param device_id		设备唯一标识
	 * @param chat_id		livechat邀请ID或EMF邮件ID
	 * @param use_type		模块类型<UseType>
	 * @param user_sid		登录成功返回的sessionid
	 * @param user_id		登录成功返回的manid
	 * @return				请求唯一Id
	 */
	long SendGift(string womanId, string vg_id, string device_id, string chat_id, int use_type, string user_sid, string user_id);

	/**
	 * 6.12.获取最近已看微视频列表（http post）（New）
	 * @param womanId		女士ID
	 * @return				请求唯一Id
	 */
	long QueryRecentVideo(string user_sid, string user_id, string womanId);

	/**
	 * 6.13.获取微视频图片（http get）（New）
	 * @param womanId		女士ID
	 * @param videoid		视频ID
	 * @param type			图片尺寸
	 * @return				请求唯一Id
	 */
	long GetVideoPhoto(
			string user_sid,
			string user_id,
			string womanId,
			string videoid,
			VIDEO_PHOTO_TYPE type,
			const string& filePath
			);

	/**
	 * 6.14.获取微视频文件URL（http post）（New）
	 * @param womanId		女士ID
	 * @param videoid		视频ID
	 * @param inviteid		邀请ID
	 * @param toflag		客户端类型（0：女士端，1：男士端）
	 * @param sendid		发送ID，在LiveChat收到女士端发出的消息中
	 * @return				请求唯一Id
	 */
	long GetVideo(
			string user_sid,
			string user_id,
			string womanId,
			string videoid,
			string inviteid,
			int toflag,
			string sendid
			);

protected:
	virtual void onSuccess(long requestId, string path, const char* buf, int size);
	virtual void onFail(long requestId, string path);
	virtual void onReceiveBody(long requestId, string path, const char* buf, int size);

private:
	string GetTempFilePath(const string& filePath);

private:
	RequestLiveChatControllerCallback mRequestLiveChatControllerCallback;

	void HandleQueryChatVirtualGift(TiXmlDocument &doc, list<Gift> &giftList, int &totalCount,
			string &path, string &version);
	void HandleQueryRecentVideo(TiXmlDocument &doc, list<LCVideoItem> &itemList);
	void HandleGetVideo(TiXmlDocument &doc, string &url);
};

#endif /* REQUESTLIVECHATCONTROLLER_H_ */
