#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../counts/getexeccounts.h"

void print_usage() {
    printf("Usage: execcnts <command> <arg> ...\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    // Check usage
    if (argc < 2) {
        print_usage();
    }
    
    int status;
    pid_t pid = fork();

    // Did the fork succeed?
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    if (pid == 0) {
        // Child process
        status = execvp(argv[1], argv + 1);
        
        if (status == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }

        // Should be unreachable
        assert(0);
    }
    
    // Parent process
    pid_t terminated = wait(&status);

    // Did the wait succeed?
    if (terminated == -1) {
        perror("wait");
        exit(EXIT_FAILURE);
    }

    // The process IDs should match
    assert(pid == terminated);

    // Get count for child pid
    int counts[4];
    int res = getExecCounts(pid, counts);

    // Check for failure
    if (res != 0) {
        perror("execcnts");
        exit(EXIT_FAILURE);
    }

    // Print the counts
    printf("pid %d:\n", getpid());
    printf("%5d fork\n", counts[0]);
    printf("%5d vfork\n", counts[1]);
    printf("%5d execve\n", counts[2]);
    printf("%5d clone\n", counts[3]);

    return EXIT_SUCCESS;
}
