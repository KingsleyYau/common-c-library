// testCURL.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <livechatmanmanager/ILiveChatManManager.h>
#include <string>
#include <list>
#include <common/CheckMemoryLeak.h>
#include <common/list_lock.h>
#include <livechat/IThreadHandler.h>

using namespace std;

ILiveChatManManager* g_manager = NULL;

// ------ thread msg process ------
enum E_THREAD_MSG
{
	// unknow
	TM_UNKNOW,

	// photo
	TM_GETPHOTO,
	TM_PHOTOFEE,
	TM_SENDPHOTO,

	// video
	TM_GETVIDEOPHOTO,
	TM_VIDEOFEE,
	TM_GETVIDEO,
	TM_ISGETINGVIDEO
};
typedef struct _tagThreadMsgItem
{
	E_THREAD_MSG msg;
	unsigned long param1;
	unsigned long param2;

	_tagThreadMsgItem()
	{
		msg = TM_UNKNOW;
		param1 = 0;
		param2 = 0;
	}

	_tagThreadMsgItem(const _tagThreadMsgItem& item)
	{
		msg = item.msg;
		param1 = item.param1;
		param2 = item.param2;
	}
} ThreadMsgItem;
typedef list_lock<ThreadMsgItem> ThreadMsgList;
bool g_threadRun = false;
ThreadMsgList g_threadMsgList;
// ------ thread msg process ------

string GetMessageLog(LCMessageItem* item)
{
	string message = "";
	char buffer[20480] = {0};
	
	switch (item->m_msgType)
	{
	case LCMessageItem::MT_Text:
	{
		LCTextItem* textItem = item->GetTextItem();
		sprintf_s(buffer, sizeof(buffer), "message:%s, illegal:%d"
			, textItem->m_message.c_str(), textItem->m_illegal);
		message = buffer;
	} break;
	case LCMessageItem::MT_Emotion:
	{
		LCEmotionItem* emotionItem = item->GetEmotionItem();
		sprintf_s(buffer, sizeof(buffer)
			, "emotionId:%s, imagePath:%s, playBigPath:%s" \
			", playBigImages:%d"
			, emotionItem->m_emotionId.c_str(), emotionItem->m_imagePath.c_str(), emotionItem->m_playBigPath.c_str()
			, emotionItem->m_playBigPaths.size());
		message = buffer;
	} break;
	case LCMessageItem::MT_Voice:
	{
		LCVoiceItem* voiceItem = item->GetVoiceItem();
		sprintf_s(buffer, sizeof(buffer)
			, "voiceId:%s, charge:%d, timeLength:%d" \
			", fileType:%s, checkCode:%s, filePath:%s"
			, voiceItem->m_voiceId.c_str(), voiceItem->m_charge, voiceItem->m_timeLength
			, voiceItem->m_fileType.c_str(), voiceItem->m_checkCode.c_str(), voiceItem->m_filePath.c_str());
		message = buffer;
	} break;
	case LCMessageItem::MT_Photo:
	{
		LCPhotoItem* photoItem = item->GetPhotoItem();
		sprintf_s(buffer, sizeof(buffer)
			, "photoId:%s, charge:%b, sendId:%s" \
			", photoDesc:%s" \
			", showFuzzyFilePath:%s, thumbFuzzyFilePath:%s" \
			", showSrcFilePath:%s, thumbSrcFilePath:%s" \
			", srcFilePath:%s"
			, photoItem->m_photoId.c_str(), photoItem->m_charge, photoItem->m_sendId.c_str()
			, photoItem->m_photoDesc.c_str()
			, photoItem->m_showFuzzyFilePath.c_str(), photoItem->m_thumbFuzzyFilePath.c_str()
			, photoItem->m_showSrcFilePath.c_str(), photoItem->m_thumbSrcFilePath.c_str()
			, photoItem->m_srcFilePath.c_str());
		message = buffer;
	} break;
	case LCMessageItem::MT_Video:
	{
		lcmm::LCVideoItem* videoItem = item->GetVideoItem();
		sprintf_s(buffer, sizeof(buffer)
			, "videoId:%s, sendId:%s, videoUrl:%s, charge:%d"
			, videoItem->m_videoId.c_str()
			, videoItem->m_sendId.c_str()
			, videoItem->m_videoUrl.c_str()
			, videoItem->m_charge);
		message = buffer;
	} break;
	case LCMessageItem::MT_System:
	{
		LCSystemItem* systemItem = item->GetSystemItem();
		sprintf_s(buffer, sizeof(buffer)
			, "message:%s"
			, systemItem->m_message.c_str());
		message = buffer;
	} break;
	case LCMessageItem::MT_Warning:
	{
		LCWarningItem* warningItem = item->GetWarningItem();
		string linkItemMsg = "";
		if (NULL != warningItem->m_linkItem) {
			sprintf_s(buffer, sizeof(buffer)
				, "linkMsg:%s, linkOptType:%d"
				, warningItem->m_linkItem->m_linkMsg.c_str()
				, warningItem->m_linkItem->m_linkOptType);
			linkItemMsg = ", ";
			linkItemMsg += buffer;
		}
		sprintf_s(buffer, sizeof(buffer)
			, "message:%s%s"
			, warningItem->m_message.c_str()
			, linkItemMsg.c_str());
		message = buffer;
	} break;
	default:
	{}
	}
	
	string body("");
	sprintf_s(buffer, sizeof(buffer)
		, " fromId:%s, toId:%s, inviteId:%s, msgType:%d" \
		", createTime:%d, sendType:%d, statusType:%d" \
		", %s"
		, item->m_fromId.c_str(), item->m_toId.c_str(), item->m_inviteId.c_str(), item->m_msgType
		, item->m_createTime, item->m_sendType, item->m_statusType
		, message.c_str());
	body = buffer;
	
	return body;
}

