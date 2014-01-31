#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/syscalls.h>
#include <linux/execcnts.h>

void increment_execcnts(struct task_struct *tsk, int execcnt_type) {
    if (execcnt_type < EXECCNTS_FORK_INDEX 
            || execcnt_type > EXECCNTS_CLONE_INDEX) {
        // Improper usage of this helper method
        return;
    }
    
    // Increment the counter
    tsk->execcnts[execcnt_type]++;
    
    // Increment the parent's counter
    if (tsk->parent != NULL && tsk != tsk->parent) {
        increment_execcnts(tsk->parent, execcnt_type);
    }
}

void reset_execcnts(struct task_struct *tsk) {
    tsk->execcnts[EXECCNTS_FORK_INDEX] = 0;
    tsk->execcnts[EXECCNTS_VFORK_INDEX] = 0;
    tsk->execcnts[EXECCNTS_EXECVE_INDEX] = 0;
    tsk->execcnts[EXECCNTS_CLONE_INDEX] = 0;
}

// Helper for the execcnts system call
long do_execcnts(pid_t pid, int __user *execcnts) {
  // Find the Process Control Block
  struct task_struct *p = find_task_by_vpid(pid);
  
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

SYSCALL_DEFINE2(execcnts, pid_t, pid, int __user *, execcnts) {
  long ret;
  
  // Wrap the logic of the system call with a read lock on the task list
  // Since this call does not modify the list, we don't need a write lock
  // But we do not want to operate on a changing task list
  read_lock(&tasklist_lock);
  ret = do_execcnts(pid, execcnts);
  read_unlock(&tasklist_lock);
  
  return ret;
}