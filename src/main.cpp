#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <cmath>
#include "errno.h"
#include <time.h>
#include "../include/ProcessPool.h"

using namespace std;
void primeSieve(int number);
clock_t start;

int main() {
    int num;
    cout << "Please input the number between 0 and 9: ";
    cin >> num;
    while (num < 0 || num > 9) {
        cout << "Please input the number between 0 and 9: ";
        cin >> num;
    }
    primeSieve(num);
    return 0;
}

void primeSieve(int number) {
    int total = pow(2, number) * 1000;
    int process_number = sqrt(total) / 2;
    static ProcessPool* mypool = ProcessPool::createPool(process_number);
    int loc = 0;

    if (mypool -> getIndex() < 0) {
    //这个进程代表的是父进程
    int father_range = total / (process_number + 1) + total % (process_number + 1) + 1;
    vector<bool> num(father_range, true);
    num[0] = num[1] = false;
    for (int i = 2; i < father_range; i++) {
        loc = i;
        if (num[i]) {
            for (int j = 0; j < process_number; j++) {
              write(mypool->getSubProcess()[j].getfds()[1], &loc, 4);
            }
            loc = i + i;
            while (loc < father_range) {
              num[loc] = false;
              loc += i;
            }
        }
    }
    for (int i = 0; i < process_number; i++) {
      close(mypool->getSubProcess()[i].getfds()[1]);
    }

    //等待所有的子进程全部结束
    int size = num.size();
    cout << "process 0: ";
    for (int i = 0; i < size; i++) {
      if (num[i]) {
        cout << i << " ";
      }
    }
    cout << endl;
    int status;
    pid_t wpid;
    while ((wpid = wait(&status)) > 0);
        clock_t now = clock();
        int processId = mypool -> getIndex() + process_number + 1;
        clock_t start = mypool -> getSubProcess()[processId].getBeginTime();
        double time = (double)(now - start) / CLOCKS_PER_SEC;
        cout << "Totaltime: " << time << endl << endl;
    } else {
        int index = mypool -> getIndex();
        int low_bound = total / (process_number + 1) * (index + 1) + total % (process_number + 1) + 1;
        int sub_range = total / (process_number + 1);
        int* step = new int();
        vector<bool> nums(sub_range, true);
        while (1) {
          //这里是如何实现选择到不同的值的
          int size = read(mypool->getSubProcess()[index].getfds()[0], step, 4);
          if (size > 0) {
            //需要找出的是第一个能被*step整除的数
            int start = 0;
            start = (low_bound / (*step)) * (*step) == low_bound ? low_bound : (low_bound / (*step)) * (*step) + (*step);
            for (int i = start; i < low_bound + sub_range;) {
              nums[i - low_bound] = false;
              i += *step;
            }
          } else {
            break;
          }
    }
    int size = nums.size();
    cout << "process " << index << ": ";
    for (int i = 0; i < size; i++) {
      if (nums[i]) {
        cout << i + low_bound << " ";
      }
    }
    cout << endl;
    clock_t now = clock();
    clock_t start = mypool -> getSubProcess()[index].getBeginTime();
    double time = (double)(now - start) / CLOCKS_PER_SEC;
    cout << "time: " << time << endl << endl;
    }
}
