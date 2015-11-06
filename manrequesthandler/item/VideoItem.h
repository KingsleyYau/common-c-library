/*
 * VideoItem.h
 *
 *  Created on: 2015-3-2
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef VIDEOITEM_H_
#define VIDEOITEM_H_

#include <string>
using namespace std;


#include <json/json/json.h>

#include "../RequestLadyDefine.h"

class VideoItem {
public:
	void Parse(Json::Value root) {
		if( root.isObject() ) {
			if( root[LADY_VIDEO_ID].isString() ) {
				id = root[LADY_VIDEO_ID].asString();
			}

			if( root[LADY_VIDEO_THUMB].isString() ) {
				thumb = root[LADY_VIDEO_THUMB].asString();
			}

			if( root[LADY_VIDEO_TIME].isString() ) {
				time = root[LADY_VIDEO_TIME].asString();
			}

			if( root[LADY_VIDEO_PHOTO].isString() ) {
				photo = root[LADY_VIDEO_PHOTO].asString();
			}
		}
	}

	VideoItem() {		id = "";
		thumb = "";
		time = "";
		photo = "";
	}
	virtual ~VideoItem() {

	}

	string id;
	string thumb;
	string time;
	string photo;

};

#endif /* VIDEOITEM_H_ */
