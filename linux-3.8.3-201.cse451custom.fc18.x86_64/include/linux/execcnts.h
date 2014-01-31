#ifndef _LINUX_EXECCNTS_H
#define _LINUX_EXECCNTS_H

#include <linux/sched.h>

#define EXECCNTS_FORK_INDEX 0
#define EXECCNTS_VFORK_INDEX 1
#define EXECCNTS_EXECVE_INDEX 2
#define EXECCNTS_CLONE_INDEX 3

/* 
 * Increments the appropriate exec counts 
 * of the specified process and its parent by one
 */ 
void increment_execcnts(struct task_struct *tsk, int execcnt_type);

/*
 * Resets the exec counts of the given process to 0
 * This should be used on the child process after forking or cloning 
 */
void reset_execcnts(struct task_struct *tsk);

#endif
