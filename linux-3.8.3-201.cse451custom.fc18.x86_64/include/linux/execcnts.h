#ifndef _LINUX_EXECCNTS_H
#define _LINUX_EXECCNTS_H

#include <linux/sched.h>

void increment_execcnts(struct task_struct *tsk, int execcnt_type);

#endif
