#include <unistd.h>
#include <sys/types.h>

int main() {
    syscall(58);
    exit(0);
}
