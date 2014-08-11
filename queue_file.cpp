/*
	作者：Piovano
	日期：2014.05.26
	版本：1.0
	描述：单文件队列
*/

#ifndef _QUEUE_FILE_C
#define _QUEUE_FILE_C

#ifndef _QUEUE_FILE_H

#include "queue_file.h"

/*
	函数：构造
	参数：文件路径，文件最大大小
*/
FileQueue<void>::FileQueue(std::string path, off_t maxSize) {
	_path = path;
	_file = -1;
	_size = lseek(_file, 0L, SEEK_END);
	lseek(_file, 0L, SEEK_SET);
	_head = sizeof(off_t) * 2;
	_tail = sizeof(off_t) * 2;
	write(_file, &_head, sizeof(off_t));
	write(_file, &_tail, sizeof(off_t));
	_maxSize = maxSize;
	sem_init(&_semPush, 0, 1);
	pthread_mutex_init(&_mutexReadWrite, NULL);
}

/*
	函数：析构
*/
FileQueue<void>::~FileQueue() {
	closeFile();
	unlink(_path.c_str());
	sem_destroy(&_semPush);
	pthread_mutex_destroy(&_mutexReadWrite);
}

/*
	函数：入队
	参数：指针、大小、阻塞
	返回：错误码
*/
int FileQueue<void>::doPush(void *pointer, off_t size, bool block) {
	pthread_mutex_lock(&_mutexReadWrite);
	if (block) {
		// No Operation
	} else {
		if (size + sizeof(off_t) + sizeof(off_t) * 2 > _maxSize) {
			pthread_mutex_unlock(&_mutexReadWrite);
			return ERROR_DATA_SIZE_IS_TOO_LARGE;
		}
		if (_maxSize - _tail < size + sizeof(off_t)) {
			pthread_mutex_unlock(&_mutexReadWrite);
			return ERROR_QUEUE_IS_FULL;
		}
	}
	openFile();
	lseek(_file, _tail, SEEK_SET);
	write(_file, &size, sizeof(off_t));
	write(_file, pointer, size);
	this->destroy(pointer);
	_tail = lseek(_file, 0L, SEEK_CUR);
	lseek(_file, sizeof(off_t), SEEK_SET);
	write(_file, &_tail, sizeof(off_t));
	pthread_mutex_unlock(&_mutexReadWrite);
	sem_post(&(this->_semPop));
	return 0;
}

/*
	函数：出队
	参数：指针、大小、阻塞
	返回：错误码
*/
int FileQueue<void>::doPop(void *&pointer, off_t &size, bool block) {
	if (block) {
		sem_wait(&(this->_semPop));
	} else {
		if (sem_trywait(&(this->_semPop)) != 0) {
			return ERROR_QUEUE_IS_EMPTY;
		}
	}
	pthread_mutex_lock(&_mutexReadWrite);
	openFile();
	lseek(_file, _head, SEEK_SET);
	read(_file, &size, sizeof(off_t));
	this->create(pointer, size);
	read(_file, pointer, size);
	_head = lseek(_file, 0L, SEEK_CUR);
	lseek(_file, 0L, SEEK_SET);
	write(_file, &_head, sizeof(off_t));
	pthread_mutex_unlock(&_mutexReadWrite);
	return 0;
}

/*
	函数：打开文件
	参数：无
	返回：文件描述符
*/
int FileQueue<void>::openFile() {
	if (_file == -1) {
		_file = open(_path.c_str(), O_RDWR | O_CREAT, S_IRWXU | S_IRWXG);
	}
	return _file;
}

/*
	函数：关闭文件
	参数：无
	返回：文件描述符
*/
int FileQueue<void>::closeFile() {
	if (_file != -1) {
		close(_file);
		_file = -1;
	}
	return _file;
}

/*
	函数：获取路径
	参数：无
	返回：路径
*/
std::string FileQueue<void>::path() {
	return _path;
}

/*
	函数：获取文件大小限制
	参数：无
	返回：文件大小限制
*/
off_t FileQueue<void>::maxSize() {
	return _maxSize;
}

/*
	函数：输出队列内容
	参数：无
	返回：无
	备注：测试用，非线程安全，在多线程环境中使用可能会输出不正确的结果
*/
void FileQueue<void>::print() {
	if (this->length() == 0) {
		return;
	}
	std::cout<<"QueueLength: "<<this->length()<<std::endl;
	off_t temp, size;
	temp = _head;
	while (true) {
		lseek(_file, temp, SEEK_SET);
		read(_file, &size, sizeof(off_t));
		std::cout<<temp<<"\t| "<<size<<std::endl;
		temp = temp + sizeof(off_t) + size;
		if (temp == _tail) {
			break;
		}
	}
	std::cout<<std::endl;
}

