/*
 * KSafeMap.h
 *
 *  Created on: 2015-3-2
 *      Author: Max.chiu
 */

#ifndef KSafeMap_H_
#define KSafeMap_H_

#include "KMutex.h"

#include<map>
using namespace std;

template<typename Key, typename Value>
class KSafeMap {
	typedef map<Key, Value> SafeMap;

public:
	typedef typename SafeMap::iterator iterator;

	KSafeMap() {

	}
	virtual ~KSafeMap() {

	}

	void Insert(Key key, Value value) {
		mKMutex.lock();
		mMap.insert( typename SafeMap::value_type(key, value) );
		mKMutex.unlock();
	}
	Value Erase(Key key) {
		Value value = 0;
		mKMutex.lock();
		typename SafeMap::iterator itr = mMap.find(key);
		if( itr != mMap.end() ) {
			mMap.erase(itr);
			value = itr->second;
		}
		mKMutex.unlock();
		return value;
	}
	void Empty() {
		mKMutex.lock();
		mMap.empty();
		mKMutex.unlock();
	}

//	Value Find(Key key) {
//		Value value = 0;
//		typename SafeMap::iterator itr = mMap.find(key);
//		if( itr != mMap.end() ) {
//			value = itr->second;
//		}
//		return value;
//	}

	iterator Find(Key key) {
		typename SafeMap::iterator itr = mMap.find(key);
		return itr;
	}

	iterator Begin() {
		return mMap.begin();
	}

	iterator End() {
		return mMap.end();
	}

	/**
	 * 经删节点, 调用前需要先记录
	 * Example:
	 * 	KSafeMap<string> map;
	 * 	RequestkMap::iterator itr = map.Find(identify);
	 *	map.Erase(itr++);
	 *
	 */
	void Erase(iterator itr) {
		if( itr != mMap.end() ) {
			mMap.erase(itr);
		}
	}
	void Lock() {
		mKMutex.lock();
	}
	void Unlock() {
		mKMutex.unlock();
	}

private:
	KMutex mKMutex;
	SafeMap mMap;
};
#endif /* KSafeMap_H_ */
