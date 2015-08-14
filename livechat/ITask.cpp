/*
 * author: Samson.Fan
 *   date: 2015-03-24
 *   file: ITask.h
 *   desc: Task（任务）接口类，一般情况下每个Task对应处理一个协议
 */

#include "ITask.h"

// 服务器主动请求的task
#include "RecvMsgTask.h"
#include "RecvEmotionTask.h"
#include "RecvVoiceTask.h"
#include "RecvWarningTask.h"
#include "UpdateStatusTask.h"
#include "UpdateTicketTask.h"
#include "RecvEditMsgTask.h"
#include "RecvTalkEventTask.h"
#include "RecvTryTalkBeginTask.h"
#include "RecvTryTalkEndTask.h"
#include "RecvEMFNoticeTask.h"
#include "RecvKickOfflineTask.h"
#include "RecvPhotoTask.h"
#include "RecvLadyVoiceCodeTask.h"
#include "RecvIdentifyCodeTask.h"

// 根据 cmd 创建 task
ITask* ITask::CreateTaskWithCmd(int cmd)
{
	ITask* task = NULL;
	switch (cmd) {
	case TCMD_RECVMSG:
		task = new RecvMsgTask();
		break;
	case TCMD_RECVEMOTION:
		task = new RecvEmotionTask();
		break;
	case TCMD_RECVVOICE:
		task = new RecvVoiceTask();
		break;
	case TCMD_RECVWARNING:
		task = new RecvWarningTask();
		break;
	case TCMD_UPDATESTATUS:
		task = new UpdateStatusTask();
		break;
	case TCMD_UPDATETICKET:
		task = new UpdateTicketTask();
		break;
	case TCMD_RECVEDITMSG:
		task = new RecvEditMsgTask();
		break;
	case TCMD_RECVTALKEVENT:
		task = new RecvTalkEventTask();
		break;
	case TCMD_RECVTRYBEGIN:
		task = new RecvTryTalkBeginTask();
		break;
	case TCMD_RECVTRYEND:
		task = new RecvTryTalkEndTask();
		break;
	case TCMD_RECVEMFNOTICE:
		task = new RecvEMFNoticeTask();
		break;
	case TCMD_RECVKICKOFFLINE:
		task = new RecvKickOfflineTask();
		break;
	case TCMD_RECVPHOTO:
		task = new RecvPhotoTask();
		break;
    case TCMD_RECVLADYVOICECODE:
        task = new RecvLadyVoiceCodeTask();
        break;
    case TCMD_RECVIDENTIFYCODE:
        task = new RecvIdentifyCodeTask();
        break;
	}

	return task;
}
