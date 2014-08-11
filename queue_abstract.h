/*
	作者：Piovano
	日期：2014.05.26
	版本：1.0
	描述：抽象队列
*/

#ifndef _QUEUE_ABSTRACT_H
#define _QUEUE_ABSTRACT_H

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

/*
	类名：抽象队列（有类型）
	功能：入队、出队、获取队列长度
*/
template<class T>
class AbstractQueue {

	protected:
	off_t _dataSize;
	sem_t _semPop;
	virtual int doPush(T *pointer, bool block) = 0;
	virtual int doPop(T *&pointer, bool block) = 0;
	void create(T *&pointer);
	void destroy(T *pointer);

	public:
	AbstractQueue(off_t dataSize);
	~AbstractQueue();
	int push(T *pointer, bool block = true);
	int pop(T *&pointer, bool block = true);
	unsigned long length();
	off_t dataSize();

};

/*
	类名：抽象队列（无类型）
	功能：入队、出队、获取队列长度
*/
template<>
class AbstractQueue<void> {

	protected:
	sem_t _semPop;
	virtual int doPush(void *pointer, off_t size, bool block) = 0;
	virtual int doPop(void *&pointer, off_t &size, bool block) = 0;
	void create(void *&pointer, size_t size);
	void destroy(void *pointer);

	public:
	AbstractQueue();
	~AbstractQueue();
	int push(void *pointer, off_t size, bool block = true);
	int pop(void *&pointer, off_t &size, bool block = true);
	unsigned long length();

};

#ifndef _QUEUE_ABSTRACT_C

#include "queue_abstract.cpp"

#endif

#endif