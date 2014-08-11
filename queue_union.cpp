/*
	作者：Piovano
	日期：2014.05.26
	版本：1.0
	描述：联合队列
*/

#ifndef _QUEUE_UNION_C
#define _QUEUE_UNION_C

#ifndef _QUEUE_UNION_H

#include "queue_union.h"

/*
	函数：构造
	参数：内存队列的最大长度，文件路径前缀，单个文件大小上限
	备注：1、实际内存队列的长度为参数设定的两倍；2、实际生成的文件路径为“前缀+序号”形式
*/
UnionQueue<void>::UnionQueue(unsigned long memoryQueueLength, std::string filePath, off_t maxSingleFileSize) {
	_pushQueue = new MemoryQueue<void>(memoryQueueLength);
	_popQueue = new MemoryQueue<void>(memoryQueueLength);
	_fileQueue = new MultiFileQueue<void>(filePath, maxSingleFileSize);
	pthread_create(&_threadPush, NULL, _pushTransfer, this);
	pthread_create(&_threadPop, NULL, _popTransfer, this);
}

/*
	函数：析构
*/
UnionQueue<void>::~UnionQueue() {
	pthread_cancel(_threadPush);
	pthread_cancel(_threadPop);
	pthread_join(_threadPush, NULL);
	pthread_join(_threadPop, NULL);
	delete _pushQueue;
	delete _popQueue;
	delete _fileQueue;
}

/*
	函数：入队传递（从内存队列到文件队列）
	参数：联合队列对象指针（this）
	返回：无
*/
void *UnionQueue<void>::_pushTransfer(void *arg) {
	UnionQueue<void> *unionQueue = (UnionQueue<void> *)arg;
	void *pointer;
	off_t size;
	while (true) {
		unionQueue->_pushQueue->pop(pointer, size, true);
		if (unionQueue->_fileQueue->length() > 0) {
			unionQueue->_fileQueue->push(pointer, size);
			#ifdef DEBUG_UNION
			pthread_spin_lock(&spinDebug);
			std::cout<<"["<<clock()<<"] "<<"PushMemoryQueue -> MultiFileQueue"<<std::endl;
			pthread_spin_unlock(&spinDebug);
			#endif
		} else {
			if (unionQueue->_popQueue->push(pointer, size, false) != 0) {
				unionQueue->_fileQueue->push(pointer, size, true);
				#ifdef DEBUG_UNION
				pthread_spin_lock(&spinDebug);
				std::cout<<"["<<clock()<<"] "<<"PushMemoryQueue -> MultiFileQueue"<<std::endl;
				pthread_spin_unlock(&spinDebug);
				#endif
			} else {
				#ifdef DEBUG_UNION
				pthread_spin_lock(&spinDebug);
				std::cout<<"["<<clock()<<"] "<<"PushMemoryQueue -> PopMemoryQueue"<<std::endl;
				pthread_spin_unlock(&spinDebug);
				#endif
			}
		}
		pthread_testcancel();
	}
}

/*
	函数：出队传递（从文件队列到内存队列）
	参数：联合队列对象指针（this）
	返回：无
*/
void *UnionQueue<void>::_popTransfer(void *arg) {
	UnionQueue *unionQueue = (UnionQueue *)arg;
	void *pointer;
	off_t size;
	while (true) {
		unionQueue->_fileQueue->pop(pointer, size, true);
		unionQueue->_popQueue->push(pointer, size, true);
		#ifdef DEBUG_UNION
		pthread_spin_lock(&spinDebug);
		std::cout<<"["<<clock()<<"] "<<"MultiFileQueue -> PopMemoryQueue"<<std::endl;
		pthread_spin_unlock(&spinDebug);
		#endif
		pthread_testcancel();
	}
}

/*
	函数：入队
	参数：指针、大小、阻塞
	返回：错误码
*/
int UnionQueue<void>::doPush(void *pointer, off_t size, bool block) {
	_pushQueue->push(pointer, size, true);
	#ifdef DEBUG_UNION
	pthread_spin_lock(&spinDebug);
	std::cout<<"["<<clock()<<"] "<<"Data -> PushMemoryQueue"<<std::endl;
	pthread_spin_unlock(&spinDebug);
	#endif
	sem_post(&(this->_semPop));
	return 0;
}

/*
	函数：出队
	参数：指针、大小、阻塞
	返回：错误码
*/
int UnionQueue<void>::doPop(void *&pointer, off_t &size, bool block) {
	if (block) {
		sem_wait(&(this->_semPop));
	} else {
		if (sem_trywait(&(this->_semPop)) != 0) {
			return ERROR_QUEUE_IS_EMPTY;
		}
	}
	_popQueue->pop(pointer, size, true);
	#ifdef DEBUG_UNION
	pthread_spin_lock(&spinDebug);
	std::cout<<"["<<clock()<<"] "<<"PopMemoryQueue -> Data"<<std::endl;
	pthread_spin_unlock(&spinDebug);
	#endif
	return 0;
}

/*
	函数：获取内存队列长度限制
	参数：无
	返回：内存队列长度限制
*/
unsigned long UnionQueue<void>::maxMemoryQueueLength() {
	return _pushQueue->maxLength();
}

/*
	函数：获取文件路径
	参数：无
	返回：文件路径
*/
std::string UnionQueue<void>::filePath() {
	return _fileQueue->path();
}

/*
	函数：获取单文件大小限制
	参数：无
	返回：单文件大小限制
*/
off_t UnionQueue<void>::maxSingleFileSize() {
	return _fileQueue->maxSingleFileSize();
}

