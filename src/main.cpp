#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include "errno.h"
#include "ProcessPool.h"

using namespace std;

void primeSieve(int number);
void printResult(vector<bool> nums, int index, int low_bound, int start);
void changeAllMultiples(vector<bool>& prime, int father_range, int process_number, ProcessPool* pool);

int main() {
    int num;
    cout << "Please input the number between 0 and 9: ";
    cin >> num;
    // num: 0 ~ 9
    while (num < 0 || num > 9) {
        cout << "Please input the number between 0 and 9: ";
        cin >> num;
    }
    primeSieve(num);
    return 0;
}

void changeAllMultiples(vector<bool>& prime, int father_range, int process_number, ProcessPool* pool) {
    int loc;
    for (int i = 2; i < father_range; i++) {
        loc = i;
        if (prime[i]) {
            for (int j = 0; j < process_number; j++) {
                write(pool->getSubProcess()[j].getfiledes()[1], &loc, 4);
            }
            for (loc = i + i; loc < father_range; loc += i)
                prime[loc] = false;
        }
    }
}

void primeSieve(int number) {
    int total = pow(2, number) * 1000;
    int process_number = sqrt(total) / 2;
    static ProcessPool* mypool = ProcessPool::createPool(process_number);
    int index = mypool->getIndex();
    if (index < 0) {
        //这个进程代表的是父进程
        int father_range = total / (process_number + 1) + total % (process_number + 1) + 1;
        vector<bool> prime(father_range, true);
        prime[0] = prime[1] = false;
        changeAllMultiples(prime, father_range, process_number, mypool);
        //   close 一个套接字的默认行为是把套接字标记为已关闭，然后立即返回到调用进程，该套接字描述符不能再由调用进程使用，
        //   也就是说它不能再作为read或write的第一个参数，然而TCP将尝试发送已排队等待发送到对端的任何数据，
        //   发送完毕后发生的是正常的TCP连接终止序列。在多进程并发服务器中，父子进程共享着套接字，套接字
        //   描述符引用计数记录着共享着的进程个数，当父进程或某一子进程close掉套接字时，描述符引用计数会相应的减一，
        //   当引用计数仍大于零时，这个close调用就不会引发TCP的四路握手断连过程。
        //   close成功为0,出错为-1
        for (int i = 0; i < process_number; i++)
            close(mypool->getSubProcess()[i].getfiledes()[1]);

        //  等待所有的子进程全部结束
        int processId = index + process_number + 1;
        clock_t begin = mypool->getSubProcess()[processId].getBeginTime();
        printResult(prime, 0, 0, begin);
        //  计算并且打印总时间
        int status;
        pid_t wpid;
        while ((wpid = wait(&status)) > 0);
        clock_t now = clock();
        double time = (double)(now - begin) / CLOCKS_PER_SEC;
        cout << "TotalTime: " << time << endl;
    } else {
        int* step = new int();
        int low_bound = total / (process_number + 1) * (index + 1) + total % (process_number + 1) + 1;
        int sub_range = total / (process_number + 1);
        vector<bool> nums(sub_range, true);

        // ssize_t read(int fd,void *buf,size_t count); 
        // 函数参数的含义:从文件描述符fd所指的文件中读取count个字节的数据到buf所指向的缓冲区，
        // count为０，不读数据，返回0,返回值就是实际读取的字节数，如果read()顺利返回实际读到的字节数，
        // 和参数count比较，若返回值＜count，说明文件到了文件末尾，或读取过程中被信号中断了读取过程，
        // 有错误时返回－１；

        while (true) {
            int size = read(mypool->getSubProcess()[index].getfiledes()[0], step, 4);
            if (size <= 0) break;
            //  需要找出的是第一个能被*step整除的数
            int value = *step;
            int start = (low_bound / value) * value == low_bound ? low_bound : (low_bound / value) * value + value;
            for (int i = start; i < low_bound + sub_range; i += value)
                nums[i - low_bound] = false;
        }

        clock_t start = mypool -> getSubProcess()[index].getBeginTime();
        printResult(nums, index, low_bound, start);
    }
}
void printResult(vector<bool> nums, int index, int low_bound, int start) {
    int n = nums.size();
    if (low_bound != 0) index += 1;
    printf(" \033[40;31mprocess\033[0m ");
    cout << index << ": ";
    for (int i = 0; i < n; i++) {
        if (nums[i])
            cout << i + low_bound << " ";
    }
    cout << endl;
    clock_t now = clock();
    double time = (double)(now - start) / CLOCKS_PER_SEC;
    if (low_bound != 0) {
        printf("\033[40;32mTIME_COST\033[0m");
        cout << " : " << time << endl << endl;
    } else {
        cout << endl;
    }
}
