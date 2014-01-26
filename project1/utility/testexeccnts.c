#include <stdio.h>
#include <unistd.h>

#define EXECCNTS_64 314
#define EXECCNTS_32 351

int main(int argc, char *argv[]) {
    int ret[4];
    long res = syscall(EXECCNTS_64, &ret);
    
    printf("System call returned: %d\n", res);
    printf("%d %d %d %d\n", ret[0], ret[1], ret[2], ret[3]);
    return 0;
}
