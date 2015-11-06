/*
 * RequestBaseController.cpp
 *
 *  Created on: 2015-3-2
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#include "RequestBaseController.h"

RequestBaseController::RequestBaseController() {
	// TODO Auto-generated constructor stub

}

RequestBaseController::~RequestBaseController() {
	// TODO Auto-generated destructor stub
}

void RequestBaseController::SetHttpRequestManager(HttpRequestManager *pHttpRequestManager) {
	mpHttpRequestManager = pHttpRequestManager;
}
long RequestBaseController::StartRequest(string url, HttpEntiy& entiy, IHttpRequestManagerCallback* callback, SiteType type, bool bNoCache) {
	if( mpHttpRequestManager != NULL ) {
		return mpHttpRequestManager->StartRequest(url, entiy, callback, type, bNoCache);
	}
	return -1;
}

bool RequestBaseController::StopRequest(long requestId, bool bWait) {
	bool bFlag = false;
	if( mpHttpRequestManager != NULL ) {
		bFlag =mpHttpRequestManager->StopRequest(requestId, bWait);
	}
	return bFlag;
}

string RequestBaseController::GetContentTypeById(long requestId) {
	string contentType = "";
	if( mpHttpRequestManager != NULL ) {
		const HttpRequest* request = mpHttpRequestManager->GetRequestById(requestId);
		if ( NULL != request ) {
			contentType = request->GetContentType();
		}
	}
	return contentType;
}

// 获取body总数及已下载数
void RequestBaseController::GetRecvLength(long requestId, int& total, int& recv)
{
	if (NULL != mpHttpRequestManager ) {
		const HttpRequest* request = mpHttpRequestManager->GetRequestById(requestId);
		if ( NULL != request ) {
			request->GetRecvDataCount(total, recv);
		}
	}
}

bool RequestBaseController::HandleResult(const char* buf, int size, string &errnum, string &errmsg, Json::Value *data, Json::Value *errdata) {
	bool bFlag = false;

	/* try to parse json */
	Json::Value root;
	Json::Reader reader;

	if( reader.parse(buf, root, false) ) {
		FileLog("httprequest", "RequestBaseController::HandleResult( parse Json finish )");
		if( root.isObject() ) {
			if( root[COMMON_RESULT].isInt() && root[COMMON_RESULT].asInt() == 1 ) {
				errnum = "";
				errmsg = "";
				if( data != NULL ) {
					*data = root[COMMON_DATA];
				}

				bFlag = true;
			} else {
				if( root[COMMON_ERRNO].isString() ) {
					errnum = root[COMMON_ERRNO].asString();
				}

				if( root[COMMON_ERRMSG].isString() ) {
					errmsg = root[COMMON_ERRMSG].asString();
				}

				if( errdata != NULL ) {
					*errdata = root[COMMON_ERRDATA];
				}

				bFlag = false;
			}
		}
	}

	FileLog("httprequest", "RequestBaseController::HandleResult( handle json result %s )", bFlag?"ok":"fail");

	return bFlag;
}

bool RequestBaseController::HandleResult(const char* buf, int size, string &errnum, string &errmsg, TiXmlDocument &doc) {
	bool bFlag = false;

	/* try to parse xml */
	TiXmlElement* itemElement;
	doc.Parse(buf);
	const char *p = NULL;

	if ( !doc.Error() ) {
		TiXmlNode *rootNode = doc.FirstChild(COMMON_ROOT);
		if( rootNode != NULL ) {
			TiXmlNode *resultNode = rootNode->FirstChild(COMMON_RESULT);
			if( resultNode != NULL ) {
				TiXmlNode *statusNode = resultNode->FirstChild(COMMON_STATUS);
				if( statusNode != NULL ) {
					itemElement = statusNode->ToElement();
					if ( itemElement != NULL ) {
						p = itemElement->GetText();
						if( p != NULL && 1 == atoi(p) ) {
							bFlag = true;
						}
					}
				}

				TiXmlNode *errcodeNode = resultNode->FirstChild(COMMON_ERRCODE);
				if( errcodeNode != NULL ) {
					itemElement = errcodeNode->ToElement();
					if ( itemElement != NULL ) {
						p = itemElement->GetText();
						if( p != NULL ) {
							errnum = p;
						}
					}
				}

				TiXmlNode *errmsgNode = resultNode->FirstChild(COMMON_ERRMSG);
				if( errmsgNode != NULL ) {
					itemElement = errmsgNode->ToElement();
					if ( itemElement != NULL ) {
						p = itemElement->GetText();
						if( p != NULL ) {
							errmsg = p;
						}
					}
				}
			}
		}
	} else {
		FileLog("httprequest", "RequestBaseController::HandleResult( Value() : %s, ErrorDesc() : %s, ErrorRow() : %d, ErrorCol() : %d )",
				doc.Value(), doc.ErrorDesc(), doc.ErrorRow(), doc.ErrorCol());
	}

	return bFlag;
}
