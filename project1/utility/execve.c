#include <unistd.h>

int main(int argc, char *argv[]) {
    char *argv[2];
    argv[0] = "fork";
    argv[1] = "\0";
    syscall(59, 1, argv);
    return 0;
}
