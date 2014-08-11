/*
	作者：Piovano
	日期：2014.05.26
	版本：1.0
	描述：多文件队列
*/

#ifndef _QUEUE_MULTI_FILE_C
#define _QUEUE_MULTI_FILE_C

#ifndef _QUEUE_MULTI_FILE_H

#include "queue_multi_file.h"

/*
	函数：整型转字符串
	参数：整型数据
	返回：字符串数据
*/
std::string MultiFileQueue<void>::intToString(int i) {
	std::stringstream stringStream;
	stringStream<<i;
	std::string s;
	stringStream>>s;
	return s;
}

/*
	函数：构造
	参数：文件路径前缀，单个文件大小上限
	备注：实际生成的文件路径为“前缀+序号”形式
*/
MultiFileQueue<void>::MultiFileQueue(std::string path, off_t maxSingleFileSize) {
	_path = path;
	_maxSingleFileSize = maxSingleFileSize;
	_maxFileQueueCount = 64;
	_fileId = 0;
	_fileQueues = new FileQueue<void> * [_maxFileQueueCount];
	*_fileQueues = new FileQueue<void>(_path + intToString(_fileId), _maxSingleFileSize);
	_head = _fileQueues;
	_tail = _fileQueues;
	pthread_mutex_init(&_mutexPushPop, NULL);
}

/*
	函数：析构
*/
MultiFileQueue<void>::~MultiFileQueue() {
	while (_head != _tail) {
		delete *_head;
		_head = _head + 1;
	}
	delete *_tail;
	delete[] _fileQueues;
	pthread_mutex_destroy(&_mutexPushPop);
}

/*
	函数：扩充单文件队列
	参数：无
	返回：无
*/
void MultiFileQueue<void>::expandFileQueues() {
	_maxFileQueueCount = _maxFileQueueCount + _maxFileQueueCount;
	FileQueue<void> **old = _fileQueues;
	_fileQueues = new FileQueue<void> * [_maxFileQueueCount];
	int size = _tail - _head + 1;
	memcpy(_fileQueues, _head, size * sizeof(FileQueue<void> *));
	_head = _fileQueues;
	_tail = _head + size - 1;
	delete[] old;
}

/*
	函数：新建单文件队列
	参数：无
	返回：无
*/
void MultiFileQueue<void>::newFileQueue() {
	if (_tail == _fileQueues + _maxFileQueueCount - 1) {
		expandFileQueues();
	}
	_tail = _tail + 1;
	_fileId = _fileId + 1;
	*_tail = new FileQueue<void>(_path + intToString(_fileId), _maxSingleFileSize);
}

/*
	函数：删除单文件队列
	参数：无
	返回：无
*/
void MultiFileQueue<void>::deleteFileQueue() {
	delete *_head;
	_head = _head + 1;
}

/*
	函数：入队
	参数：指针、大小、阻塞
	返回：错误码
*/
int MultiFileQueue<void>::doPush(void *pointer, off_t size, bool block) {
	pthread_mutex_lock(&_mutexPushPop);
	int result = (*_tail)->push(pointer, false);
	if (result != 0) {
		if (result == ERROR_DATA_SIZE_IS_TOO_LARGE) {
			return result;
		} else {
			newFileQueue();
			result = (*_tail)->push(pointer, false);
		}
	}
	pthread_mutex_unlock(&_mutexPushPop);
	sem_post(&(this->_semPop));
	return 0;
}

/*
	函数：出队
	参数：指针、大小、阻塞
	返回：错误码
*/
int MultiFileQueue<void>::doPop(void *&pointer, off_t &size, bool block) {
	if (block) {
		sem_wait(&(this->_semPop));
	} else {
		if (sem_trywait(&(this->_semPop)) != 0) {
			return ERROR_QUEUE_IS_EMPTY;
		}
	}
	pthread_mutex_lock(&_mutexPushPop);
	while ((*_head)->pop(pointer, size, false) != 0) {
		if (_head < _tail) {
			deleteFileQueue();
		} else {
			std::cout<<"IMPORTANT"<<std::endl;
			exit(0);
			// TODO 若信号量指示的队列长度与实际长度不符，则进入此分支。可能吗？
		}
	}
	pthread_mutex_unlock(&_mutexPushPop);
	return 0;
}

/*
	函数：获取路径
	参数：无
	返回：路径
*/
std::string MultiFileQueue<void>::path() {
	return _path;
}

/*
	函数：获取单文件大小限制
	参数：无
	返回：单文件大小限制
*/
off_t MultiFileQueue<void>::maxSingleFileSize() {
	return _maxSingleFileSize;
}

#endif

/*
	函数：整型转字符串
	参数：整型数据
	返回：字符串数据
*/
template<class T>
std::string MultiFileQueue<T>::intToString(int i) {
	std::stringstream stringStream;
	stringStream<<i;
	std::string s;
	stringStream>>s;
	return s;
}

