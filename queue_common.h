/*
	作者：Piovano
	日期：2014.05.26
	版本：1.0
	描述：队列共享库
*/

#ifndef _QUEUE_COMMON
#define _QUEUE_COMMON

#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

/*
	类名：内存区块结构
	成员：区块指针，区块大小
*/
struct MemoryBlock {
	void *pointer;
	off_t size;
};

/*
	常量：错误码
*/
const int ERROR_QUEUE_IS_FULL			= 1;				//队列满，无法入队
const int ERROR_QUEUE_IS_EMPTY			= 2;				//队列空，无法出队
const int ERROR_DATA_SIZE_IS_TOO_LARGE	= 3;				//试图投入的数据过大，无法放入文件中

/*
	常量：调试
*/
#ifdef DEBUG
pthread_spinlock_t spinDebug;

void debugInitialize() __attribute__((constructor));

void debugInitialize() {
	std::cout<<"Debug Mode"<<std::endl;
	pthread_spin_init(&spinDebug, PTHREAD_PROCESS_PRIVATE);
}
#endif

#endif