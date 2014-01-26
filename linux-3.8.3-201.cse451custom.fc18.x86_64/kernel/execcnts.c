#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/types.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE2(execcnts, pid_t, pid, struct exec_count_struct *, execcnts) {
  // Grab a read lock on the process list
  // Since this call does not modify the list, we don't need a write lock
  read_lock(&tasklist_lock);
  
  // Find the Process Control Block
  struct task_struct *p = find_vpid(pid);
  
  // Process not found
  if (p == NULL) {
    return -1;
  }
  
  *execcnts = p->execcnts;
  
  return 0;
}