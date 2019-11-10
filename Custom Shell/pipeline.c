/**
* @file pipeline.c
* @author Ghufran Latif
* @date 1 Nov. 2019
* @Pipeline contains all the functions are used to execute and
 * parse commands. Used to populate the array of command line structs
*/
#include "pipeline.h"

#include <stdio.h>

#include "debug.h"

#include <string.h>

#include <stdlib.h>

#include <stdbool.h>

#include <fcntl.h>

#include <unistd.h>

#include <stddef.h>

#include <sys/wait.h>

/**
 * struct to be used in the execut pipeline function
 */

struct command_line {
    char **tokens;
    bool stdout_pipe;
    char *stdout_file;
    char* cmnd;
};


/**
 * Parses and executes the given arguement array
 * @param char*tokenize_args[] - arguement array
 * @param line_sz - int, length of the argument array
 */

void execute(char *tokenize_args[], int line_sz){
//    printf("-Exec. Func\n");
    struct command_line *cmds;
    cmds = malloc(line_sz * sizeof(struct command_line)); // set the size of cmds array
 
    parseargs(tokenize_args, line_sz, cmds);
    
    execute_pipeline(cmds, 0);

    free(cmds);

}

/**
 * Takes the arguement array and populates an array of command_line structs
 * @param *tokenize_args[] - array of tokenized arguments
 * @param line_sz - size of the tokenized arguments
 * @param cmds - array of command_line structs
 */
void parseargs(char *tokenize_args[], int line_sz, struct command_line *cmds)
{
    int pipe_count = 0;
    int counter = 1;
    
    cmds[pipe_count].tokens = malloc(4096);
    cmds[pipe_count].tokens[0] = tokenize_args[0];
    
    for(int i = 1; i < line_sz; i++){
        if (strcmp(tokenize_args[i], ">")  == 0) { // iterate through the cmds array and add
            cmds[pipe_count].stdout_file = tokenize_args[i+1];
            cmds[pipe_count].stdout_pipe = false;
            tokenize_args[i] = NULL;
            return;
        }
        
        else if(strcmp(tokenize_args[i], "|")  == 0 ){
            cmds[pipe_count].stdout_pipe = true;
            cmds[pipe_count].tokens[counter] = NULL;
            pipe_count++;
            cmds[pipe_count].tokens = malloc(4096);
            *cmds[pipe_count].tokens = tokenize_args[i+1];
    
            tokenize_args[i] = NULL;
            counter = 0;

        }else{
            cmds[pipe_count].tokens[counter] = tokenize_args[i];
            cmds[pipe_count].stdout_pipe = false;
            counter+=1;
        }
    }
    cmds[pipe_count].tokens[counter] = 0;
}

/**
 * Exectues an array of command_line structs recursively
 * @param cmds - of type struct command_line - the array of commands
 * @param index - the inext to keep track within the array
 */

void execute_pipeline(struct command_line *cmds, int index)
{
    /* Creates a pipe. */
    if(!cmds[index].stdout_pipe){
        int fd2 = fileno(stdout);
        if(cmds[index].stdout_file != NULL)
        {
            fd2 = open(cmds[index].stdout_file, O_RDWR | O_CREAT | O_TRUNC, 0644);
            dup2(fd2, fileno(stdout));
        }
        execvp(cmds[index].tokens[0],  cmds[index].tokens);
        close(STDOUT_FILENO);
        close(STDIN_FILENO);

        close(fd2);
        exit(1);
    }

    int fd[2] = { 0 };
    if (pipe(fd) == -1) {
        perror("pipe");
        
    }
    
    pid_t pid = fork();

    if(pid == -1)
    {
        fprintf(stderr, "Fork error\n");
    }
    else if (pid == 0){
        
        /* Child */
        close(fd[0]);// close the read file des

       if (dup2(fd[1], fileno(stdout)) == -1) {
              perror("dup2");
        }

        /* Process what we recieved */
        
        execvp(cmds[index].tokens[0], cmds[index].tokens);

        close(fd[1]);
        close(STDOUT_FILENO);
        close(STDIN_FILENO);
        exit(EXIT_FAILURE);
        
    } else
    {
        /* Parent */
        close(fd[1]); // close the write file des
        if (dup2(fd[0], fileno(stdin)) == -1) {
              perror("dup2");
        }
        index++;
        execute_pipeline(cmds, index);
        close(fd[0]);
    }
}

