/*
	作者：Piovano
	日期：2014.05.26
	版本：1.0
	描述：多文件队列
*/

#ifndef _QUEUE_MULTI_FILE_H
#define _QUEUE_MULTI_FILE_H

#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#include "queue_common.h"
#include "queue_abstract.h"
#include "queue_file.h"

/*
	类名：多文件队列
	功能：入队、出队、获取路径、获取单文件大小限制、获取队列长度
*/
template<class T>
class MultiFileQueue : public AbstractQueue<T> {

	private:
	std::string _path;
	off_t _maxSingleFileSize;
	FileQueue<T> **_fileQueues, **_head, **_tail;
	int _maxFileQueueCount, _fileId;
	void expandFileQueues();
	void newFileQueue();
	void deleteFileQueue();
	sem_t _semPush;
	pthread_mutex_t _mutexPushPop;
	int doPush(T *pointer, bool block);
	int doPop(T *&pointer, bool block);
	std::string intToString(int i);

	public:
	MultiFileQueue(off_t dataSize, std::string path, off_t maxSingleFileSize = 1073741824);
	~MultiFileQueue();
	std::string path();
	off_t maxSingleFileSize();

};

/*
	类名：多文件队列
	功能：入队、出队、获取路径、获取单文件大小限制、获取队列长度
*/
template<>
class MultiFileQueue<void> : public AbstractQueue<void> {

	private:
	std::string _path;
	off_t _maxSingleFileSize;
	FileQueue<void> **_fileQueues, **_head, **_tail;
	int _maxFileQueueCount, _fileId;
	void expandFileQueues();
	void newFileQueue();
	void deleteFileQueue();
	sem_t _semPush;
	pthread_mutex_t _mutexPushPop;
	int doPush(void *pointer, off_t size, bool block);
	int doPop(void *&pointer, off_t &size, bool block);
	std::string intToString(int i);

	public:
	MultiFileQueue(std::string path, off_t maxSingleFileSize = 1073741824);
	~MultiFileQueue();
	std::string path();
	off_t maxSingleFileSize();

};

#ifndef _QUEUE_MULTI_FILE_C

#include "queue_multi_file.cpp"

#endif

#endif