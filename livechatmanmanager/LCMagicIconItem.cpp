/*
 *  author: Alex shum
 *    date: 2016-09-08
 *    file:LCMagicIconItem.cpp
 *    desc:LiveChat小高级表情消息item
 */

#include "LCMagicIconItem.h"
#include <common/CommonFunc.h>
#include <common/CheckMemoryLeak.h>

LCMagicIconItem::LCMagicIconItem()
{
    m_magicIconId = "";
    m_thumbPath    = "";
    m_sourcePath  = "";
}

LCMagicIconItem::~LCMagicIconItem()
{

}


//初始化
bool LCMagicIconItem::Init(const string& magicIconId
                           , const string& thumPath
                           , const string& sourcePath)
{
    bool result = false;
    if( !magicIconId.empty())
    {
        m_magicIconId = magicIconId;
        
        if ( !thumPath.empty()) {
            result = true;
            if ( IsFileExist(thumPath)){
                m_thumbPath = thumPath;
            }
            
            if ( IsFileExist(sourcePath)) {
                m_sourcePath = sourcePath;
            }
        }
        
    }
    return result;
}