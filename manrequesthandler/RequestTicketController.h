/*
 * RequestTicketController.h
 *
 *  Created on: 2015-07-13
 *      Author: Samson.Fan
 */

#ifndef REQUESTTICKETCONTROLLER_H_
#define REQUESTTICKETCONTROLLER_H_

#include "RequestBaseController.h"
#include "item/Ticket.h"

typedef void (*OnRequestTicketList)(long requestId, bool success, const string& errnum, const string& errmsg, int pageIndex, int pageSize, int dataCount, const TicketList& ticketList);
typedef void (*OnRequestTicketDetail)(long requestId, bool success, const string& errnum, const string& errmsg, const TicketDetailItem& item);
typedef void (*OnRequestReplyTicket)(long requestId, bool success, const string& errnum, const string& errmsg);
typedef void (*OnRequestResolvedTicket)(long requestId, bool success, const string& errnum, const string& errmsg);
typedef void (*OnRequestAddTicket)(long requestId, bool success, const string& errnum, const string& errmsg);
typedef struct _tagRequestTicketControllerCallback {
	OnRequestTicketList onRequestTicketList;
	OnRequestTicketDetail onRequestTicketDetail;
	OnRequestReplyTicket onRequestReplyTicket;
	OnRequestResolvedTicket onRequestResolvedTicket;
	OnRequestAddTicket onRequestAddTicket;
} RequestTicketControllerCallback;

class RequestTicketController : public RequestBaseController, public IHttpRequestManagerCallback {
public:
	RequestTicketController(HttpRequestManager* pHttpRequestManager, const RequestTicketControllerCallback& callback);
	virtual ~RequestTicketController();

public:
	long QueryTicketList(int pageIndex, int pageSize);
	long TicketDetail(const string& ticketId);
	long ReplyTicket(const string& ticketId, const string& message, const string& filePath);
	long ResolvedTicket(const string& ticketId);
	long AddTicket(int typeId, const string& title, const string& message, const string& filePath);

private:
	void QueryTicketListCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void TicketDetailCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void ReplyTicketCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void ResolvedTicketCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void AddTicketCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);

protected:
	void onSuccess(long requestId, string path, const char* buf, int size);
	void onFail(long requestId, string path);

private:
	RequestTicketControllerCallback m_Callback;
};

#endif /* REQUESTTICKETCONTROLLER_H_ */
