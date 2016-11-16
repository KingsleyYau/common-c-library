/*
 * author: Alex shum
 *   date: 2016-09-08
 *   file: LCMagicIconItem.h
 *   desc: LiveChat小高级表情消息
 */

#pragma once

#include <string>
using namespace std;

class LCMagicIconItem
{
public:
    LCMagicIconItem();
    virtual ~LCMagicIconItem();
    
public:
    //初始化
    bool Init(const string& magicIconId
              , const string& thumPath
              , const string& sourcePath);
    
public:
    string  m_magicIconId;      // 小高级表情ID
    string  m_thumbPath;         // 拇子图本地缓存地址
    string  m_sourcePath;       // 原图本地缓存地址
};