class LiveChatManManagerListener : public ILiveChatManManagerListener
{
public:
	LiveChatManManagerListener() 
	{
		printf("LiveChatManManagerListener construct\n");

		m_sendEmotionIndex = 0;
	};
	virtual ~LiveChatManManagerListener() 
	{
		printf("LiveChatManManagerListener distruct\n");
	};

protected:
	// ------- message listener -------
	virtual void OnRecvEditMsg(const string& fromId) {
		printf("OnRecvEditMsg() fromId:%s\n", fromId.c_str());
	};
	virtual void OnRecvMessage(LCMessageItem* msgItem) {
		// ��ӡlog
		string message = GetMessageLog(msgItem);
		printf("OnRecvMessage() message:%s\n", message.c_str());

		// ������Ϣ
		const char* msg = "1.2.3.4.5";
		g_manager->SendTextMessage(msgItem->m_fromId, msg);
	};
	virtual void OnRecvSystemMsg(LCMessageItem* msgItem) {
		string message = GetMessageLog(msgItem);
		printf("OnRecvSystemMsg() message:%s\n", message.c_str());
	};
	virtual void OnRecvWarningMsg(LCMessageItem* msgItem) {
		string message = GetMessageLog(msgItem);
		printf("OnRecvWarningMsg() message:%s\n", message.c_str());
	};
	virtual void OnSendTextMessage(LCC_ERR_TYPE errType, const string& errMsg, LCMessageItem* msgItem) {
		string message = GetMessageLog(msgItem);
		printf("OnSendTextMessage() errType:%d, errMsg:%s, message:%s\n"
			, errType, errMsg.c_str(), message.c_str());
	};
	virtual void OnSendMessageListFail(LCC_ERR_TYPE errType, const LCMessageList& msgList) {
		printf("OnSendMessageListFail() errType:%d, msgList.size:%d\n"
			, errType, msgList.size());
	};

