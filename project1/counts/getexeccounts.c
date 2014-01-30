#include "getexeccounts.h"

#define NUM_ELEMENT 4

int getExecCounts(int pid, int* pArray)
{
	int i;
	for (i = 0; i < NUM_ELEMENT; ++i)
		pArray[i] = i;

	return 0;
}