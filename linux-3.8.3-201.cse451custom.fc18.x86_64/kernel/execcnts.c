#include <linux/syscalls.h>

SYSCALL_DEFINE1(execcnts, unsigned int, pid)
{
  printk("Hello world: execcnts");
}