	// ------- emotion listener -------
	virtual void OnGetEmotionConfig(bool success, const string& errNo, const string& errMsg, const OtherEmotionConfigItem& config) {
		string emotionLog("");
		for (OtherEmotionConfigItem::EmotionList::const_iterator iter = config.manEmotionList.begin();
			iter != config.manEmotionList.end();
			iter++)
		{
			if (!emotionLog.empty()) {
				emotionLog += ",";
			}
			emotionLog += (*iter).fileName;
		}

		printf("OnGetEmotionConfig() success:%d, errNo:%s, errMsg:%s, path:%s, emotionList:%s\n"
			, success, errNo.c_str(), errMsg.c_str()
			, config.path.c_str(), emotionLog.c_str());

		if (success) {
			m_emotionConfig = config;
		}
	};
	virtual void OnGetEmotionImage(bool success, const LCEmotionItem* item) {
		printf("OnGetEmotionImage() success:%d, emotionId:%s, imagePath:%s, playBigPath:%s, playPath:%d\n"
			, success, item->m_emotionId.c_str(), item->m_imagePath.c_str(), item->m_playBigPath.c_str(), item->m_playBigPaths.size());
	};
	virtual void OnGetEmotionPlayImage(bool success, const LCEmotionItem* item) {
		printf("OnGetEmotionPlayImage() success:%d, emotionId:%s, imagePath:%s, playBigPath:%s, playPath:%d\n"
			, success, item->m_emotionId.c_str(), item->m_imagePath.c_str(), item->m_playBigPath.c_str(), item->m_playBigPaths.size());
	};
	virtual void OnRecvEmotion(LCMessageItem* msgItem) {
		// ��ӡlog
		string message = GetMessageLog(msgItem);
		printf("OnRecvEmotion() message:%s\n", message.c_str());

		// ��ȡ����ͼ
		if (msgItem->GetEmotionItem()->m_imagePath.empty()) {
			g_manager->GetEmotionImage(msgItem->GetEmotionItem()->m_emotionId);
		}

		// ��ȡ����ͼ
		if (msgItem->GetEmotionItem()->m_playBigPath.empty()) {
			g_manager->GetEmotionPlayImage(msgItem->GetEmotionItem()->m_emotionId);
		}

		// ���͸߼�����
		OtherEmotionConfigItem::EmotionItem emotionItem;
		OtherEmotionConfigItem::EmotionList::iterator iter;
		int i;
		for (i = 0, iter = m_emotionConfig.manEmotionList.begin();
			iter != m_emotionConfig.manEmotionList.end() && i <= m_sendEmotionIndex;
			iter++, i++)
		{
			emotionItem = (*iter);
		}
		m_sendEmotionIndex = (m_sendEmotionIndex + 1) % m_emotionConfig.manEmotionList.size();
		g_manager->SendEmotion(msgItem->m_fromId, emotionItem.fileName);
	};
	virtual void OnSendEmotion(LCC_ERR_TYPE errType, const string& errMsg, LCMessageItem* msgItem) {
		// ��ӡlog
		string message = GetMessageLog(msgItem);
		printf("OnSendEmotion() errType:%d, errMsg:%s, message:%s\n"
			, errType, errMsg.c_str(), message.c_str());

		// ��ȡ����ͼ
		if (msgItem->GetEmotionItem()->m_imagePath.empty()) {
			g_manager->GetEmotionImage(msgItem->GetEmotionItem()->m_emotionId);
		}

		// ��ȡ����ͼ
		if (msgItem->GetEmotionItem()->m_playBigPath.empty()) {
			g_manager->GetEmotionPlayImage(msgItem->GetEmotionItem()->m_emotionId);
		}
	};

	// ------- voice listener -------
	virtual void OnGetVoice(LCC_ERR_TYPE errType, const string& errNo, const string& errMsg, LCMessageItem* msgItem) {
		// ��ȡ�����ļ��ɹ�
		if (errType == LCC_ERR_SUCCESS) {
			// ���������ļ�
			g_manager->SendVoice(
				msgItem->m_fromId
				, msgItem->GetVoiceItem()->m_filePath
				, msgItem->GetVoiceItem()->m_fileType
				, msgItem->GetVoiceItem()->m_timeLength);
		}
	};
	virtual void OnRecvVoice(LCMessageItem* msgItem) {
		// ��ȡ�����ļ�
		g_manager->GetVoice(msgItem->m_fromId, msgItem->m_msgId);
	};
	virtual void OnSendVoice(LCC_ERR_TYPE errType, const string& errNo, const string& errMsg, LCMessageItem* msgItem) {
		// ���ͳɹ�
	};

