#ifndef _GETEXECOUNTS_H
#define _GETEXECOUNTS_H 

/* 
 * A library routine that returns the numbers of times 
 * the fork, vfork, clone and exec system calls
 * have been called
 */
int getExecCounts(int pid, int* pArray);

#endif // _GETEXECOUNTS_H