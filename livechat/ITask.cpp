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
#include "RecvShowPhotoTask.h"
#include "RecvLadyVoiceCodeTask.h"
#include "RecvIdentifyCodeTask.h"
#include "RecvVideoTask.h"
#include "RecvShowVideoTask.h"
#include "RecvAutoInviteTask.h"
#include "RecvAutoChargeResultTask.h"
#include "RecvMagicIconTask.h"
#include "RecvThemeMotionTask.h"
#include "RecvThemeRecommendTask.h"
#include "RecvLadyAutoInviteTask.h"
#include "RecvLadyAutoInviteStatusTask.h"
#include "RecvManFeeThemeTask.h"
#include "RecvManApplyThemeTask.h"
#include "RecvLadyCamStatusTask.h"
#include "RecvAcceptCamInviteTask.h"
#include "RecvCamShareInviteTask.h"
#include "RecvCamShareStartTask.h"
#include "RecvCamHearbeatExceptionTask.h"
#include <common/CheckMemoryLeak.h>

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
	case TCMD_RECVSHOWPHOTO:
		task = new RecvShowPhotoTask();
		break;
    case TCMD_RECVLADYVOICECODE:
        task = new RecvLadyVoiceCodeTask();
        break;
    case TCMD_RECVIDENTIFYCODE:
        task = new RecvIdentifyCodeTask();
        break;
    case TCMD_RECVVIDEO:
        task = new RecvVideoTask();
        break;
    case TCMD_RECVSHOWVIDEO:
    	task = new RecvShowVideoTask();
    	break;
    case TCMD_RECVAUTOINVITEMSG:
    	task = new RecvAutoInviteTask();
    	break;
    case TCMD_RECVAUTOCHARGE:
		task = new RecvAutoChargeResultTask();
		break;
    case TCMD_RECVMAGICICON:
    	task = new RecvMagicIconTask();
		break;
	case TCMD_RECVTHEMEMOTION:
		task = new RecvThemeMotionTask();
    	break;
	case TCMD_RECVTHEMERECOMMEND:
		task = new RecvThemeRecommendTask();
		break;
	case TCMD_RECVLADYAUTOINVITE:
		task = new RecvLadyAutoInviteTask();
		break;
	case TCMD_RECVLADYAUTOINVITESTATUS:
		task = new RecvLadyAutoInviteStatusTask();
		break;
	case TCMD_RECVMANFEETHEME:
		task = new RecvManFeeThemeTask();
		break;
	case TCMD_RECVMANAPPLYTHEME:
		task = new RecvManApplyThemeTask();
		break;
	case TCMD_RECVLADYCAMSTATUS:
		task = new RecvLadyCamStatusTask();
		break;
	case TCMD_RECVACCEPTCAMINVITE:
		task = new RecvAcceptCamInviteTask();
		break;
	case TCMD_RECVCAMSHAREINVITE:
		task = new RecvCamShareInviteTask();
		break;
	case TCMD_RECVCAMSHARESTART:
		task = new RecvCamShareStartTask();
	
	case TCMD_RECVCAMHEARBEATEXCEPTION:
		task = new RecvCamHearbeatExceptionTask();
	
	}

	return task;
}
