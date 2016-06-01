/*
 * MagicIconConfig.h
 *
 *  Created on: 2016年4月7日
 *      Author: HOUNOR DE ELITES
 */

#ifndef MAGICICONCONFIG_H_
#define MAGICICONCONFIG_H_

#include <string>
#include <list>
using namespace std;

#include "../RequestOtherDefine.h"
#include <xml/tinyxml.h>

class MagicIconConfig {
public:
	class MagicIconType{
	public:
		MagicIconType(){
			typeId = "";
			typeTitle = "";
		}
		MagicIconType(const MagicIconType& type){
			typeId = type.typeId;
			typeTitle = type.typeTitle;
		}
		virtual ~MagicIconType(){}
	public:
		bool parsing(const TiXmlNode* typeNode){
			bool result = false;
			if(NULL != typeNode){
				const TiXmlElement* idNode = typeNode->FirstChildElement(LC_GET_MAGICICON_CONFIG_ID);
				if(NULL != idNode){
					typeId = idNode->GetText();
				}

				const TiXmlElement* titleNode = typeNode->FirstChildElement(LC_GET_MAGICICON_CONFIG_TITLE);
				if(NULL != titleNode){
					typeTitle = titleNode->GetText();
				}
				if(!typeId.empty()){
					result = true;
				}
			}
			return result;
		}
	public:
		string typeId;
		string typeTitle;
	};
	typedef list<MagicIconType> MagicTypeList;

	class MagicIconItem{
	public:
		MagicIconItem(){
			iconId = "";
			iconTitle = "";
			price = 0.0;
			hotflag = 0;
			typeId = "";
			updatetime = 0;
		}
		MagicIconItem(const MagicIconItem& item){
			iconId = item.iconId;
			iconTitle = item.iconTitle;
			price = item.price;
			hotflag = item.hotflag;
			typeId = item.typeId;
			updatetime = item.updatetime;
		}
		virtual ~MagicIconItem(){}
	public:
		bool parsing(const TiXmlNode* itemNode){
			bool result = false;
			if(NULL != itemNode){
				const TiXmlElement* idNode = itemNode->FirstChildElement(LC_GET_MAGICICON_CONFIG_ID);
				if(NULL != idNode){
					iconId = idNode->GetText();
				}

				const TiXmlElement* titleNode = itemNode->FirstChildElement(LC_GET_MAGICICON_CONFIG_TITLE);
				if(NULL != titleNode){
					iconTitle = titleNode->GetText();
				}
				const TiXmlElement* priceNode = itemNode->FirstChildElement(LC_GET_MAGICICON_CONFIG_PRICE);
				if(NULL != priceNode){
					const char* pPrice = priceNode->GetText();
					if(NULL != pPrice){
						price = atof(pPrice);
					}
				}

				const TiXmlElement* flagNode = itemNode->FirstChildElement(LC_GET_MAGICICON_CONFIG_HOTFLAG);
				if(NULL != flagNode){
					const char* flag = flagNode->GetText();
					if(NULL != flag){
						hotflag = atoi(flag);
					}
				}
				const TiXmlElement* typeIdNode = itemNode->FirstChildElement(LC_GET_MAGICICON_CONFIG_TYPEID);
				if(NULL != typeIdNode){
					typeId = typeIdNode->GetText();
				}

				const TiXmlElement* updatetimeNode = itemNode->FirstChildElement(LC_GET_MAGICICON_CONFIG_UPDATETIME);
				if(NULL != updatetimeNode){
					const char* pUpdatetime = updatetimeNode->GetText();
					if(NULL != pUpdatetime){
						updatetime = atoi(pUpdatetime);
					}
				}
				if(!iconId.empty()){
					result = true;
				}
			}
			return result;
		}
	public:
		string iconId;
		string iconTitle;
		double price;
		int hotflag;
		string typeId;
		int updatetime;
	};
	typedef list<MagicIconItem> MagicIconList;
public:
	MagicIconConfig(){
		path = "";
		maxupdatetime = 0;
	}
	MagicIconConfig(const MagicIconConfig& config){
		path = config.path;
		maxupdatetime = config.maxupdatetime;
		typeList = config.typeList;
		magicIconList = config.magicIconList;
	}
	virtual ~MagicIconConfig(){

	}
public:
	bool parsing(const TiXmlNode *rootNode){
		bool result = false;
		if(NULL != rootNode){
			const TiXmlElement* pathNodeElement = rootNode->FirstChildElement(LC_GET_MAGICICON_CONFIG_BASE_PATH);
			if(NULL != pathNodeElement){
				path = pathNodeElement->GetText();
			}
			const TiXmlElement* maxUpdatetimeNodeElement = rootNode->FirstChildElement(LC_GET_MAGICICON_CONFIG_MAXUPDATETIME);
			if(NULL != maxUpdatetimeNodeElement){
				const char* pUpdatetime = maxUpdatetimeNodeElement->GetText();
				if(NULL != pUpdatetime){
					maxupdatetime = atoi(pUpdatetime);
				}
			}

			const TiXmlNode* dataNode = rootNode->FirstChild(LC_GET_MAGICICON_CONFIG_DATA);
			if(NULL != dataNode){
				const TiXmlNode* infoNode = dataNode->FirstChild(LIVECHAT_INFO);
				if(NULL != infoNode){
					const TiXmlNode* magicItemNode = infoNode->FirstChild(LC_GET_MAGICICON_CONFIG_LIST);
					while(magicItemNode != NULL){
						MagicIconItem magicItem;
						if(magicItem.parsing(magicItemNode)){
							magicIconList.push_back(magicItem);
						}
						magicItemNode = magicItemNode->NextSibling();
					}
				}

				const TiXmlNode* typeListNode = dataNode->FirstChild(LC_GET_MAGICICON_CONFIG_TYPELIST);
				if(NULL != typeListNode){
					const TiXmlNode* magicTypeNode = typeListNode->FirstChild(LC_GET_MAGICICON_CONFIG_LIST);
					while(magicTypeNode != NULL){
						MagicIconType magicType;
						if(magicType.parsing(magicTypeNode)){
							typeList.push_back(magicType);
						}
						magicTypeNode = magicTypeNode->NextSibling();
					}
				}
			}

			if(!path.empty()){
				result = true;
			}
		}
		return result;
	}
public:
	string path;
	int maxupdatetime;
	MagicTypeList typeList;
	MagicIconList magicIconList;
};

#endif /* MAGICICONCONFIG_H_ */
