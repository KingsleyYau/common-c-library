/*
*author:Alex shum
*  date:2016-8-15
*  file:CommonFuncCmdHandle.h
*  desc:命令处理类共同用到的一些函数，多数是打印命令格式
*/
//#ifndef COMMONFUNCCMDHANDLE_H_
//#define COMMONFUNCCMDHANDLE_H_
#pragma once
#include <list>
#include "CmdHandleDef.h"
#include <common/CommonFunc.h>
using namespace std;

//打印退出命令
void ExitInfo();
//打印初始化命令
void InitInfo();
//打印上传版本号命令
void UploadVerInfo();
//打印登录命令
void LoginInfo();
//打印登出命令
void LogoutInfo();
//打印发送消息命令
void SendTextInfo();
//打印循环发送消息命令
void SendLoopTextInfo();
//打印高级表情命令
void SendEmotionInfo();
//打印循环表情命令
void SendLoopEmotionInfo();
//打印小表情命令
void SendMagicIconInfo();
//打印获取用户信息命令
void GetUserInfo();
//打印上传主题版本命令
void UploadThemeVerInfo();
//打印用户主题包命令
void GetUserThemeInfo();
//打印获取所有主题包命令
void GetAllThemeInfo();
//打印购买主题包命令
void BuyThemeInfo();
//打印应用主题包命令
void ApplyThemeInfo();
//打印播放主题包动画命令
void PlayThemeInfo();
//打印TalkList命令
void GetTalkListInfo();
//打印自动邀请状态
void GetAutoInviteStatusInfo();
//打印自动邀请消息
void SendAutoInviteInfo();
//打印女士推荐男士购买主题包
void SendThemeRecommendInfo();
//插入循环消息命令到队列
void InsertLoopTextProcCmd(const list<string>& userList);
//插入循环高级表情命令到队列
void InsertLoopEmotionProcCmd(const list<string>& userList);
//#endif /* COMMONFUNCCMDHANDLE_H_ */
