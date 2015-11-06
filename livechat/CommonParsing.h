/*
 * author: Samson.Fan
 *   date: 2015-06-16
 *   file: CommonParsing.h
 *   desc: LiveChatClient公共解析协议
 */

#pragma once

#include "ILiveChatClientDef.h"
#include <AmfParser.h>
#include <string>

bool ParsingUserInfoItem(amf_object_handle handle, UserInfoItem& item);
