/*
 * RequestTicketController.cpp
 *
 *  Created on: 2015-07-13
 *      Author: Samson.Fan
 */

#include "RequestTicketController.h"
#include "RequestDefine.h"
#include "RequestTicketDefine.h"
#include "./item/ticket.h"
#include "../common/CommonFunc.h"

RequestTicketController::RequestTicketController(HttpRequestManager *pHttpRequestManager, const RequestTicketControllerCallback& callback)
{
	// TODO Auto-generated constructor stub
	SetHttpRequestManager(pHttpRequestManager);
	m_Callback = callback;
}

RequestTicketController::~RequestTicketController()
{
	// TODO Auto-generated destructor stub
}

/* IHttpRequestManagerCallback */
void RequestTicketController::onSuccess(long requestId, string url, const char* buf, int size)
{
	FileLog("httprequest", "RequestTicketController::onSuccess( url : %s, buf( size : %d ) )", url.c_str(), size);
	if (size < MAX_LOG_BUFFER) {
		FileLog("httprequest", "RequestTicketController::onSuccess(), buf: %s", buf);
	}

	if( url.compare(TICKET_TICKETLIST_PATH) == 0 ) {
		QueryTicketListCallbackHandle(requestId, url, true, buf, size);
	}
	else if( url.compare(TICKET_TICKETDETAIL_PATH) == 0 ) {
		TicketDetailCallbackHandle(requestId, url, true, buf, size);
	}
	else if( url.compare(TICKET_REPLYTICKET_PATH) == 0 ) {
		ReplyTicketCallbackHandle(requestId, url, true, buf, size);
	}
	else if( url.compare(TICKET_RESOLVEDTICKET_PATH) == 0 ) {
		ResolvedTicketCallbackHandle(requestId, url, true, buf, size);
	}
	else if( url.compare(TICKET_ADDTICKET_PATH) == 0 ) {
		AddTicketCallbackHandle(requestId, url, true, buf, size);
	}
	FileLog("httprequest", "RequestTicketController::onSuccess() end, url:%s", url.c_str());
}

void RequestTicketController::onFail(long requestId, string url)
{
	FileLog("httprequest", "RequestTicketController::onFail( url : %s )", url.c_str());
	/* request fail, callback fail */
	if( url.compare(TICKET_TICKETLIST_PATH) == 0 ) {
		QueryTicketListCallbackHandle(requestId, url, false, NULL, 0);
	}
	else if( url.compare(TICKET_TICKETDETAIL_PATH) == 0 ) {
		TicketDetailCallbackHandle(requestId, url, false, NULL, 0);
	}
	else if( url.compare(TICKET_REPLYTICKET_PATH) == 0 ) {
		ReplyTicketCallbackHandle(requestId, url, false, NULL, 0);
	}
	else if( url.compare(TICKET_RESOLVEDTICKET_PATH) == 0 ) {
		ResolvedTicketCallbackHandle(requestId, url, false, NULL, 0);
	}
	else if( url.compare(TICKET_ADDTICKET_PATH) == 0 ) {
		AddTicketCallbackHandle(requestId, url, false, NULL, 0);
	}
	FileLog("httprequest", "RequestTicketController::onFail() end, url:%s", url.c_str());
}

// ----------------------- TicketList -----------------------
long RequestTicketController::QueryTicketList(int pageIndex, int pageSize)
{
	char temp[16];
	HttpEntiy entiy;

	// pageIndex
	sprintf(temp, "%d", pageIndex);
	entiy.AddContent(COMMON_PAGE_INDEX, temp);
	// pageSize
	sprintf(temp, "%d", pageSize);
	entiy.AddContent(COMMON_PAGE_SIZE, temp);

	string url = TICKET_TICKETLIST_PATH;
	FileLog("httprequest", "RequestTicketController::GetTicetList"
			"(url:%s, pageIndex:%d, pageSize:%d)",
			url.c_str(), pageIndex, pageSize);

	return StartRequest(url, entiy, this);
}

