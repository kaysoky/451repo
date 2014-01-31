#include "getexeccounts.h" // getExecCounts
#include <stdio.h>  // printf
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <signal.h> // kill

// Print usage and exit
void print_usage()
{
    printf("Usage: ./getDriver pid\n");
    exit(EXIT_FAILURE);
}

// Invalid pid
void print_invalid_pid()
{
    printf("pid is not found\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[])
{
    // Check usage
    if (argc < 2)
        print_usage();

    // Get pid
    int pid = atoi(argv[1]);

    // Check pid
    // For kill(), given option 0, it checks if 
    // the pid valid and does not kill the process
    if (!kill(pid, 0))
        print_invalid_pid();

    // Call libray routine
    int pArray[4];
    if (getExecCounts(pid, pArray) != 0) {
        printf("Error: getExecCounts\n");
        exit(EXIT_FAILURE);
    }

    // print result
    printf("pid %d:\n", pid);
    printf("%5d fork\n", pArray[0]);
    printf("%5d vfork\n", pArray[1]);
    printf("%5d execve\n", pArray[2]);
    printf("%5d clone\n", pArray[3]);

    return EXIT_SUCCESS;
}