	// ------- photo listener -------
	virtual void OnGetPhoto(LCC_ERR_TYPE errType, const string& errNo, const string& errMsg, LCMessageItem* msgItem) {
		
		if (!msgItem->GetPhotoItem()->m_charge
			&& !msgItem->GetPhotoItem()->m_showFuzzyFilePath.empty())
		{
			// ����ͼƬ��ͼƬδ�����ѻ�ȡ��ͼ��
			ThreadMsgItem msgItem1;
			msgItem1.msg = TM_PHOTOFEE;
			msgItem1.param1 = (unsigned long)msgItem;
			g_threadMsgList.push_back(msgItem1);
		}
		else if (msgItem->GetPhotoItem()->m_charge
			&& !msgItem->GetPhotoItem()->m_srcFilePath.empty())
		{
			// ����ͼƬ��ͼƬ�Ѿ��������ѻ�ȡԭͼ��
			ThreadMsgItem msgItem1;
			msgItem1.msg = TM_SENDPHOTO;
			msgItem1.param1 = (unsigned long)msgItem;
			g_threadMsgList.push_back(msgItem1);
		}
	};
	virtual void OnPhotoFee(bool success, const string& errNo, const string& errMsg, LCMessageItem* msgItem) {
		// ����ɹ�
		if (success) {
			// ��ȡԭͼ
			ThreadMsgItem msgItem1;
			msgItem1.msg = TM_GETPHOTO;
			msgItem1.param1 = (unsigned long)msgItem;
			msgItem1.param2 = GPT_ORIGINAL;
			g_threadMsgList.push_back(msgItem1);

			// ��ȡ��ͼ
			ThreadMsgItem msgItem2;
			msgItem2.msg = TM_GETPHOTO;
			msgItem2.param1 = (unsigned long)msgItem;
			msgItem2.param2 = GPT_LARGE;
			g_threadMsgList.push_back(msgItem2);
		}
	};
	virtual void OnRecvPhoto(LCMessageItem* msgItem) {
		// ��ȡСͼ
		ThreadMsgItem msgItem1;
		msgItem1.msg = TM_GETPHOTO;
		msgItem1.param1 = (unsigned long)msgItem;
		msgItem1.param2 = GPT_SMALL;
		g_threadMsgList.push_back(msgItem1);

		// ��ȡ��ͼ
		ThreadMsgItem msgItem2;
		msgItem2.msg = TM_GETPHOTO;
		msgItem2.param1 = (unsigned long)msgItem;
		msgItem2.param2 = GPT_LARGE;
		g_threadMsgList.push_back(msgItem2);
	};
	virtual void OnSendPhoto(LCC_ERR_TYPE errType, const string& errNo, const string& errMsg, LCMessageItem* msgItem) {
		// ����ͼƬ�ɹ�
	};

	// ------- video listener -------
	virtual void OnGetVideo(
			LCC_ERR_TYPE errType
			, const string& userId
			, const string& videoId
			, const string& inviteId
			, const string& videoPath
			, const LCMessageList& msgList) 
	{
			LCMessageItem* item = *(msgList.begin());
			string* pVideoId = new string;
			*pVideoId = item->GetVideoItem()->m_videoId;

			// ��ȡ����״̬
			ThreadMsgItem msgItem1;
			msgItem1.msg = TM_ISGETINGVIDEO;
			msgItem1.param1 = (unsigned long)pVideoId;
			g_threadMsgList.push_back(msgItem1);
	};
	virtual void OnGetVideoPhoto(
			LCC_ERR_TYPE errType
			, const string& errNo
			, const string& errMsg
			, const string& userId
			, const string& inviteId
			, const string& videoId
			, VIDEO_PHOTO_TYPE
			, const string& filePath
			, const LCMessageList& msgList) 
	{
			// ������Ƶ
			ThreadMsgItem msgItem1;
			msgItem1.msg = TM_VIDEOFEE;
			msgItem1.param1 = (unsigned long)*(msgList.begin());
			g_threadMsgList.push_back(msgItem1);
	};
	virtual void OnRecvVideo(LCMessageItem* msgItem) 
	{
		bool result = false;

		// ��ȡ��ƵͼƬ
		LCUserItem* userItem = msgItem->GetUserItem();
		lcmm::LCVideoItem* videoItem = msgItem->GetVideoItem();
		result = g_manager->GetVideoPhoto(userItem->m_userId, videoItem->m_videoId, userItem->m_inviteId);

		// ������Ƶ
		//result = g_manager->VideoFee(userId, msgItem->m_msgId);
	};
	virtual void OnVideoFee(bool success, const string& errNo, const string& errMsg, LCMessageItem* msgItem) 
	{
		bool result = false;

		// ��ȡ��Ƶ
		{
			ThreadMsgItem msgItem1;
			msgItem1.msg = TM_GETVIDEO;
			msgItem1.param1 = (unsigned long)msgItem;
			g_threadMsgList.push_back(msgItem1);
		}

		// ��ȡ����״̬
		{
			string* pVideoId = new string;
			*pVideoId = msgItem->GetVideoItem()->m_videoId;

			ThreadMsgItem msgItem1;
			msgItem1.msg = TM_ISGETINGVIDEO;
			msgItem1.param1 = (unsigned long)pVideoId;
			msgItem1.param2 = 3;
			g_threadMsgList.push_back(msgItem1);
		}
	};

