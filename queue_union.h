/*
	作者：Piovano
	日期：2014.05.26
	版本：1.0
	描述：联合队列
*/

#ifndef _QUEUE_UNION_H
#define _QUEUE_UNION_H

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
#include "queue_memory.h"
#include "queue_multi_file.h"

/*
	类名：联合队列
	功能：入队、出队、获取队列长度、获取内存队列长度限制、获取文件路径、获取单文件大小限制
*/
template<class T>
class UnionQueue : public AbstractQueue<T> {

	private:
	MemoryQueue<T> *_pushQueue, *_popQueue;
	MultiFileQueue<T> *_fileQueue;
	sem_t _semEdit;
	pthread_t _threadPush, _threadPop;
	static void *_pushTransfer(void *arg);
	static void *_popTransfer(void *arg);
	int doPush(T *pointer, bool block);
	int doPop(T *&pointer, bool block);

	public:
	UnionQueue(off_t dataSize, unsigned long memoryQueueLength, std::string filePath, off_t maxSingleFileSize = 1073741824);
	~UnionQueue();
	unsigned long maxMemoryQueueLength();
	std::string filePath();
	off_t maxSingleFileSize();

};

/*
	类名：联合队列
	功能：入队、出队、获取队列长度、获取内存队列长度限制、获取文件路径、获取单文件大小限制
*/
template<>
class UnionQueue<void> : public AbstractQueue<void> {

	private:
	MemoryQueue<void> *_pushQueue, *_popQueue;
	MultiFileQueue<void> *_fileQueue;
	sem_t _semEdit;
	pthread_t _threadPush, _threadPop;
	static void *_pushTransfer(void *arg);
	static void *_popTransfer(void *arg);
	int doPush(void *pointer, off_t size, bool block);
	int doPop(void *&pointer, off_t &size, bool block);

	public:
	UnionQueue(unsigned long memoryQueueLength, std::string filePath, off_t maxSingleFileSize = 1073741824);
	~UnionQueue();
	unsigned long maxMemoryQueueLength();
	std::string filePath();
	off_t maxSingleFileSize();

};

#ifndef _QUEUE_UNION_C

#include "queue_union.cpp"

#endif

#endif