#endif

/*
	函数：构造
	参数：数据大小、内存队列的最大长度，文件路径前缀，单个文件大小上限
	备注：1、实际内存队列的长度为参数设定的两倍；2、实际生成的文件路径为“前缀+序号”形式
*/
template<class T>
UnionQueue<T>::UnionQueue(off_t dataSize, unsigned long memoryQueueLength, std::string filePath, off_t maxSingleFileSize) : AbstractQueue<T>(dataSize) {
	_pushQueue = new MemoryQueue<T>(dataSize, memoryQueueLength);
	_popQueue = new MemoryQueue<T>(dataSize, memoryQueueLength);
	_fileQueue = new MultiFileQueue<T>(dataSize, filePath, maxSingleFileSize);
	pthread_create(&_threadPush, NULL, _pushTransfer, this);
	pthread_create(&_threadPop, NULL, _popTransfer, this);
}

/*
	函数：析构
*/
template<class T>
UnionQueue<T>::~UnionQueue() {
	pthread_cancel(_threadPush);
	pthread_cancel(_threadPop);
	pthread_join(_threadPush, NULL);
	pthread_join(_threadPop, NULL);
	delete _pushQueue;
	delete _popQueue;
	delete _fileQueue;
}

/*
	函数：入队传递（从内存队列到文件队列）
	参数：联合队列对象指针（this）
	返回：无
*/
template<class T>
void *UnionQueue<T>::_pushTransfer(void *arg) {
	UnionQueue<T> *unionQueue = (UnionQueue<T> *)arg;
	T *pointer;
	while (true) {
		unionQueue->_pushQueue->pop(pointer, true);
		if (unionQueue->_fileQueue->length() > 0) {
			unionQueue->_fileQueue->push(pointer);
			#ifdef DEBUG_UNION
			pthread_spin_lock(&spinDebug);
			std::cout<<"["<<clock()<<"] "<<"PushMemoryQueue -> MultiFileQueue"<<std::endl;
			pthread_spin_unlock(&spinDebug);
			#endif
		} else {
			if (unionQueue->_popQueue->push(pointer, false) != 0) {
				unionQueue->_fileQueue->push(pointer, true);
				#ifdef DEBUG_UNION
				pthread_spin_lock(&spinDebug);
				std::cout<<"["<<clock()<<"] "<<"PushMemoryQueue -> MultiFileQueue"<<std::endl;
				pthread_spin_unlock(&spinDebug);
				#endif
			} else {
				#ifdef DEBUG_UNION
				pthread_spin_lock(&spinDebug);
				std::cout<<"["<<clock()<<"] "<<"PushMemoryQueue -> PopMemoryQueue"<<std::endl;
				pthread_spin_unlock(&spinDebug);
				#endif
			}
		}
		pthread_testcancel();
	}
}

/*
	函数：出队传递（从文件队列到内存队列）
	参数：联合队列对象指针（this）
	返回：无
*/
template<class T>
void *UnionQueue<T>::_popTransfer(void *arg) {
	UnionQueue *unionQueue = (UnionQueue *)arg;
	T *pointer;
	while (true) {
		unionQueue->_fileQueue->pop(pointer, true);
		unionQueue->_popQueue->push(pointer, true);
		#ifdef DEBUG_UNION
		pthread_spin_lock(&spinDebug);
		std::cout<<"["<<clock()<<"] "<<"MultiFileQueue -> PopMemoryQueue"<<std::endl;
		pthread_spin_unlock(&spinDebug);
		#endif
		pthread_testcancel();
	}
}

/*
	函数：入队
	参数：指针、阻塞
	返回：错误码
*/
template<class T>
int UnionQueue<T>::doPush(T *pointer, bool block) {
	_pushQueue->push(pointer, true);
	#ifdef DEBUG_UNION
	pthread_spin_lock(&spinDebug);
	std::cout<<"["<<clock()<<"] "<<"Data -> PushMemoryQueue"<<std::endl;
	pthread_spin_unlock(&spinDebug);
	#endif
	sem_post(&(this->_semPop));
	return 0;
}

/*
	函数：出队
	参数：指针、阻塞
	返回：错误码
*/
template<class T>
int UnionQueue<T>::doPop(T *&pointer, bool block) {
	if (block) {
		sem_wait(&(this->_semPop));
	} else {
		if (sem_trywait(&(this->_semPop)) != 0) {
			return ERROR_QUEUE_IS_EMPTY;
		}
	}
	_popQueue->pop(pointer, true);
	#ifdef DEBUG_UNION
	pthread_spin_lock(&spinDebug);
	std::cout<<"["<<clock()<<"] "<<"PopMemoryQueue -> Data"<<std::endl;
	pthread_spin_unlock(&spinDebug);
	#endif
	return 0;
}

/*
	函数：获取内存队列长度限制
	参数：无
	返回：内存队列长度限制
*/
template<class T>
unsigned long UnionQueue<T>::maxMemoryQueueLength() {
	return _pushQueue->maxLength();
}

/*
	函数：获取文件路径
	参数：无
	返回：文件路径
*/
template<class T>
std::string UnionQueue<T>::filePath() {
	return _fileQueue->path();
}

/*
	函数：获取单文件大小限制
	参数：无
	返回：单文件大小限制
*/
template<class T>
off_t UnionQueue<T>::maxSingleFileSize() {
	return _fileQueue->maxSingleFileSize();
}

#endif