	// ------- other listener -------
	virtual void OnChangeOnlineStatus(LCUserItem* userItem) {};
	virtual void OnGetHistoryMessage(bool success, const string& errNo, const string& errMsg, LCUserItem* userItem) {};
	virtual void OnGetUsersHistoryMessage(bool success, const string& errNo, const string& errMsg, const LCUserList& userList) {};
	virtual void OnGetTalkList(LCC_ERR_TYPE errType, const string& errMsg) {};
	virtual void OnGetUserStatus(LCC_ERR_TYPE errType, const string& errMsg, const LCUserList& userList) {};
	virtual void OnLogin(LCC_ERR_TYPE errType, const string& errMsg, bool isAutoLogin) 
	{
		printf("OnLogin() errType:%d, errMsg:%s, isAutoLogin:%d", errType, errMsg.c_str(), isAutoLogin);
	};
	virtual void OnLogout(LCC_ERR_TYPE errType, const string& errMsg, bool isAutoLogin) {};
	virtual void OnRecvEMFNotice(const string& fromId, TALK_EMF_NOTICE_TYPE noticeType) {};
	virtual void OnRecvKickOffline(KICK_OFFLINE_TYPE kickType) {};
	virtual void OnSetStatus(LCC_ERR_TYPE errType, const string& errMsg) {};
	virtual void OnUpdateStatus(LCUserItem* userItem) {};
	virtual void OnEndTalk(LCC_ERR_TYPE errType, const string& errMsg, LCUserItem* userItem) {};
	virtual void OnRecvTalkEvent(LCUserItem* userItem) {};
	virtual void OnRecvTryTalkBegin(LCUserItem* userItem, int time) {};
	virtual void OnRecvTryTalkEnd(LCUserItem* userItem) {};
	virtual void OnUseTryTicket(LCC_ERR_TYPE err, const string& errmsg, const string& userId, TRY_TICKET_EVENT tickEvent) {};

	// ------- user info listener -------
	virtual void OnGetUserInfo(const string& userId, LCC_ERR_TYPE errType, const string& errMsg, const UserInfoItem& userInfo) {};
	virtual void OnGetUsersInfo(LCC_ERR_TYPE errType, const string& errMsg, const UserInfoList& userList) {};

	// ------- other http listener -------
	virtual void OnCheckCoupon(bool success, const string& errNo, const string& errMsg, const string& userId, CouponStatus status) {};

private:
	OtherEmotionConfigItem m_emotionConfig;
	int m_sendEmotionIndex;
};

LiveChatManManagerListener g_listener;

bool Init(ILiveChatManManager* manager, bool isDemo)
{
	list<string> ips;
	int port;

	if (isDemo)
	{
		ips.push_back("58.64.141.117");
		port = 5000;
	}
	else
	{
		ips.push_back("192.168.8.141");
		port = 5001;
	}

	OTHER_SITE_TYPE siteType = OTHER_SITE_CL;
	string webHost = "http://demo.chnlove.com";
	string appHost = "http://demo-mobile.chnlove.com";
	string chatVoiceHost = "http://58.64.141.117:9901";

	string httpUser = "test";
	string httpPassword = "5179";

	double minBalance = 0.5;
	string appVer = "320";

	// E:\Work\QpidNetwork\src\testCURL\Debug
	//string path = "E:\\Work\\QpidNetwork\\src\\testCURL\\Debug";

	// E:\Work\QpidNetwork\src\common-c-library\livechatmanmanager_t\Debug
	char fileName[MAX_PATH] = {0};
	GetModuleFileName(NULL, fileName, sizeof(fileName));
	char* strEnd = strrchr(fileName, '\\');
	if (NULL != strEnd) {
		*strEnd = '\0';
	}
	string path = fileName;

	return manager->Init(ips, port, siteType
				, webHost, appHost, chatVoiceHost
				, httpUser, httpPassword
				, appVer, path, minBalance
				, &g_listener);
}