void RequestTicketController::QueryTicketListCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size)
{
	TicketList ticketList;
	int pageIndex = 0, pageSize = 0, dataCount = 0;
	string errnum = "";
	string errmsg = "";
	bool bFlag = false;

	if (requestRet) {
		// request success
		Json::Value dataJson;
		if( HandleResult(buf, size, errnum, errmsg, &dataJson) ) {
			if (dataJson.isObject()) {
				// 解析pageIndex、pageCount、dataCount
				Json::Value pageIndexJson = dataJson[COMMON_PAGE_INDEX];
				if (pageIndexJson.isIntegral()) {
					pageIndex = pageIndexJson.asInt();
				}

				Json::Value pageSizeJson = dataJson[COMMON_PAGE_SIZE];
				if (pageSizeJson.isIntegral()) {
					pageSize = pageSizeJson.asInt();
				}

				Json::Value dataCountJson = dataJson[COMMON_DATA_COUNT];
				if (dataCountJson.isIntegral()) {
					dataCount = dataCountJson.asInt();
				}

				// 解析列表
				Json::Value dataListJson = dataJson[COMMON_DATA_LIST];
				bFlag = dataListJson.isArray();
				if(bFlag) {
					for(int i = 0; i < dataListJson.size(); i++) {
						TicketListItem item;
						if (item.Parsing(dataListJson.get(i, Json::Value::null))) {
							ticketList.push_back(item);
						}
					}
				}
			}

			if (!bFlag) {
				// parsing fail
				errnum = LOCAL_ERROR_CODE_PARSEFAIL;
				errmsg = LOCAL_ERROR_CODE_PARSEFAIL_DESC;

				FileLog("httprequest", "RequestTicketController::TicketListCallbackHandle() parsing fail:"
						"(url:%s, size:%d, buf:%s)",
						url.c_str(), size, buf);
			}
		}
	}
	else {
		// request fail
		errnum = LOCAL_ERROR_CODE_TIMEOUT;
		errmsg = LOCAL_ERROR_CODE_TIMEOUT_DESC;
	}

	if( m_Callback.onRequestTicketList != NULL ) {
		m_Callback.onRequestTicketList(requestId, bFlag, errnum, errmsg, pageIndex, pageSize, dataCount, ticketList);
	}
}

// ----------------------- TicketDetail -----------------------
long RequestTicketController::TicketDetail(const string& ticketId)
{
	HttpEntiy entiy;
	entiy.AddContent(TICKET_REQUEST_TICKETID, ticketId.c_str());

	string url = TICKET_TICKETDETAIL_PATH;
	FileLog("httprequest", "RequestTicketController::TicketDetail"
			"(url:%s, ticketId:%s)",
			url.c_str(), ticketId.c_str());

	return StartRequest(url, entiy, this);
}

void RequestTicketController::TicketDetailCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size)
{
	TicketDetailItem item;
	string errnum = "";
	string errmsg = "";
	bool bFlag = false;

	if (requestRet) {
		// request success
		Json::Value dataJson;
		if( HandleResult(buf, size, errnum, errmsg, &dataJson) ) {
			// success
			bFlag = item.Parsing(dataJson);
			if (!bFlag)
			{
				// parsing fail
				errnum = LOCAL_ERROR_CODE_PARSEFAIL;
				errmsg = LOCAL_ERROR_CODE_PARSEFAIL_DESC;

				FileLog("httprequest", "RequestTicketController::TicketDetailCallbackHandle() parsing fail:"
							"(url:%s, size:%d, buf:%s)",
							url.c_str(), size, buf);
			}
		}
	}
	else {
		// request fail
		errnum = LOCAL_ERROR_CODE_TIMEOUT;
		errmsg = LOCAL_ERROR_CODE_TIMEOUT_DESC;
	}

	if( m_Callback.onRequestTicketDetail != NULL ) {
		m_Callback.onRequestTicketDetail(requestId, bFlag, errnum, errmsg, item);
	}
}

