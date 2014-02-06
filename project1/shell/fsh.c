#include <stdio.h>
#include <readline/readline.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include "queue.h"

#define INPUT_LENGTH 80

// Used to prompt the user for input
const char* SHELL_PROMPT = "CSE451Shell% ";

// Used to split input strings into tokens
const char* STRING_TOKEN_DELIMITERS = " \t\n";

// Used to recognize the built-in command to exit the shell
const char* BUILT_IN_COMMAND_EXIT = "exit";

// Used to recognize the built-in command to change the working directory
const char* BUILT_IN_COMMAND_CD = "cd";

// Used to read input file
const char* BUILT_IN_COMMAND_PERIOD = ".";

// The name of the environmental variable that is looked up
// When the "cd" command is used without arguments
const char* DEFAULT_CD_ENVIRO_VAR = "HOME";

// This is either the default exit code (0)
// or the exit code of the last executed program
int exit_code = 0;

// Pointer to the file being read from due to the "." command
FILE *fp = NULL;

// Tokenizes the given line into an array of C-strings ("returned_tokens")
// The returned array will have a NULL entry at the end
// It is the caller's responsibility to free the array
//   Note: Freeing the given "line" will also free the tokens
// Also returns the number of tokens
static int tokenize(char *line, char ***returned_tokens) {
    // This is used to store the tokens during the tokenization
    // It will be empty at the start and end of the method
    queue *token_queue = queue_create();

    // Split the line of input into tokens
    // Note: no memory is allocated by strtok()
    char *token = strtok(line, STRING_TOKEN_DELIMITERS);

    // Continue to tokenize until there are no more tokens
    while (token) {
        queue_append(token_queue, (queue_element*) token);
        token = strtok(NULL, STRING_TOKEN_DELIMITERS);
    }

    // Initialize the return values
    size_t num_tokens = queue_size(token_queue);
    char **tokens = (char **) malloc((num_tokens + 1) * sizeof(char *));
    if (tokens == NULL) {
        // Uh oh, malloc fail!
        exit(EXIT_FAILURE);
    }

    // Convert from queue to array
    for (int i = 0; i < num_tokens; i++) {
        bool res = queue_remove(token_queue, (queue_element**) &tokens[i]);

        // The removal must work
        assert(res);
    }

    // All tokens must have been transferred from queue to array
    assert(queue_is_empty(token_queue));

    // The token array must be NULL-terminated
    tokens[num_tokens] = (char *) NULL;

    // Return the tokens and number of tokens
    queue_destroy(token_queue);
    *returned_tokens = tokens;
    return num_tokens;
}

// Calls the chdir() syscall
// Using either the second token or the user's HOME variable
// If neither are found, nothing happens
static void change_directory(const int num_tokens, const char **tokens) {
    const char *directory;

    // Is there an additional argument?
    if (num_tokens > 1) {
        directory = tokens[1];
    } else {
        // Grab the directory from the user's HOME variable
        directory = getenv(DEFAULT_CD_ENVIRO_VAR);
    }

    // Nothing to do here
    // This happens if no argument is supplied
    // and the environmental variable is not defined
    if (directory == NULL) {
        return;
    }

    // Make a system call to change the working directory
    int res = chdir(directory);

    // Let the user know if the system call fails
    if (res != 0) {
        perror("chdir");
    }
}

// Treats the first token as a path to an executable
// And executes it, using the subsequent tokens as arguments
static void execute_program(const int num_tokens, const char **tokens) {
    // Fork the process
    int status;
    pid_t pid = fork();

    // Did the fork succeed?
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        status = execvp(tokens[0], (char * const*) &tokens[0]);
        if (status == -1) {
            perror(tokens[0]);
            exit(EXIT_FAILURE);
        }

    } else {
        // Parent process
        pid_t terminated = wait(&status);

        // Did the wait succeed?
        if (terminated == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }

        // The process IDs should match
        assert(pid == terminated);

        // Did the child terminated normally?
        if (WIFEXITED(status)) {
            // Save the exit code
            exit_code = WEXITSTATUS(status);
        }
    }
}

// Takes a tokenized input line and runs the associated command
static void do_main(char **tokens, int num_tokens) {
    // Is there any input?
    if (num_tokens == 0) {
        // Nothing to do in this case
        return;
    }

    // Read from file
    if (strcmp(tokens[0], BUILT_IN_COMMAND_PERIOD) == 0) {
        
        if (fp != NULL) {
            printf("Recursive execution from a file is not supported");
            return;
        }
        
        fp = fopen(tokens[1], "r");
        
        // Check if the file opened successfully
        if (fp == NULL) {
            perror("fopen");
        }

    // Interpret the input line by looking at the first token
    } else if (strcmp(tokens[0], BUILT_IN_COMMAND_EXIT) == 0) {
        // Is there an additional argument?
        if (num_tokens > 1) {
            // Try to parse the second argument into a number
            // Ignore the other arguments
            // Heavily based on: http://linux.die.net/man/3/strtol
            char *endptr;
            long val;

            // To distinguish success/failure after the call
            errno = 0;
            val = strtol(tokens[1], &endptr, 10);

            // Is there integer overflow?
            // Or did some other unexpected error occur?
            if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
                    || (errno != 0 && val == 0)) {
                perror("strtol");
                val = exit_code;
            }

            // Was a number given at all?
            if (endptr == tokens[1]) {
                val = exit_code;
            }

            // Successfully parsed a number
            // Or set the value to be the default exit code
            exit_code = val;
        }

        // Exit with the appropriate code
        exit(exit_code);

    // Check for the "cd" command
    } else if (strcmp(tokens[0], BUILT_IN_COMMAND_CD) == 0) {
        change_directory(num_tokens, (const char **) tokens);

        return;

    // Treat the first argument as a path to an executable
    } else {
        execute_program(num_tokens, (const char **) tokens);
    }
}

int main() {
    while (1) { 
        // Input line  
        char *line;

        // Read from file
        if (fp != NULL) {
            line = (char *) malloc(INPUT_LENGTH * sizeof(char));
            
            // Reset file pointer if needed
            if (fgets(line, INPUT_LENGTH, fp) == NULL) {
                // Close the file
                fclose(fp);
                fp = NULL;
                
                // No line was read, so ask the user for input
                free(line);
                continue;
            } 
        } else {
            // Get a line of input
            // Note: this must be freed before exiting/looping
            line = readline(SHELL_PROMPT);
        }

        // Tokenize and execute
        char **tokens;
        int num_tokens = tokenize(line, &tokens);
        do_main(tokens, num_tokens);

        // Clean up before looping
        free(tokens);
        free(line);
    }
}
