/*
 * CommonFunc.h
 *
 *  Created on: 2015-3-12
 *      Author: Samson.Fan
 * Description: 存放公共函数
 */

#ifndef COMMONFUNCDEFINE_H_
#define COMMONFUNCDEFINE_H_

#include <string>
using namespace std;

// 获取数组元素个数
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))

// 判断文件是否存在
bool IsFileExist(const string& path);
// 判断目录是否存在
bool IsDirExist(const string& path);
// 新建目录
bool MakeDir(const string& path);
// 删除目录（包括目录中所有文件及文件夹）
bool RemoveDir(const string& path);
// 删除文件
bool RemoveFile(const string& path);
// 修改文件名
bool RenameFile(const string& srcPath, const string& desPath);
// 复制文件
bool CopyFile(const string& srcPath, const string& desPath);
// 清空目录（删除目录里所有文件及文件夹）
bool CleanDir(const string& path);

// 初始化random
bool InitRandom();
// 获取random数
int GetRandomValue();

// 获取当前时间（Unix Timestamp）
long GetCurrentTime();

#endif /* COMMONFUNCDEFINE_H_ */
