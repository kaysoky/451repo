#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/syscalls.h>
#include <linux/execcnts.h>

// Recurses up a process tree 
// and increments the appropriate execcnts of each process by one
void increment_execcnts(struct task_struct *tsk, int execcnt_type) {
    if (execcnt_type < EXECCNTS_FORK_INDEX 
            || execcnt_type > EXECCNTS_CLONE_INDEX) {
        // Improper usage of this helper method
        return;
    }
    
    // Increment the counter
    tsk->execcnts[execcnt_type]++;
    
    // Increment the parent's counter
    if (tsk->real_parent != NULL) {
        increment_execcnts(tsk->real_parent, execcnt_type);
    }
}

SYSCALL_DEFINE2(execcnts, pid_t, pid, int __user *, execcnts) {
  // Grab a read lock on the process list
  // Since this call does not modify the list, we don't need a write lock
  read_lock(&tasklist_lock);
  
  // Find the Process Control Block
  struct task_struct *p = find_task_by_vpid(pid);
  read_unlock(&tasklist_lock);
  
  // Process not found
  if (p == NULL) {
    return -1;
  }
  
  // Copy the counts into user space
  if (copy_to_user(execcnts, &p->execcnts, 4 * sizeof(int))) {
    return -EFAULT;
  }
  
  return 0;
}