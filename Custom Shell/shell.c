/**
* @file shell.c
* @author Ghufran Latif
* @date 1 Nov. 2019
* @Initializes the shell and prints the prompts takes care the number of commands passed
*/

#include <fcntl.h>
#include <locale.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>

#include "debug.h"
#include "history.h"
#include "tokenizer.h"
#include "pipeline.h"

#define ARG_MAX 4096

int cmnd = 0;
bool entry_hold = false;
int status;

/**
 * Prints prompt given the command number and shows the current working directory
 * and updates emoji according to the status of the process
 * @param int cmnd - command number
 * @param int status - status of child process
*/

void print_prompt(int cmnd, int status){
    char hostname[HOST_NAME_MAX] = {};
    char username[HOST_NAME_MAX] = {};
    char cwd[PATH_MAX];
    char new_wd[PATH_MAX] = {};
    
    getcwd(cwd, PATH_MAX);
    
    char *next_tok = cwd;
    char *curr_tok;
    while ((curr_tok = next_token(&next_tok, "/")) != NULL) {
        strcat(new_wd, "/");
        strcat(new_wd, curr_tok);
    }
    
    if(startswith("/home", cwd)){
          strcpy(new_wd, "~");
      }
     
    gethostname(hostname, HOST_NAME_MAX);
    strcpy(username, getlogin());

    
    if(status == 0){
        printf("[\U0001F911] - [%d] - [%s@%s: %s] >", cmnd, username, hostname, new_wd);
    }else{
        printf("[\U0001F912] - [%d] - [%s@%s: %s] >", cmnd, username, hostname, new_wd);
    }
    fflush(stdout);
}

/**
 *Handles the cntrl+c signal to make sure it doesn't quit
 *@param signo - the signal number
 */

void sigint_handler(int signo) {
    if(isatty(STDIN_FILENO)){
        printf("\n");
        print_prompt(cmnd,status);
        fflush(stdout);
    }
}


/**
 * Checks for builtin arguments within the command shelll
 * @param char*args[] - array of arguments to check
 * @param int*line_sz - size of the arguments array
 * @return - int
 */
int customargs(char* args[], int *line_sz, size_t line_sz1, char* ptr2, char* ptr3) {
    if(strstr(args[0], "#") != NULL) {
        return 0;
    }

    if(strcmp(args[0], "history") == 0) {
        print_history();
        return 0;
    }
    if(strcmp(args[0], "!!") == 0) {
        char* cmnd1 = runlastcmnd();
        if(cmnd1 != NULL){
            entry_hold = true;
            *line_sz = tokenize_args(args, cmnd1, line_sz1, ptr2);
            return -1;
        }
        return 0;
    }

    if(strstr(args[0], "!") != NULL) {
        char* cmnd1 = bangFound(args);
        if(cmnd1 != NULL){
            entry_hold = true;
            *line_sz = tokenize_args(args, cmnd1, line_sz1, ptr2);
            return -1;
        }
        return 0;

    }
    if(strcmp(args[0], "cd") == 0) {
        if(args[1] != NULL){
            if(chdir(args[1]) != 0){
                perror("CHDIR");
            }
        }
        else{
            struct passwd *pw = getpwuid(getuid());
            const char *home = pw->pw_dir;
            if (chdir(home)!=0)
            {
                perror("chdir");
                
            }
            
        }
        return 0;
    }
    if(strcmp(args[0], "exit") == 0) {
        free(ptr2);
        free(ptr3);
        exit(0);
        return -1;
     }
    if(strcmp(args[0],"setenv") ==0){
        if(args[2] != NULL){
            setenv(args[1], args[2], true);
        }
        return 0;
    }
    return 0;
}


/**
 * Main Function which initializes the shell
 */

int main(void)
{
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigint_handler);

    LOGP("Initializing shell\n");
    char *locale = setlocale(LC_ALL, "en_US.UTF-8");
    LOG("Setting locale: %s\n",
        (locale != NULL) ? locale : "could not set locale!");
    while(true) {
        if (isatty(STDIN_FILENO)) {
            print_prompt(cmnd, status);
        }
        char *line = NULL;
        char *line_ptr = NULL;
        char *args[ARG_MAX];
        char *ptr = NULL;
        size_t line_sz;
        
        if(getline(&line, &line_sz, stdin) == -1) {
            free(line);
            break;
        }

        LOG("read line from stdin: %s", line);
        //TOKENIZE ARGS
        int numCmnds;
        numCmnds = tokenize_args(args, line, line_sz, line_ptr);
        
        //EMPTY ARG CHECK
        if(args[0] == 0)
        {
            continue;
        }
        if(numCmnds == 0)
        {
            continue;
        }
        
        //CHECK IF BUILT-IN
        customargs(args, &numCmnds, line_sz, line_ptr, line);
        free(line_ptr);
        //ADD COMMAND TO HISTORY
        if(!entry_hold){
            add_entry(cmnd++, line);
        }else{
            entry_hold = false;
        }
        
        pid_t child = fork();
        if(child == -1)
        {
            perror("fork");
        }
        else if(child == 0)
        {
            /* Were the child!*/
            execute(args, numCmnds);
            close(STDOUT_FILENO);
            close(STDIN_FILENO);
        }
        else//the parent
        {
            waitpid(child, &status, 0);
            
        }
        free(line);
        free(line_ptr);
        free(ptr);

        if(args[0] != NULL){
            free(args[0]);
        }
        

    }
    return 0;
}