bool Login(ILiveChatManManager* manager, const string& userId, const string& sId)
{
	//string userId = "CM42137154";
	//string sId = "11111";

	list<string> cookies;
	cookies.push_back("demo-mobile.chnlove.com	FALSE	/	FALSE	0	PHPSESSID	ihrnv09kbfu6n0cqjkhibqd7a4");
	cookies.push_back(".chnlove.com	TRUE	/	FALSE	1467020352	CHNPOPUREG	samson.fan%40qpidnetwork.com");
	cookies.push_back("demo-mobile.chnlove.com	FALSE	/member/	FALSE	1	CHNCOOKSTA	deleted");

	return manager->Login(userId, sId, cookies, "866501010242493", true);
}

static TH_RETURN_PARAM ThreadMsgProc(void* arg)
{
	while (g_threadRun)
	{
		g_threadMsgList.lock();
		ThreadMsgList::iterator iter = g_threadMsgList.begin();
		if (iter != g_threadMsgList.end())
		{
			ThreadMsgItem threadMsgItem = (*iter);
			switch (threadMsgItem.msg)
			{
			case TM_GETPHOTO:
				{
					LCMessageItem* msgItem = (LCMessageItem*)threadMsgItem.param1;
					GETPHOTO_PHOTOSIZE_TYPE sizeType = (GETPHOTO_PHOTOSIZE_TYPE)threadMsgItem.param2;
					g_manager->GetPhoto(msgItem->m_fromId, msgItem->m_msgId, sizeType);
				}break;
			case TM_PHOTOFEE:
				{
					LCMessageItem* msgItem = (LCMessageItem*)threadMsgItem.param1;
					g_manager->PhotoFee(msgItem->m_fromId, msgItem->m_msgId);
				}break;
			case TM_SENDPHOTO:
				{
					LCMessageItem* msgItem = (LCMessageItem*)threadMsgItem.param1;
					g_manager->SendPhoto(msgItem->m_fromId, msgItem->GetPhotoItem()->m_srcFilePath);
				}break;
			case TM_GETVIDEOPHOTO:
				{
					LCMessageItem* msgItem = (LCMessageItem*)threadMsgItem.param1;
					g_manager->GetVideoPhoto(msgItem->m_fromId, msgItem->GetVideoItem()->m_videoId, msgItem->m_inviteId);
				}break;
			case TM_VIDEOFEE:
				{
					LCMessageItem* msgItem = (LCMessageItem*)threadMsgItem.param1;
					g_manager->VideoFee(msgItem->m_fromId, msgItem->m_msgId);
				}break;
			case TM_GETVIDEO:
				{
					LCMessageItem* msgItem = (LCMessageItem*)threadMsgItem.param1;
					g_manager->GetVideo(msgItem->m_fromId, msgItem->GetVideoItem()->m_videoId, msgItem->m_inviteId, msgItem->GetVideoItem()->m_videoUrl);
				}break;
			case TM_ISGETINGVIDEO:
				{
					if (threadMsgItem.param2 > 0) {
						Sleep(threadMsgItem.param2 * 1000);
					}

					string* videoId = (string*)threadMsgItem.param1;
					g_manager->IsGetingVideo(*videoId);
					delete videoId;
				}break;
			default:
				break;
			}

			g_threadMsgList.pop_front();
		}
		else {
			Sleep(50);
		}
		g_threadMsgList.unlock();
	}

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	bool result = false;

	// ��ʼ���߳�
	IThreadHandler* pThreadHandler = IThreadHandler::CreateThreadHandler();
	g_threadRun = true;
	pThreadHandler->Start(ThreadMsgProc, NULL);

	// ��������ʼ��
	g_manager = ILiveChatManManager::Create();
	int isDemo = 1;
	printf("isDemo[true:1/false:0]:");
	scanf("%d", &isDemo);
	result = Init(g_manager, isDemo!=0);
	printf("Init() reuslt:%d\n", result);

	// ��¼
	printf("\n\n");
	char account[512] = {0};
	strcpy(account, "CM28171208");
	char password[512] = {0};
	strcpy(account, "1234");

	printf("account: ");
	scanf("%s", &account);

	printf("password: ");
	scanf("%s", &password);

	result = Login(g_manager, account, password);
	printf("Login() result:%d\n", result);

	system("pause");

	Sleep(2 * 1000);
	g_manager->Logout(true);
	ILiveChatManManager::Release(g_manager);

	//system("pause");
	g_threadRun = false;
	pThreadHandler->WaitAndStop();
	IThreadHandler::ReleaseThreadHandler(pThreadHandler);

	OutputMemoryLeakInfo();

	system("pause");

	return 0;
}

