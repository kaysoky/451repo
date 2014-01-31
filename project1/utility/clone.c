#include <unistd.h>

int main(int argc, char *argv[]) {
    syscall(56);
    return 0;
}
