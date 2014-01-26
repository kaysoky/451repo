#ifndef _LINUX_EXECCNTS_H
#define _LINUX_EXECCNTS_H

#include <linux/sched.h>

#define EXECCNTS_FORK_INDEX 0
#define EXECCNTS_VFORK_INDEX 1
#define EXECCNTS_EXECVE_INDEX 2
#define EXECCNTS_CLONE_INDEX 3

void increment_execcnts(struct task_struct *tsk, int execcnt_type);

#endif
