/**
* @file history.h
* @author Ghufran Latif
* @date 1 Nov. 2019
* @headers file for history.c
*/
#ifndef _HISTORY_H_
#define _HISTORY_H_
#include <stdbool.h>

#define HIST_MAX 100


char* history[HIST_MAX];
/**
 * Histort entry struct used to keep track of command id
 */
struct history_entry {
    unsigned int cmd_pos;
    char* cmd_line;
};

/**
*Finds the last position in the history array and returns
* @return - the last position  in the history array
*/
char* runlastcmnd();
char* bangFound(char* args[]);
char* findbyid (int cmndnum);
void print_history();
char* findbychar(char* arg1);
bool startswith(const char *pre, const char *str);
int add_entry(unsigned int cmd_pos, char* line);
int is_empty(char *s);

#endif
