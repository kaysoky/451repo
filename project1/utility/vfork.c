#include <unistd.h>

int main(int argc, char *argv[]) {
    syscall(58);
    return 0;
}
