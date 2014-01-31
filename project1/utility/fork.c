#include <unistd.h>

int main(int argc, char *argv[]) {
    syscall(57);
    return 0;
}
