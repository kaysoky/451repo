#include <unistd.h>
#include <linux/sched.h>

#DEFINE EXECCNTS_64 314
#DEFINE EXECCNTS_32 351

int main(int argc, char *argv[]) {
    struct exec_count_struct ret;
    syscall(EXECCNTS_64, &ret);
    
    printf("%u %u %u %u\n", ret.fork, ret.vfork, ret.execve, ret.clone);
}