/*
	函数：构造
	参数：数据大小、文件路径前缀，单个文件大小上限
	备注：实际生成的文件路径为“前缀+序号”形式
*/
template<class T>
MultiFileQueue<T>::MultiFileQueue(off_t dataSize, std::string path, off_t maxSingleFileSize) : AbstractQueue<T>(dataSize) {
	_path = path;
	_maxSingleFileSize = maxSingleFileSize;
	_maxFileQueueCount = 64;
	_fileId = 0;
	_fileQueues = new FileQueue<T> * [_maxFileQueueCount];
	*_fileQueues = new FileQueue<T>(dataSize, _path + intToString(_fileId), _maxSingleFileSize);
	#ifdef DEBUG_MULTI_FILE
	pthread_spin_lock(&spinDebug);
	std::cout<<"["<<clock()<<"] "<<"Create File: "<<_path + intToString(_fileId)<<std::endl;
	pthread_spin_unlock(&spinDebug);
	#endif
	_head = _fileQueues;
	//std::cout<<"Init(H): "<<(intptr_t)_head<<"->"<<(intptr_t)*_head<<std::endl;
	_tail = _fileQueues;
	//std::cout<<"Init(T): "<<(intptr_t)_head<<"->"<<(intptr_t)*_head<<std::endl;
	pthread_mutex_init(&_mutexPushPop, NULL);
}

/*
	函数：析构
*/
template<class T>
MultiFileQueue<T>::~MultiFileQueue() {
	while (_head != _tail) {
		delete *_head;
		_head = _head + 1;
	}
	delete *_tail;
	delete[] _fileQueues;
	pthread_mutex_destroy(&_mutexPushPop);
}

/*
	函数：扩充单文件队列
	参数：无
	返回：无
*/
template<class T>
void MultiFileQueue<T>::expandFileQueues() {
	_maxFileQueueCount = _maxFileQueueCount + _maxFileQueueCount;
	FileQueue<T> **old = _fileQueues;
	_fileQueues = new FileQueue<T> * [_maxFileQueueCount];
	int size = _tail - _head + 1;
	memcpy(_fileQueues, _head, size * sizeof(FileQueue<T> *));
	_head = _fileQueues;
	//std::cout<<"Expand(H): "<<(intptr_t)_head<<"->"<<(intptr_t)*_head<<std::endl;
	_tail = _head + size - 1;
	//std::cout<<"Expand(T): "<<(intptr_t)_head<<"->"<<(intptr_t)*_head<<std::endl;
	delete[] old;
}

/*
	函数：新建单文件队列
	参数：无
	返回：无
*/
template<class T>
void MultiFileQueue<T>::newFileQueue() {
	if (_tail == _fileQueues + _maxFileQueueCount - 1) {
		expandFileQueues();
	}
	_tail = _tail + 1;
	_fileId = _fileId + 1;
	*_tail = new FileQueue<T>(this->_dataSize, _path + intToString(_fileId), _maxSingleFileSize);
	#ifdef DEBUG_MULTI_FILE
	pthread_spin_lock(&spinDebug);
	std::cout<<"["<<clock()<<"] "<<"Create File: "<<_path + intToString(_fileId)<<std::endl;
	pthread_spin_unlock(&spinDebug);
	#endif
}

/*
	函数：删除单文件队列
	参数：无
	返回：无
*/
template<class T>
void MultiFileQueue<T>::deleteFileQueue() {
	delete *_head;
	_head = _head + 1;
	#ifdef DEBUG_MULTI_FILE
	pthread_spin_lock(&spinDebug);
	std::cout<<"["<<clock()<<"] "<<"Delete File."<<std::endl;
	pthread_spin_unlock(&spinDebug);
	#endif
}

/*
	函数：入队
	参数：指针、阻塞
	返回：错误码
*/
template<class T>
int MultiFileQueue<T>::doPush(T *pointer, bool block) {
	pthread_mutex_lock(&_mutexPushPop);
	int result = (*_tail)->push(pointer, false);
	if (result != 0) {
		if (result == ERROR_DATA_SIZE_IS_TOO_LARGE) {
			return result;
		} else {
			newFileQueue();
			result = (*_tail)->push(pointer, false);
		}
	}
	pthread_mutex_unlock(&_mutexPushPop);
	sem_post(&(this->_semPop));
	return result;
}

/*
	函数：出队
	参数：指针、阻塞
	返回：错误码
*/
template<class T>
int MultiFileQueue<T>::doPop(T *&pointer, bool block) {
	if (block) {
		sem_wait(&(this->_semPop));
	} else {
		if (sem_trywait(&(this->_semPop)) != 0) {
			return ERROR_QUEUE_IS_EMPTY;
		}
	}
	pthread_mutex_lock(&_mutexPushPop);
	while ((*_head)->pop(pointer, false) != 0) {
		if (_head < _tail) {
			deleteFileQueue();
		} else {
			std::cout<<"IMPORTANT"<<std::endl;
			exit(0);
			// TODO 若信号量指示的队列长度与实际长度不符，则进入此分支。可能吗？
		}
	}
	pthread_mutex_unlock(&_mutexPushPop);
	return 0;
}

/*
	函数：获取路径
	参数：无
	返回：路径
*/
template<class T>
std::string MultiFileQueue<T>::path() {
	return _path;
}

/*
	函数：获取单文件大小限制
	参数：无
	返回：单文件大小限制
*/
template<class T>
off_t MultiFileQueue<T>::maxSingleFileSize() {
	return _maxSingleFileSize;
}

#endif