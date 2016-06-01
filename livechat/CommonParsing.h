/*
 * author: Samson.Fan
 *   date: 2015-06-16
 *   file: CommonParsing.h
 *   desc: LiveChatClient公共解析协议
 */

#pragma once

#include "ILiveChatClientDef.h"
#include <amf/AmfParser.h>
#include <string>

// 解析用户信息
bool ParsingUserInfoItem(amf_object_handle handle, UserInfoItem& item);

// 解析主题包
bool ParsingThemeInfoItem(amf_object_handle handle, ThemeInfoItem& item);
