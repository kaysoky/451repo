#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = syscall(58);
    if (pid == 0) {
        char *test[2];
        test[0] = "fork";
        test[1] = "\0";
        syscall(59, 1, test);
    }
    
    return 0;
}
