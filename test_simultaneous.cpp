/*
	作者：Piovano
	日期：2014.05.26
	描述：并发测试
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
#include "queue_multi_file.h"

using namespace std;

/*
	名称：内存队列阻塞/非阻塞示例
	备注：由于内存队列不对内存区块进行操作，故示例所有用到的指针均为 NULL 指针。
*/

pthread_t tPush[50], tPop[50];

/*
	名称：入队线程
	描述：所有元素均可入队
*/
void *threadPush(void *arg) {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	MultiFileQueue<int> *queue = (MultiFileQueue<int> *)arg;
	int *pointer = NULL;
	for (int i = 0; i < 20; i = i + 1) {
		pointer = new int(i);
		*pointer = i;
		cout<<"PUSH\t"<<*pointer<<"\t";
		queue->push(pointer);
		pthread_testcancel();
	}
}

/*
	名称：出队线程
	描述：以阻塞方式出队
*/
void *threadPop(void *arg) {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	MultiFileQueue<int> *queue = (MultiFileQueue<int> *)arg;
	int *pointer;
	off_t size;
	for (int i = 0; i < 20; i = i + 1) {
		queue->pop(pointer);
		cout<<"POP\t"<<*pointer<<"\t";
		pthread_testcancel();
	}
}

/*
	名称：主函数
*/
int main() {
	cout<<"Started."<<endl;
	string path = "./file";
	MultiFileQueue<int> queue(sizeof(int), path, 1280);
	int i;
	for (i = 0; i < 50; i = i + 1) {
		pthread_create(&tPush[i], NULL, threadPush, &queue);
	}
	for (i = 0; i < 50; i = i + 1) {
		pthread_create(&tPop[i], NULL, threadPop, &queue);
	}
	for (i = 0; i < 50; i = i + 1) {
		pthread_join(tPush[i], NULL);
	}
	for (i = 0; i < 50; i = i + 1) {
		pthread_join(tPop[i], NULL);
	}
	cout<<"Finished."<<endl;
}