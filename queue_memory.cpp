/*
	作者：Piovano
	日期：2014.05.26
	版本：1.0
	描述：内存队列
*/

#ifndef _QUEUE_MEMORY_C
#define _QUEUE_MEMORY_C

#ifndef _QUEUE_MEMORY_H

#include "queue_memory.h"

/*
	函数：构造
	参数：队列最大长度
*/
MemoryQueue<void>::MemoryQueue(unsigned long maxLength) {
	_maxLength = maxLength;
	_queue = new MemoryBlock[_maxLength];
	_head = _queue;
	_tail = _queue - 1;
	sem_init(&_semPush, 0, _maxLength);
	pthread_spin_init(&_spinPush, PTHREAD_PROCESS_PRIVATE);
	pthread_spin_init(&_spinPop, PTHREAD_PROCESS_PRIVATE);
}

/*
	函数：析构
*/
MemoryQueue<void>::~MemoryQueue() {
	delete[] _queue;
	sem_destroy(&_semPush);
	pthread_spin_destroy(&_spinPush);
	pthread_spin_destroy(&_spinPop);
}

/*
	函数：入队
	参数：指针、大小、阻塞
	返回：错误码
*/
int MemoryQueue<void>::doPush(void *pointer, off_t size, bool block) {
	if (block) {
		sem_wait(&_semPush);
	} else {
		if (sem_trywait(&_semPush) != 0) {
			return ERROR_QUEUE_IS_FULL;
		}
	}
	pthread_spin_lock(&_spinPush);
	if (_tail == _queue + _maxLength - 1) {
		_tail = _queue;
	} else {
		_tail = _tail + 1;
	}
	_tail->pointer = pointer;
	_tail->size = size;
	pthread_spin_unlock(&_spinPush);
	sem_post(&(this->_semPop));
	return 0;
}

/*
	函数：出队
	参数：指针、大小、阻塞
	返回：错误码
*/
int MemoryQueue<void>::doPop(void *&pointer, off_t &size, bool block) {
	if (block) {
		sem_wait(&(this->_semPop));
	} else {
		if (sem_trywait(&(this->_semPop)) != 0) {
			return ERROR_QUEUE_IS_EMPTY;
		}
	}
	pthread_spin_lock(&_spinPop);
	pointer = _head->pointer;
	size = _head->size;
	if (_head == _queue + _maxLength - 1) {
		_head = _queue;
	} else {
		_head = _head + 1;
	}
	pthread_spin_unlock(&_spinPop);
	sem_post(&_semPush);
	return 0;
}

/*
	函数：获取队列长度限制
	参数：无
	返回：队列长度限制
*/
unsigned long MemoryQueue<void>::maxLength() {
	return _maxLength;
}

/*
	函数：输出队列内容
	参数：无
	返回：无
	备注：测试用，非线程安全，在多线程环境中使用可能会输出不正确的结果
*/
void MemoryQueue<void>::print() {
	if (this->length() == 0) {
		return;
	}
	std::cout<<"QueueLength: "<<this->length()<<std::endl;
	MemoryBlock *temp;
	temp = _head;
	while (true) {
		std::cout<<(temp - _queue)<<"\t| "<<(temp - _head)<<"\t| "<<(temp->size)<<std::endl;
		if (temp < _queue + _maxLength - 1) {
			temp = temp + 1;
		} else {
			temp = _queue;
		}
		if (temp == _tail + 1) {
			break;
		}
	}
}

#endif

/*
	函数：构造
	参数：数据大小，队列最大长度
*/
template<class T>
MemoryQueue<T>::MemoryQueue(off_t dataSize, unsigned long maxLength) : AbstractQueue<T>(dataSize) {
	_maxLength = maxLength;
	_queue = new T*[_maxLength];
	_head = _queue;
	_tail = _queue - 1;
	sem_init(&_semPush, 0, _maxLength);
	pthread_spin_init(&_spinPush, PTHREAD_PROCESS_PRIVATE);
	pthread_spin_init(&_spinPop, PTHREAD_PROCESS_PRIVATE);
}

/*
	函数：析构
*/
template<class T>
MemoryQueue<T>::~MemoryQueue() {
	delete[] _queue;
	sem_destroy(&_semPush);
	pthread_spin_destroy(&_spinPush);
	pthread_spin_destroy(&_spinPop);
}

/*
	函数：入队
	参数：指针、阻塞
	返回：错误码
*/
template<class T>
int MemoryQueue<T>::doPush(T *pointer, bool block) {
	if (block) {
		sem_wait(&_semPush);
	} else {
		if (sem_trywait(&_semPush) != 0) {
			return ERROR_QUEUE_IS_FULL;
		}
	}
	pthread_spin_lock(&_spinPush);
	if (_tail == _queue + _maxLength - 1) {
		_tail = _queue;
	} else {
		_tail = _tail + 1;
	}
	*_tail = pointer;
	pthread_spin_unlock(&_spinPush);
	sem_post(&(this->_semPop));
	return 0;
}

/*
	函数：出队
	参数：指针、阻塞
	返回：错误码
*/
template<class T>
int MemoryQueue<T>::doPop(T *&pointer, bool block) {
	if (block) {
		sem_wait(&(this->_semPop));
	} else {
		if (sem_trywait(&(this->_semPop)) != 0) {
			return ERROR_QUEUE_IS_EMPTY;
		}
	}
	pthread_spin_lock(&_spinPop);
	pointer = *_head;
	if (_head == _queue + _maxLength - 1) {
		_head = _queue;
	} else {
		_head = _head + 1;
	}
	pthread_spin_unlock(&_spinPop);
	sem_post(&_semPush);
	return 0;
}

/*
	函数：获取队列长度限制
	参数：无
	返回：队列长度限制
*/
template<class T>
unsigned long MemoryQueue<T>::maxLength() {
	return _maxLength;
}

/*
	函数：输出队列内容
	参数：无
	返回：无
	备注：测试用，非线程安全，在多线程环境中使用可能会输出不正确的结果
*/
template<class T>
void MemoryQueue<T>::print() {
	if (this->length() == 0) {
		return;
	}
	std::cout<<"QueueLength: "<<this->length()<<std::endl;
	T **temp;
	temp = _head;
	while (true) {
		std::cout<<(temp - _queue)<<"\t| "<<(temp - _head)<<"\t| "<<(temp->size)<<std::endl;
		if (temp < _queue + _maxLength - 1) {
			temp = temp + 1;
		} else {
			temp = _queue;
		}
		if (temp == _tail + 1) {
			break;
		}
	}
}

#endif