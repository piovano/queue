/*
	作者：Piovano
	日期：2014.05.26
	版本：1.0
	描述：内存队列
*/

#ifndef _QUEUE_MEMORY_H
#define _QUEUE_MEMORY_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#include "queue_common.h"
#include "queue_abstract.h"

/*
	类名：内存队列
	功能：入队、出队、获取队列长度、获取队列长度限制、输出队列内容（测试）
*/
template<class T>
class MemoryQueue : public AbstractQueue<T> {

	private:
	T **_queue;
	T **_head, **_tail;
	unsigned long _maxLength;
	sem_t _semPush;
	pthread_spinlock_t _spinPush, _spinPop;
	int doPush(T *pointer, bool block);
	int doPop(T *&pointer, bool block);

	public:
	MemoryQueue(off_t dataSize, unsigned long maxLength);
	~MemoryQueue();
	unsigned long maxLength();
	void print();

};

/*
	类名：内存队列
	功能：入队、出队、获取队列长度、获取队列长度限制、输出队列内容（测试）
*/
template<>
class MemoryQueue<void> : public AbstractQueue<void> {

	private:
	MemoryBlock *_queue;
	MemoryBlock *_head, *_tail;
	unsigned long _maxLength;
	sem_t _semPush;
	pthread_spinlock_t _spinPush, _spinPop;
	int doPush(void *pointer, off_t size, bool block);
	int doPop(void *&pointer, off_t &size, bool block);

	public:
	MemoryQueue(unsigned long maxLength);
	~MemoryQueue();
	unsigned long maxLength();
	void print();

};

#ifndef _QUEUE_MEMORY_C

#include "queue_memory.cpp"

#endif

#endif