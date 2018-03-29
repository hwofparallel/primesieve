#ifndef PROCESS_H
#define PROCESS_H
#include <sys/types.h>
#include <time.h>

class Process {
public:
	Process() {
		m_pid = -1;
	}
	void setId(pid_t id) {
		m_pid = id;
	}
	void setBeginTime(clock_t time) {
		beginTime = time;
	}
	int * getfiledes() {
		return filedes;	
	}
	clock_t getBeginTime() {
		return beginTime;
	}

private:
	pid_t m_pid;
	// filedes[0] is to read pipe, filedes[1] is to write pipe
	int filedes[2];
	int beginTime;
};
#endif

