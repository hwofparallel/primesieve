#include <unistd.h>
#include <iostream>
#include "ProcessPool.h"
using namespace std;

ProcessPool::ProcessPool(int number) {
    process_number = number;
    index = -1;
    sub_process = new Process[number];
    for (int i = 0; i < number; i++) {
        //  为每一个进程创建一个管道，成功返回0,失败返回-1
        if (pipe(sub_process[i].getfiledes()) < 0) {
            cout << "error occurred when pipe!" << endl;
        }
	    //  使用fork创建子进程
        pid_t pid = fork();
        clock_t beginTime;
        if (pid < 0) {
            cout << "error occurred when fork" << endl;
        } else if (pid == 0) {
            //  子进程
            beginTime = clock();
            index = i;
            close(sub_process[i].getfiledes()[1]);
            break;
        } else {
            //  父进程，返回子进程的id
            beginTime = clock();
            sub_process[i].setId(pid);
            index = i - number;
            close(sub_process[i].getfiledes()[0]);
        } 
        sub_process[i].setBeginTime(beginTime);
    }
}

ProcessPool* ProcessPool::createPool(int number) {
    static ProcessPool instance(number);
    return &instance;
}

Process* ProcessPool::getSubProcess() {
    return sub_process;
}

int ProcessPool::getIndex() {
    return index;
}

ProcessPool::~ProcessPool() {
  delete []sub_process;
}
