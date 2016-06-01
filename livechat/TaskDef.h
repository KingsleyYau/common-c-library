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
	TCMD_SENDLADYPHOTO		= 180,	// 女士发送图片
	TCMD_GETRECENTCONTACTLIST	= 85,	// 获取最近联系人列表
	TCMD_SEARCHONLINEMAN	= 98,	// 搜索在线男士
	TCMD_REPLYIDENTIFYCODE  = 105,	// 回复验证码
	TCMD_REFRESHIDENTIFYCODE    = 106,	// 刷新验证码
	TCMD_REFRESHINVITETEMPLATE  = 161,	// 刷新邀请模板
	TCMD_GETFEERECENTCONTACTLIST= 162,	// 获取已扣费最近联系人列表
	TCMD_GETLADYCHATINFO    = 170,	// 获取女士聊天信息（包括在聊及邀请的男士列表等）
	TCMD_PLAYVIDEO          = 194,  // 播放微视频
	TCMD_SENDLADYVIDEO		= 196,	// 女士发送微视频
	TCMD_GETLADYCONDITION	= 214,	// 获取女士择偶条件
	TCMD_GETLADYCUSTOMTEMPLATE	= 215,  //获取女士自定义模板
	TCMD_UPLOADLADYAUTOINVITE	= 210,	// 上传弹出女士自动邀请消息
	TCMD_UPLOADAUTOCHARGE   = 238,	// 上传自动充值状态
	TCMD_SENDMAGICICON	    = 200,	// 发送小高级表情
	TCMD_GETPAIDTHEME		= 219,	// 获取指定男/女士已购主题包
	TCMD_GETALLPAIDTHEME	= 220,	// 获取男/女士所有已购主题包
	TCMD_UPLOADTHEMELISTVER = 225,	// 上传主题包列表版本号
	TCMD_MANFEETHEME		= 218,	// 男士购买主题包
	TCMD_MANAPPLYTHEME		= 223,	// 男士应用主题包
	TCMD_PLAYTHEMEMOTION	= 227,	// 男/女士播放主题包动画
	// 服务器主动请求命令
	TCMD_RECVMSG		    = 24,	// 文字聊天信息通知
	TCMD_RECVEMOTION	    = 101,	// 高级表情聊天信息通知
	TCMD_RECVVOICE		    = 153,	// 语音聊天信息通知
	TCMD_RECVWARNING	    = 122,	// 警告信息通知
	TCMD_UPDATESTATUS	    = 28,	// 状态更新通知
	TCMD_UPDATETICKET	    = 109,	// 票根更新通知
	TCMD_RECVEDITMSG	    = 45,	// 对方正在编辑通知
	TCMD_RECVTALKEVENT	    = 39,	// 聊天事件通知
	TCMD_RECVTRYBEGIN	    = 58,	// 试聊开始通知
	TCMD_RECVTRYEND		    = 59,	// 试聊结束通知
	TCMD_RECVEMFNOTICE	    = 53,	// 邮件更新通知
	TCMD_RECVKICKOFFLINE    = 27,	// 服务器维护及被踢通知
	TCMD_RECVPHOTO		    = 177,	// 图片信息通知
	TCMD_RECVSHOWPHOTO		= 179,	// 图片被查看通知
	TCMD_RECVIDENTIFYCODE   = 104,	// 验证码通知
	TCMD_RECVLADYVOICECODE  = 157,	// 女士语音编码通知
	TCMD_RECVVIDEO          = 193,  // 微视频信息通知
	TCMD_RECVSHOWVIDEO		= 192,	// 微视频被查看通知
	TCMD_RECVAUTOINVITEMSG	= 209,	// 自动邀请消息通知(仅男士)
	TCMD_RECVAUTOCHARGE	    = 236,	// 自动充值服务器通知(仅男士)
	TCMD_RECVMAGICICON	    = 199,	// 小高级表情聊天信息通知
	TCMD_RECVTHEMEMOTION	= 229,	// 播放主题包动画通知
	TCMD_RECVTHEMERECOMMEND	= 233,	// 女士推荐男士购买主题包通知(仅男士)
} TASK_CMD_TYPE;

