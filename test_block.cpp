/*
	作者：Piovano
	日期：2014.05.26
	描述：阻塞测试
*/

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

/*
	名称：联合队列阻塞/非阻塞示例
	备注：由于联合队列没有长度上限，故只对出队进行阻塞/非阻塞区分。
*/

sem_t _semPrint;
pthread_t tPush, tPopBlock, tPopNonBlock;

void *threadPush(void *arg) {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	UnionQueue<void> *queue = (UnionQueue<void> *)arg;
	void *pointer = NULL;
	off_t size;
	int i, j;
	for (i = 0; i < 10; i = i + 1) {
		size = rand() % 10;
		pointer = malloc(sizeof(int) * size);
		for (j = 0; j < size; j = j + 1) {
			*(((int *)pointer) + j) = j;
		}
		queue->push(pointer, (off_t) sizeof(int) * size);
		sem_wait(&_semPrint);
		cout<<"Push:\t["<<(intptr_t)pointer<<", "<<sizeof(int) * size<<"]"<<endl;
		sem_post(&_semPrint);
		usleep(1000000);
		pthread_testcancel();
	}
	usleep(25000000);
	for (i = 0; i < 10; i = i + 1) {
		size = rand() % 10;
		pointer = malloc(sizeof(int) * size);
		for (j = 0; j < size; j = j + 1) {
			*(((int *)pointer) + j) = j;
		}
		queue->push(pointer, (off_t) sizeof(int) * size);
		sem_wait(&_semPrint);
		cout<<"Push:\t["<<(intptr_t)pointer<<", "<<sizeof(int) * size<<"]"<<endl;
		sem_post(&_semPrint);
		usleep(1000000);
		pthread_testcancel();
	}
}

void *threadPopBlock(void *arg) {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	UnionQueue<void> *queue = (UnionQueue<void> *)arg;
	void *pointer;
	off_t size;
	for (int i = 0; i < 10; i = i + 1) {
		queue->pop(pointer, size);
		sem_wait(&_semPrint);
		cout<<"Pop(Block):\t["<<(intptr_t)(pointer)<<", "<<size<<"]"<<endl;
		sem_post(&_semPrint);
		usleep(2000000);
		pthread_testcancel();
	}
}

void *threadPopNonBlock(void *arg) {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	UnionQueue<void> *queue = (UnionQueue<void> *)arg;
	void *pointer;
	off_t size;
	int ret;
	for (int i = 0; i < 10; i = i + 1) {
		ret = queue->pop(pointer, size, false);
		sem_wait(&_semPrint);
		if (ret == 0) {
			cout<<"Pop(Non-Block) S:\t["<<(intptr_t)(pointer)<<", "<<size<<"]"<<endl;
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
	string path = "/home/piovano/queue2/file";
	UnionQueue<void> queue(2, path, 8);
	pthread_create(&tPush, NULL, threadPush, &queue);
	pthread_create(&tPopBlock, NULL, threadPopBlock, &queue);
	pthread_create(&tPopNonBlock, NULL, threadPopNonBlock, &queue);
	pthread_join(tPush, NULL);
	pthread_join(tPopBlock, NULL);
	pthread_join(tPopNonBlock, NULL);
	cout<<"Finished."<<endl;
}