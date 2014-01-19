#include <stdio.h>
#include <readline/readline.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "queue.h"

// Used to prompt the user for input
const char* SHELL_PROMPT = "CSE451Shell% ";

// Used to split input strings into tokens
const char* STRING_TOKEN_DELIMITERS = " \t";

// Used to recognize the built-in command to exit the shell
const char* BUILT_IN_COMMAND_EXIT = "exit";

// Used to recognize the built-in command to change the working directory
const char* BUILT_IN_COMMAND_CD = "cd";

// The name of the environmental variable that is looked up
// When the "cd" command is used without arguments
const char* DEFAULT_CD_ENVIRO_VAR = "HOME";

// This is either the default exit code (0) 
// or the exit code of the last executed program
int exit_code = 0;

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
    queue_append(token_queue, (queue_element*) token);
    
    // Continue to tokenize until there are no more tokens
    while ((token = strtok(NULL, STRING_TOKEN_DELIMITERS))) {
        queue_append(token_queue, (queue_element*) token);
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
    char *directory;
    
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
        perror();
    }
}

// Treats the first token as a path to an executable
// And executes it, using the subsequent tokens as arguments
static void execute_program(const int num_tokens, const char **tokens) {
    //TODO
}

int main() {
    while (1) {
        // Get a line of input
        // Note: this must be freed before exiting/looping
        char *line = readline(SHELL_PROMPT);
        char **tokens;
        int num_tokens = tokenize(line, &tokens);
        
        // Is there any input?
        if (num_tokens == 0) {
            // Then clean up and loop
            free(tokens);
            free(line);
            continue;
        }
        
        // Interpret the input line by looking at the first token
        if (strcmp(tokens[0], BUILT_IN_COMMAND_EXIT) == 0) {
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
                    perror();
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
            free(tokens);
            free(line);
            exit(exit_code);
        }
        
        // Check for the "cd" command
        if (strcmp(tokens[0], BUILT_IN_COMMAND_CD) == 0) {
            change_directory(num_tokens, tokens);
            
            // Clean up before looping
            free(tokens);
            free(line);
            continue;
        }
        
        // Treat the first argument as a path to an executable
        execute_program(num_tokens, tokens);
        
        // Clean up before looping
        free(tokens);
        free(line);
    }
}
