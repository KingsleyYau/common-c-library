/*
*author:Alex shum
*  date:2016-8-15
*  file:CommonFuncCmdHandle.h
*  desc:������๲ͬ�õ���һЩ�����������Ǵ�ӡ�����ʽ
*/
//#ifndef COMMONFUNCCMDHANDLE_H_
//#define COMMONFUNCCMDHANDLE_H_
#pragma once
#include <list>
#include "CmdHandleDef.h"
#include <common/CommonFunc.h>
using namespace std;

//��ӡ�˳�����
void ExitInfo();
//��ӡ��ʼ������
void InitInfo();
//��ӡ�ϴ��汾������
void UploadVerInfo();
//��ӡ��¼����
void LoginInfo();
//��ӡ�ǳ�����
void LogoutInfo();
//��ӡ������Ϣ����
void SendTextInfo();
//��ӡѭ��������Ϣ����
void SendLoopTextInfo();
//��ӡ�߼���������
void SendEmotionInfo();
//��ӡѭ����������
void SendLoopEmotionInfo();
//��ӡС��������
void SendMagicIconInfo();
//��ӡ��ȡ�û���Ϣ����
void GetUserInfo();
//��ӡ�ϴ�����汾����
void UploadThemeVerInfo();
//��ӡ�û����������
void GetUserThemeInfo();
//��ӡ��ȡ�������������
void GetAllThemeInfo();
//��ӡ�������������
void BuyThemeInfo();
//��ӡӦ�����������
void ApplyThemeInfo();
//��ӡ�����������������
void PlayThemeInfo();
//��ӡTalkList����
void GetTalkListInfo();
//��ӡ�Զ�����״̬
void GetAutoInviteStatusInfo();
//��ӡ�Զ�������Ϣ
void SendAutoInviteInfo();
//��ӡŮʿ�Ƽ���ʿ���������
void SendThemeRecommendInfo();
//����ѭ����Ϣ�������
void InsertLoopTextProcCmd(const list<string>& userList);
//����ѭ���߼������������
void InsertLoopEmotionProcCmd(const list<string>& userList);
//#endif /* COMMONFUNCCMDHANDLE_H_ */
