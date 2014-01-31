#include <unistd.h>

int main() {
    char *test[2];
    test[0] = "fork";
    test[1] = "\0";
    syscall(59, 1, test);
    return 0;
}
