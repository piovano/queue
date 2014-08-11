/*
	作者：Piovano
	日期：2014.05.26
	版本：1.0
	描述：单文件队列
*/

#ifndef _QUEUE_FILE_H
#define _QUEUE_FILE_H

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
	类名：单文件队列
	功能：入队、出队、获取队列长度、打开文件、关闭文件、获取路径、获取文件大小限制、输出队列内容（非线程安全）
*/
template<class T>
class FileQueue : public AbstractQueue<T> {

	private:
	std::string _path;
	int _file;
	off_t _maxSize, _size;
	off_t _head, _tail;
	sem_t _semPush;
	pthread_mutex_t _mutexReadWrite;
	int doPush(T *pointer, bool block);
	int doPop(T *&pointer, bool block);

	public:
	FileQueue(off_t dataSize, std::string path, off_t maxSize);
	~FileQueue();
	int openFile();
	int closeFile();
	std::string path();
	off_t maxSize();
	void print();

};

/*
	类名：单文件队列
	功能：入队、出队、获取队列长度、打开文件、关闭文件、获取路径、获取文件大小限制、输出队列内容（非线程安全）
*/
template<>
class FileQueue<void> : public AbstractQueue<void> {

	private:
	std::string _path;
	int _file;
	off_t _maxSize, _size;
	off_t _head, _tail;
	sem_t _semPush;
	pthread_mutex_t _mutexReadWrite;
	int doPush(void *pointer, off_t size, bool block);
	int doPop(void *&pointer, off_t &size, bool block);

	public:
	FileQueue(std::string path, off_t maxSize);
	~FileQueue();
	int openFile();
	int closeFile();
	std::string path();
	off_t maxSize();
	void print();

};

#ifndef _QUEUE_FILE_C

#include "queue_file.cpp"

#endif

#endif