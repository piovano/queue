/*
	作者：Piovano
	日期：2014.05.26
	版本：1.0
	描述：抽象队列
*/

#ifndef _QUEUE_ABSTRACT_C
#define _QUEUE_ABSTRACT_C

#ifndef _QUEUE_ABSTRACT_H

#include "queue_abstract.h"

/*
	函数：构造
	参数：无
*/
AbstractQueue<void>::AbstractQueue() {
	sem_init(&_semPop, 0, 0);
}

/*
	函数：析构
*/
AbstractQueue<void>::~AbstractQueue() {
	sem_destroy(&_semPop);
}

/*
	函数：分配内存
	参数：指针，大小
	返回：无
*/
void AbstractQueue<void>::create(void *&pointer, size_t size) {
	pointer = malloc(size);
}

/*
	函数：销毁内存
	参数：指针
	返回：无
*/
void AbstractQueue<void>::destroy(void *pointer) {
	free(pointer);
}

/*
	函数：入队
	参数：指针、大小、阻塞
	返回：错误码
*/
int AbstractQueue<void>::push(void *pointer, off_t size, bool block) {
	doPush(pointer, size, block);
}

/*
	函数：出队
	参数：指针、大小、阻塞
	返回：错误码
*/
int AbstractQueue<void>::pop(void *&pointer, off_t &size, bool block) {
	doPop(pointer, size, block);
}

/*
	函数：队列长度
	参数：无
	返回：队列长度
*/
unsigned long AbstractQueue<void>::length() {
	int length;
	sem_getvalue(&_semPop, &length);
	return length;
}

#endif

/*
	函数：构造
	参数：数据大小
*/
template<class T>
AbstractQueue<T>::AbstractQueue(off_t dataSize) {
	_dataSize = dataSize;
	sem_init(&_semPop, 0, 0);
}

/*
	函数：析构
	参数：无
*/
template<class T>
AbstractQueue<T>::~AbstractQueue() {
	sem_destroy(&_semPop);
}

/*
	函数：分配内存
	参数：指针
	返回：无
*/
template<class T>
void AbstractQueue<T>::create(T *&pointer) {
	pointer = new T;
}

/*
	函数：销毁内存
	参数：指针
	返回：无
*/
template<class T>
void AbstractQueue<T>::destroy(T *pointer) {
	delete pointer;
}

/*
	函数：入队
	参数：指针、阻塞
	返回：错误码
*/
template<class T>
int AbstractQueue<T>::push(T *pointer, bool block) {
	return doPush(pointer, block);
}

/*
	函数：出队
	参数：指针、阻塞
	返回：错误码
*/
template<class T>
int AbstractQueue<T>::pop(T *&pointer, bool block) {
	return doPop(pointer, block);
}

/*
	函数：队列长度
	参数：无
	返回：队列长度
*/
template<class T>
unsigned long AbstractQueue<T>::length() {
	int length;
	sem_getvalue(&_semPop, &length);
	return length;
}

/*
	函数：数据大小
	参数：无
	返回：数据大小
*/
template<class T>
off_t AbstractQueue<T>::dataSize() {
	return _dataSize;
}

#endif