/*
 * ThemeConfig.h
 *
 *  Created on: 2016年4月19日
 *      Author: Hunter
 */

#ifndef THEMECONFIG_H_
#define THEMECONFIG_H_

#include <string>
#include <list>
using namespace std;

#include "../RequestOtherDefine.h"
#include "ThemeItem.h"
#include <xml/tinyxml.h>

class ThemeConfig{
public:
	class ThemeType{
	public:
		ThemeType(){
			typeId = "";
			typeName = "";
		}
		ThemeType(const ThemeType& type){
			typeId = type.typeId;
			typeName = type.typeName;
		}
		virtual ~ThemeType(){

		}
	public:
		bool parsing(const TiXmlNode* typeNode){
			bool result = false;
			if(NULL != typeNode){
				//typeId
				const TiXmlElement* typeIdNode = typeNode->FirstChildElement(LC_GET_THEME_CONFIG_THEME_TYPEID);
				if(NULL != typeIdNode){
					typeId = typeIdNode->GetText();
				}
				//typeName
				const TiXmlElement* typeNameNode = typeNode->FirstChildElement(LC_GET_THEME_CONFIG_THEME_TYPENAME);
				if(NULL != typeNameNode){
					typeName = typeNameNode->GetText();
				}

				if(!typeId.empty()){
					result = true;
				}
			}
			return result;
		}
	public:
		string typeId;
		string typeName;
	};
	typedef list<ThemeType> ThemeTypeList;

	class ThemeTag{
	public:
		ThemeTag(){
			tagId = "";
			typeId = "";
			tagName = "";
		}
		ThemeTag(const ThemeTag& tag){
			tagId = tag.tagId;
			typeId = tag.typeId;
			tagName = tag.tagName;
		}
		virtual ~ThemeTag(){

		}
	public:
		bool parsing(const TiXmlNode* tagNode){
			bool result = false;
			if(NULL != tagNode){
				//tagId
				const TiXmlElement* tagIdNode = tagNode->FirstChildElement(LC_GET_THEME_CONFIG_THEME_TAGSID);
				if(NULL != tagIdNode){
					tagId = tagIdNode->GetText();
				}
				//typeId
				const TiXmlElement* typeIdNode = tagNode->FirstChildElement(LC_GET_THEME_CONFIG_THEME_TYPEID);
				if(NULL != typeIdNode){
					typeId = typeIdNode->GetText();
				}
				//tagId
				const TiXmlElement* tagNameNode = tagNode->FirstChildElement(LC_GET_THEME_CONFIG_THEME_TAGSNAME);
				if(NULL != tagNameNode){
					tagName = tagNameNode->GetText();
				}
				if(!tagId.empty()){
					result = true;
				}
			}
			return result;
		}
	public:
		string tagId;
		string typeId;
		string tagName;
	};
	typedef list<ThemeTag> ThemeTagList;

public:
	ThemeConfig(){
		themeVersion = "";
		themePath = "";
	}

	ThemeConfig(const ThemeConfig& config){
		themeVersion = config.themeVersion;
		themePath = config.themePath;
		themeTypeList = config.themeTypeList;
		themeTagList = config.themeTagList;
		themeItemList = config.themeItemList;
	}

	virtual ~ThemeConfig(){}

public:
	bool parsing(const TiXmlNode* rootNode){
		bool result = false;
		if(NULL != rootNode){
			//themeVersion
			const TiXmlElement* themeVersionNode = rootNode->FirstChildElement(LC_GET_THEME_CONFIG_THEME_VERSION);
			if(NULL != themeVersionNode){
				themeVersion = themeVersionNode->GetText();
			}

			//themePath
			const TiXmlElement* themePathNode = rootNode->FirstChildElement(LC_GET_THEME_CONFIG_THEME_PATH);
			if(NULL != themePathNode){
				themePath = themePathNode->GetText();
			}
			//theme typeList
			const TiXmlNode* typeListNode = rootNode->FirstChild(LC_GET_THEME_CONFIG_THEME_TYPELIST);
			if(NULL != typeListNode){
				const TiXmlNode* typeNode = typeListNode->FirstChild(LC_GET_THEME_CONFIG_THEME_LIST);
				while(NULL != typeNode){
					ThemeType themeType;
					if(themeType.parsing(typeNode)){
						themeTypeList.push_back(themeType);
					}
					typeNode = typeNode->NextSibling();
				}
			}

			//theme tagList
			const TiXmlNode* tagListNode = rootNode->FirstChild(LC_GET_THEME_CONFIG_THEME_TAGLIST);
			if(NULL != tagListNode){
				const TiXmlNode* tagNode = tagListNode->FirstChild(LC_GET_THEME_CONFIG_THEME_LIST);
				while(NULL != tagNode){
					ThemeTag themeTag;
					if(themeTag.parsing(tagNode)){
						themeTagList.push_back(themeTag);
					}
					tagNode = tagNode->NextSibling();
				}
			}

			//theme itemList
			const TiXmlNode* itemListNode = rootNode->FirstChild(LC_GET_THEME_CONFIG_THEME_FORMAN);
			if(NULL != itemListNode){
				const TiXmlNode* itemNode = itemListNode->FirstChild(LC_GET_THEME_CONFIG_THEME_LIST);
				while(NULL != itemNode){
					ThemeItem themeItem;
					if(themeItem.parsing(itemNode)){
						themeItemList.push_back(themeItem);
					}
					itemNode = itemNode->NextSibling();
				}
			}

			if(!themePath.empty()){
				result = true;
			}
		}
		return result;
	}

public:
	string themeVersion;
	string themePath;
	ThemeTypeList themeTypeList;
	ThemeTagList themeTagList;
	ThemeItemList themeItemList;
};

#endif /* THEMECONFIG_H_ */