#endif

/*
	函数：构造
	参数：数据大小，文件路径，文件最大大小
*/
template<class T>
FileQueue<T>::FileQueue(off_t dataSize, std::string path, off_t maxSize) : AbstractQueue<T>(dataSize) {
	_path = path;
	_file = -1;
	_size = lseek(_file, 0L, SEEK_END);
	lseek(_file, 0L, SEEK_SET);
	_head = sizeof(off_t) * 2;
	_tail = sizeof(off_t) * 2;
	write(_file, &_head, sizeof(off_t));
	write(_file, &_tail, sizeof(off_t));
	_maxSize = maxSize;
	sem_init(&_semPush, 0, 1);
	pthread_mutex_init(&_mutexReadWrite, NULL);
}

/*
	函数：析构
*/
template<class T>
FileQueue<T>::~FileQueue() {
	close(_file);
	unlink(_path.c_str());
	sem_destroy(&_semPush);
	pthread_mutex_destroy(&_mutexReadWrite);
}

/*
	函数：入队
	参数：指针、阻塞
	返回：错误码
*/
template<class T>
int FileQueue<T>::doPush(T *pointer, bool block) {
	pthread_mutex_lock(&_mutexReadWrite);
	if (block) {
		// No Operation
	} else {
		if (this->_dataSize + sizeof(off_t) * 2 > _maxSize) {
			pthread_mutex_unlock(&_mutexReadWrite);
			return ERROR_DATA_SIZE_IS_TOO_LARGE;
		}
		if (_maxSize - _tail < this->_dataSize) {
			pthread_mutex_unlock(&_mutexReadWrite);
			return ERROR_QUEUE_IS_FULL;
		}
	}
	openFile();
	lseek(_file, _tail, SEEK_SET);
	write(_file, pointer, this->_dataSize);
	this->destroy(pointer);
	_tail = lseek(_file, 0L, SEEK_CUR);
	lseek(_file, sizeof(off_t), SEEK_SET);
	write(_file, &_tail, sizeof(off_t));
	pthread_mutex_unlock(&_mutexReadWrite);
	sem_post(&(this->_semPop));
	return 0;
}

/*
	函数：出队
	参数：指针、阻塞
	返回：错误码
*/
template<class T>
int FileQueue<T>::doPop(T *&pointer, bool block) {
	if (block) {
		sem_wait(&(this->_semPop));
	} else {
		if (sem_trywait(&(this->_semPop)) != 0) {
			return ERROR_QUEUE_IS_EMPTY;
		}
	}
	pthread_mutex_lock(&_mutexReadWrite);
	openFile();
	lseek(_file, _head, SEEK_SET);
	this->create(pointer);
	read(_file, pointer, this->_dataSize);
	_head = lseek(_file, 0L, SEEK_CUR);
	lseek(_file, 0L, SEEK_SET);
	write(_file, &_head, sizeof(off_t));
	pthread_mutex_unlock(&_mutexReadWrite);
	return 0;
}

/*
	函数：打开文件
	参数：无
	返回：文件描述符
*/
template<class T>
int FileQueue<T>::openFile() {
	if (_file == -1) {
		_file = open(_path.c_str(), O_RDWR | O_CREAT, S_IRWXU | S_IRWXG);
	}
	return _file;
}

/*
	函数：关闭文件
	参数：无
	返回：文件描述符
*/
template<class T>
int FileQueue<T>::closeFile() {
	if (_file != -1) {
		close(_file);
		_file = -1;
	}
	return _file;
}

/*
	函数：获取路径
	参数：无
	返回：路径
*/
template<class T>
std::string FileQueue<T>::path() {
	return _path;
}

/*
	函数：获取文件大小限制
	参数：无
	返回：文件大小限制
*/
template<class T>
off_t FileQueue<T>::maxSize() {
	return _maxSize;
}

/*
	函数：输出队列内容
	参数：无
	返回：无
	备注：测试用，非线程安全，在多线程环境中使用可能会输出不正确的结果
*/
template<class T>
void FileQueue<T>::print() {
	if (this->length() == 0) {
		return;
	}
	std::cout<<"QueueLength: "<<this->length()<<std::endl;
	off_t temp;
	temp = _head;
	while (true) {
		lseek(_file, temp, SEEK_SET);
		temp = temp + this->_dataSize;
		if (temp == _tail) {
			break;
		}
	}
	std::cout<<std::endl;
}

#endif