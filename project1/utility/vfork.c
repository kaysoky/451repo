#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main() {
    syscall(58);
    exit(0);
}
