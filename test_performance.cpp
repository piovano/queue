/*
	作者：Piovano
	日期：2014.05.26
	描述：并发测试
*/

#define DEBUG
#define DEBUG_MULTI_FILE

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
#include "queue_union.h"

using namespace std;

struct GData {
	int head[192];
	float foot[3000];
};

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
	UnionQueue<GData> *queue = (UnionQueue<GData> *)arg;
	GData *pointer = NULL;
	for (int i = 0; i < 5000000; i = i + 1) {
		pointer = new GData;
		pointer->head[0] = i;
		//if (i % 1000000 == 0) {
		//	cout<<"PUSH\t"<<pointer->head[0]<<endl;
		//}
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
	UnionQueue<GData> *queue = (UnionQueue<GData> *)arg;
	GData *pointer;
	//off_t size;
	for (int i = 0; i < 10000000; i = i + 1) {
		queue->pop(pointer);
		//if (i % 1000000 == 0) {
		//	cout<<"POP\t"<<pointer->head[0]<<endl;
		//}
		delete pointer;
		pthread_testcancel();
	}
}

/*
	名称：主函数
*/
int main() {
	struct timespec timeStart;
	clock_gettime(CLOCK_REALTIME, &timeStart); 
	cout<<"Started."<<endl;
	string path = "./file";
	UnionQueue<GData> queue(sizeof(GData), 1024, path, 1024*1024*1024);
	pthread_create(&tPush[0], NULL, threadPush, &queue);
	pthread_create(&tPush[1], NULL, threadPush, &queue);
	pthread_create(&tPop[0], NULL, threadPop, &queue);
	//pthread_create(&tPop[1], NULL, threadPop, &queue);
	pthread_join(tPush[0], NULL);
	pthread_join(tPush[1], NULL);
	pthread_join(tPop[0], NULL);
	//pthread_join(tPop[1], NULL);
	struct timespec timeEnd;
	clock_gettime(CLOCK_REALTIME, &timeEnd); 
	cout<<"Finished."<<endl;
	long sec = 0;
	long msec = 0;
	long usec = 0;
	long nsec = 0;
	if (timeStart.tv_nsec < timeEnd.tv_nsec) {
		sec = timeEnd.tv_sec - timeStart.tv_sec;
		nsec = timeEnd.tv_nsec - timeStart.tv_nsec;
	} else {
		sec = timeEnd.tv_sec - timeStart.tv_sec - 1;
		nsec = timeEnd.tv_nsec + 1000000000 - timeStart.tv_nsec;
	}
	msec = nsec / 1000000;
	nsec = nsec % 1000000;
	usec = nsec / 1000;
	nsec = nsec % 1000;
	cout<<"用时："<<sec<<"秒"<<msec<<"毫秒"<<usec<<"微妙"<<nsec<<"纳秒"<<endl;
}