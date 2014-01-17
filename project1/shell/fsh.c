#include <stdio.h>
#include <readline/readline.h>
#include <string.h>
#include <assert.h>
#include "queue.h"

// Used to prompt the user for input
const char* SHELL_PROMPT = "CSE451Shell% ";

// Used to split input strings into tokens
const char* STRING_TOKEN_DELIMITERS = " \t";

int main(int argc, char* argv[]) {
    while (1) {
        // Get a line of input
        char *line = readline(SHELL_PROMPT);

        // Split the line of input into tokens
        // Use a queue to make the list of tokens
        queue *token_queue = queue_create();
        char *token = strtok(line, STRING_TOKEN_DELIMITERS);
        queue_append(token_queue, (queue_element*) token);
        
        // Continue to tokenize until there are no more tokens
        while ((token = strtok(NULL, STRING_TOKEN_DELIMITERS))) {
            queue_append(token_queue, (queue_element*) token);
        }
        
        // Convert from queue to array
        size_t num_tokens = queue_size(token_queue);
        char **tokens = (char **) malloc((num_tokens + 1) * sizeof(char *));
        for (int i = 0; i < num_tokens; i++) {
            bool res = queue_remove(token_queue, (queue_element**) &tokens[i]);
            
            // The removal must work
            assert(res);
        }
        
        // The token array must be NULL-terminated
        tokens[num_tokens] = (char *) NULL;
        
        // Interpret the input line by looking at the first token
        //TODO
    }
}
