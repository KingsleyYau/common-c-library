/*
 * author: Samson.Fan
 *   date: 2015-03-25
 *   file: TaskDef.h
 *   desc: 记录Task的定义
 */

#pragma once

// 使用协议定义
typedef enum {
	NOHEAD_PROTOCOL = -1,   // 只有长度没有包头
	JSON_PROTOCOL = 0,      // json协议
	AMF_PROTOCOL = 1        // amf协议
} TASK_PROTOCOL_TYPE;

// 命令定义
typedef enum {
	TCMD_UNKNOW			    = 0,	// 未知命令
	// 客户端主动请求命令
	TCMD_CHECKVER		    = -1,	// 版本检测
	TCMD_LOGIN			    = 9,	// 登录
	TCMD_GETUSERINFO	    = 10,	// 获取用户信息
	TCMD_GETUSERSINFO       = 12,	// 获取多个用户信息
	TCMD_GETCONTACTLIST	    = 11,	// 获取联系人或黑名单列表
	TCMD_GETBLOCKUSERS	    = 168,	// 获取被屏蔽女士列表
	TCMD_SETSTATUS		    = 17,	// 设置在线状态
	TCMD_UPLOADDEVID	    = 84,	// 上传设备ID
	TCMD_UPLOADDEVTYPE	    = 148,	// 上传设备类型
	TCMD_ENDTALK		    = 36,	// 结束聊天
	TCMD_GETUSERSTATUS	    = 50,	// 获取用户在线状态
	TCMD_GETTALKINFO	    = 87,	// 获取会话信息
	TCMD_UPLOADTICKET	    = 108,	// 上传票根
	TCMD_SENDLADYEDITINGMSG	= 47,	// 通知对方女士正在编辑消息
	TCMD_SENDMSG		    = 15,	// 发送聊天消息
	TCMD_SENDEMOTION	    = 41,	// 发送高级表情
	TCMD_SENDVGIFT		    = 142,	// 发送虚拟礼物
	TCMD_GETVOICECODE	    = 158,	// 获取语音发送验证码
	TCMD_GETLADYVOICECODE	= 159,	// 获取女士语音发送验证码
	TCMD_SENDVOICE		    = 152,	// 发送语音
	TCMD_USETRYTICKET	    = 80,	// 使用试聊券
	TCMD_GETTALKLIST	    = 91,	// 获取邀请或在聊列表
	TCMD_UPLOADVER		    = 163,	// 上传客户端版本号
	TCMD_SENDPHOTO		    = 181,	// 发送图片
	TCMD_SHOWPHOTO		    = 182,	// 显示图片
	TCMD_GETRECENTCONTACTLIST	= 85,	// 获取最近联系人列表
	TCMD_SEARCHONLINEMAN	= 98,	// 搜索在线男士
	TCMD_REPLYIDENTIFYCODE  = 105,	// 回复验证码
	TCMD_REFRESHIDENTIFYCODE    = 106,	// 刷新验证码
	TCMD_REFRESHINVITETEMPLATE  = 161,	// 刷新邀请模板
	TCMD_GETFEERECENTCONTACTLIST= 162,	// 获取已扣费最近联系人列表
	TCMD_GETLADYCHATINFO    = 170,	// 获取女士聊天信息（包括在聊及邀请的男士列表等）
	// 服务器主动请求命令
	TCMD_RECVMSG		    = 24,	// 文字聊天信息通知
	TCMD_RECVEMOTION	    = 101,	// 高级表情聊天信息通知
	TCMD_RECVVOICE		    = 153,	// 语音聊天信息通知
	TCMD_RECVWARNING	    = 121,	// 警告信息通知
	TCMD_UPDATESTATUS	    = 28,	// 状态更新通知
	TCMD_UPDATETICKET	    = 109,	// 票根更新通知
	TCMD_RECVEDITMSG	    = 45,	// 对方正在编辑通知
	TCMD_RECVTALKEVENT	    = 39,	// 聊天事件通知
	TCMD_RECVTRYBEGIN	    = 58,	// 试聊开始通知
	TCMD_RECVTRYEND		    = 59,	// 试聊结束通知
	TCMD_RECVEMFNOTICE	    = 53,	// 邮件更新通知
	TCMD_RECVKICKOFFLINE    = 27,	// 服务器维护及被踢通知
	TCMD_RECVPHOTO		    = 177,	// 图片信息通知
	TCMD_RECVIDENTIFYCODE   = 104,	// 验证码通知
	TCMD_RECVLADYVOICECODE  = 157,	// 女士语音编码通知
} TASK_CMD_TYPE;

// 判断是否客户端主动请求的命令
inline bool IsRequestCmd(int cmd)
{
	bool result = false;
	switch (cmd) {
	case TCMD_CHECKVER:
	case TCMD_LOGIN:
	case TCMD_SETSTATUS:
	case TCMD_UPLOADDEVID:
	case TCMD_UPLOADDEVTYPE:
	case TCMD_ENDTALK:
	case TCMD_GETUSERSTATUS:
	case TCMD_GETTALKINFO:
	case TCMD_UPLOADTICKET:
    case TCMD_SENDLADYEDITINGMSG:
	case TCMD_SENDMSG:
	case TCMD_SENDEMOTION:
	case TCMD_SENDVGIFT:
	case TCMD_GETVOICECODE:
	case TCMD_SENDVOICE:
	case TCMD_USETRYTICKET:
	case TCMD_GETTALKLIST:
	case TCMD_SENDPHOTO:
	case TCMD_SHOWPHOTO:
	case TCMD_GETCONTACTLIST:
	case TCMD_UPLOADVER:
	case TCMD_GETUSERINFO:
	case TCMD_GETBLOCKUSERS:
    case TCMD_GETRECENTCONTACTLIST:
    case TCMD_SEARCHONLINEMAN:
    case TCMD_REPLYIDENTIFYCODE:
    case TCMD_REFRESHIDENTIFYCODE:
    case TCMD_REFRESHINVITETEMPLATE:
    case TCMD_GETFEERECENTCONTACTLIST:
    case TCMD_GETLADYCHATINFO:
		result = true;	// 主动请求的命令
		break;
	default:
		break;
	}
	return result;
}
