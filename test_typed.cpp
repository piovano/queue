/*
	作者：Piovano
	日期：2014.05.26
	描述：有类型数据测试
*/

#define DEBUG
#define DEBUG_UNION

#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>

#include "queue_common.h"
#include "queue_union.h"

using namespace std;

sem_t _semPrint;
pthread_t tPush, tPopBlock, tPopNonBlock;

void *threadPush(void *arg) {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	UnionQueue<int> *queue = (UnionQueue<int> *)arg;
	int *pointer = NULL;
	int i, j;
	for (i = 0; i < 10; i = i + 1) {
		pointer = new int(i);
		*pointer = i;
		queue->push(pointer);
		sem_wait(&_semPrint);
		cout<<"Push:\t["<<(intptr_t)pointer<<", "<<*pointer<<"]"<<endl;
		sem_post(&_semPrint);
		usleep(1000000);
		pthread_testcancel();
	}
	usleep(25000000);
	for (i = 0; i < 10; i = i + 1) {
		pointer = new int(i);
		*pointer = i;
		queue->push(pointer);
		sem_wait(&_semPrint);
		cout<<"Push:\t["<<(intptr_t)pointer<<", "<<*pointer<<"]"<<endl;
		sem_post(&_semPrint);
		usleep(1000000);
		pthread_testcancel();
	}
}

void *threadPopBlock(void *arg) {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	UnionQueue<int> *queue = (UnionQueue<int> *)arg;
	int *pointer;
	for (int i = 0; i < 10; i = i + 1) {
		queue->pop(pointer);
		sem_wait(&_semPrint);
		cout<<"Pop(Block):\t["<<(intptr_t)(pointer)<<", "<<*pointer<<"]"<<endl;
		sem_post(&_semPrint);
		usleep(2000000);
		pthread_testcancel();
	}
}

void *threadPopNonBlock(void *arg) {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	UnionQueue<int> *queue = (UnionQueue<int> *)arg;
	int *pointer;
	int ret;
	for (int i = 0; i < 10; i = i + 1) {
		ret = queue->pop(pointer, false);
		sem_wait(&_semPrint);
		if (ret == 0) {
			cout<<"Pop(Non-Block) S:\t["<<(intptr_t)(pointer)<<", "<<*pointer<<"]"<<endl;
		} else {
			cout<<"Pop(Non-Block) F"<<endl;
		}
		sem_post(&_semPrint);
		usleep(2000000);
		pthread_testcancel();
	}
}

int main() {
	cout<<"Started."<<endl;
	sem_init(&_semPrint, 0, 1);
	string path = "./file";
	UnionQueue<int> queue(sizeof(int), 2, path, 64);
	pthread_create(&tPush, NULL, threadPush, &queue);
	pthread_create(&tPopBlock, NULL, threadPopBlock, &queue);
	pthread_create(&tPopNonBlock, NULL, threadPopNonBlock, &queue);
	pthread_join(tPush, NULL);
	pthread_join(tPopBlock, NULL);
	pthread_join(tPopNonBlock, NULL);
	cout<<"Finished."<<endl;
}