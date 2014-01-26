#include <unistd.h>
#include <linux/sched.h>

#define EXECCNTS_64 314
#define EXECCNTS_32 351

int main(int argc, char *argv[]) {
    int ret[4];
    syscall(EXECCNTS_64, &ret);
    
    printf("%u %u %u %u\n", ret[0], ret[1], ret[2], ret[3]);
}
