#include "getexeccounts.h"

#define EXECCNTS_64 314

int getExecCounts(int pid, int* pArray) {
	int res;
    res = syscall(EXECCNTS_64, pid, pArray);
	return res;
}