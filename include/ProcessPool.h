#ifndef PROCESSPOOL_H
#define PROCESSPOOL_H
#include "Process.h"
#include <unistd.h>

class ProcessPool {
public:
	ProcessPool(int number);
	// 单例模式
	static ProcessPool* createPool(int number);
	int getIndex();
	Process* getSubProcess();
	~ProcessPool();
private:
	Process* sub_process;
	int process_number;
	int index;
};
#endif