// ----------------------- ReplyTicket -----------------------
long RequestTicketController::ReplyTicket(const string& ticketId, const string& message, const string& filePath)
{
	HttpEntiy entiy;
	entiy.AddContent(TICKET_REQUEST_TICKETID, ticketId.c_str());
	entiy.AddContent(TICKET_REQUEST_MESSAGE, message.c_str());
	if (!filePath.empty()) {
		entiy.AddFile(TICKET_REQUEST_FILE, filePath);
	}

	string url = TICKET_REPLYTICKET_PATH;
	FileLog("httprequest", "RequestTicketController::ReplyTicket"
			"(url:%s, ticketId:%s, message:%s, filePath:%s)",
			url.c_str(), ticketId.c_str(), message.c_str(), filePath.c_str());

	return StartRequest(url, entiy, this);
}

void RequestTicketController::ReplyTicketCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size)
{
	string errnum = "";
	string errmsg = "";
	bool bFlag = false;

	if (requestRet) {
		// request success
		bFlag = HandleResult(buf, size, errnum, errmsg, NULL);
	}
	else {
		// request fail
		errnum = LOCAL_ERROR_CODE_TIMEOUT;
		errmsg = LOCAL_ERROR_CODE_TIMEOUT_DESC;
	}

	if( m_Callback.onRequestReplyTicket != NULL ) {
		m_Callback.onRequestReplyTicket(requestId, bFlag, errnum, errmsg);
	}
}

// ----------------------- ResolvedTicket -----------------------
long RequestTicketController::ResolvedTicket(const string& ticketId)
{
	char temp[16];
	HttpEntiy entiy;

	// ticketid
	entiy.AddContent(TICKET_REQUEST_TICKETID, ticketId.c_str());

	string url = TICKET_RESOLVEDTICKET_PATH;
	FileLog("httprequest", "RequestTicketController::ResolvedTicket"
			"(url:%s, ticketId:%s)",
			url.c_str(), ticketId.c_str());

	return StartRequest(url, entiy, this);
}

void RequestTicketController::ResolvedTicketCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size)
{
	string errnum = "";
	string errmsg = "";
	bool bFlag = false;

	if (requestRet) {
		// request success
		bFlag = HandleResult(buf, size, errnum, errmsg, NULL);
	}
	else {
		// request fail
		errnum = LOCAL_ERROR_CODE_TIMEOUT;
		errmsg = LOCAL_ERROR_CODE_TIMEOUT_DESC;
	}

	if( m_Callback.onRequestResolvedTicket != NULL ) {
		m_Callback.onRequestResolvedTicket(requestId, bFlag, errnum, errmsg);
	}
}

// ----------------------- AddTicket -----------------------
long RequestTicketController::AddTicket(int typeId, const string& title, const string& message, const string& filePath)
{
	HttpEntiy entiy;
	char temp[64] = {0};

	// typeId
	sprintf(temp, "%d", typeId);
	entiy.AddContent(TICKET_REQUEST_TYPEID, temp);
	// title
	entiy.AddContent(TICKET_REQUEST_TITLE, title.c_str());
	// message
	entiy.AddContent(TICKET_REQUEST_MESSAGE, message.c_str());
	// filePath
	if (!filePath.empty()) {
		entiy.AddFile(TICKET_REQUEST_FILE, filePath.c_str());
	}

	string url = TICKET_ADDTICKET_PATH;
	FileLog("httprequest", "RequestTicketController::AddTicket"
			"(url:%s, typeId:%d, title:%s, message:%s, filePath:%s)",
			url.c_str(), typeId, title.c_str(), message.c_str(), filePath.c_str());

	return StartRequest(url, entiy, this);
}

void RequestTicketController::AddTicketCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size)
{
	string errnum = "";
	string errmsg = "";
	bool bFlag = false;

	if (requestRet) {
		// request success
		bFlag = HandleResult(buf, size, errnum, errmsg, NULL);
	}
	else {
		// request fail
		errnum = LOCAL_ERROR_CODE_TIMEOUT;
		errmsg = LOCAL_ERROR_CODE_TIMEOUT_DESC;
	}

	if( m_Callback.onRequestAddTicket != NULL ) {
		m_Callback.onRequestAddTicket(requestId, bFlag, errnum, errmsg);
	}
}