// 判断是否客户端主动请求的命令
inline bool IsRequestCmd(int cmd)
{
	bool result = false;
	switch (cmd) {
	case TCMD_CHECKVER:				// 版本检测
	case TCMD_LOGIN:				// 登录
	case TCMD_GETUSERINFO:			// 获取用户信息
	case TCMD_GETUSERSINFO:			// 获取多个用户信息
	case TCMD_GETCONTACTLIST:		// 获取联系人或黑名单列表
	case TCMD_GETBLOCKUSERS:		// 获取被屏蔽女士列表
	case TCMD_SETSTATUS:			// 设置在线状态
	case TCMD_UPLOADDEVID:			// 上传设备ID
	case TCMD_UPLOADDEVTYPE:		// 上传设备类型
	case TCMD_ENDTALK:				// 结束聊天
	case TCMD_GETUSERSTATUS:		// 获取用户在线状态
	case TCMD_GETTALKINFO:			// 获取会话信息
	case TCMD_UPLOADTICKET:			// 上传票根
	case TCMD_SENDLADYEDITINGMSG:	// 通知对方女士正在编辑消息
	case TCMD_SENDMSG:				// 发送聊天消息
	case TCMD_SENDEMOTION:			// 发送高级表情
	case TCMD_SENDVGIFT:			// 发送虚拟礼物
	case TCMD_GETVOICECODE:			// 获取语音发送验证码
	case TCMD_GETLADYVOICECODE:		// 获取女士语音发送验证码
	case TCMD_SENDVOICE:			// 发送语音
	case TCMD_USETRYTICKET:			// 使用试聊券
	case TCMD_GETTALKLIST:			// 获取邀请或在聊列表
	case TCMD_UPLOADVER:			// 上传客户端版本号
	case TCMD_SENDPHOTO:			// 发送图片
	case TCMD_SENDLADYPHOTO:		// 女士发送图片
	case TCMD_SHOWPHOTO:			// 显示图片
	case TCMD_GETRECENTCONTACTLIST:	// 获取最近联系人列表
	case TCMD_SEARCHONLINEMAN:		// 搜索在线男士
	case TCMD_REPLYIDENTIFYCODE:	// 回复验证码
	case TCMD_REFRESHIDENTIFYCODE:	// 刷新验证码
	case TCMD_REFRESHINVITETEMPLATE:	// 刷新邀请模板
	case TCMD_GETFEERECENTCONTACTLIST:	// 获取已扣费最近联系人列表
	case TCMD_GETLADYCHATINFO:		// 获取女士聊天信息（包括在聊及邀请的男士列表等）
	case TCMD_PLAYVIDEO:			// 播放视频
	case TCMD_SENDLADYVIDEO:		// 女士发送微视频
	case TCMD_GETLADYCONDITION:		// 获取女士择偶条件
	case TCMD_GETLADYCUSTOMTEMPLATE:// 获取女士自定义模板
	case TCMD_UPLOADLADYAUTOINVITE:	// 上传弹出女士自动邀请消息
	case TCMD_UPLOADAUTOCHARGE:		// 上传自动充值状态
	case TCMD_SENDMAGICICON:		// 发送小高级表情
	case TCMD_GETPAIDTHEME:			// 获取指定男/女士已购主题包
	case TCMD_GETALLPAIDTHEME:		// 获取男/女士所有已购主题包
	case TCMD_UPLOADTHEMELISTVER:	// 上传主题包列表版本号
	case TCMD_MANFEETHEME:			// 男士购买主题包
	case TCMD_MANAPPLYTHEME:		// 男士应用主题包
	case TCMD_PLAYTHEMEMOTION:		// 男/女士播放主题包动画
		result = true;	// 主动请求的命令
		break;
	default:
		break;
	}
	return result;
}
