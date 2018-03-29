#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <cmath>
#include "errno.h"
#include <time.h>
#include "ProcessPool.h"

using namespace std;

void primeSieve(int number);
void printResult(vector<bool> nums, int index, int low_bound, int start);

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
            loc = i + i;
            while (loc < father_range) {
                  prime[loc] = false;
                  loc += i;
            }
        }
    }
}

void primeSieve(int number) {
    int total = pow(2, number) * 1000;
    int process_number = sqrt(total) / 2;
    static ProcessPool* mypool = ProcessPool::createPool(process_number);

    if (mypool->getIndex() < 0) {
        //这个进程代表的是父进程
        int father_range = total / (process_number + 1) + total % (process_number + 1) + 1;
        vector<bool> prime(father_range, true);
        prime[0] = prime[1] = false;

        changeAllMultiples(prime, father_range, process_number, mypool);

        for (int i = 0; i < process_number; i++) {
          close(mypool->getSubProcess()[i].getfiledes()[1]);
        }

        //  等待所有的子进程全部结束
        clock_t begin = mypool->getSubProcess()[processId].getBeginTime();
        printResult(prime, 0, 0, begin);
	//  计算并且打印总时间
        int status;
        pid_t wpid;
        while ((wpid = wait(&status)) > 0);
        clock_t now = clock();
        int processId = mypool->getIndex() + process_number + 1;
        double time = (double)(now - begin) / CLOCKS_PER_SEC;
        cout << "TotalTime: " << time << endl << endl;
    } else {
        int index = mypool->getIndex();
        int* step = new int();
        int low_bound = total / (process_number + 1) * (index + 1) + total % (process_number + 1) + 1;
        int sub_range = total / (process_number + 1);
        vector<bool> nums(sub_range, true);
        while (1) {
            //  这里是如何实现选择到不同的值的
            int size = read(mypool->getSubProcess()[index].getfiledes()[0], step, 4);
            if (size > 0) {
                //  需要找出的是第一个能被*step整除的数
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
        clock_t start = mypool -> getSubProcess()[index].getBeginTime();
        printResult(nums, index, low_bound, start);
    }
}
void printResult(vector<bool> nums, int index, int low_bound, int start) {
    int n = nums.size();
    cout << "process " << index << ": ";
    for (int i = 0; i < n; i++) {
        if (nums[i])
            cout << i + low_bound << " ";
    }
    cout << endl;
    clock_t now = clock();
    double time = (double)(now - start) / CLOCKS_PER_SEC;
    if (index >= 0)
        cout << "TIME_COST: " << time << endl << endl;
}
