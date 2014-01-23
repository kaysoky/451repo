#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE1(execcnts, pid_t, pid) {
  printk("Hello world: execcnts\n");
  return 0;
}