/**
* @file history.c
* @author Ghufran Latif
* @date 1 Nov. 2019
* @History
 * Contains all the functions that you use the history array for better encapsulation
*/
#include "history.h"

#include <stdio.h>

#include "debug.h"

#include <string.h>

#include <stdlib.h>

#include <ctype.h>

#include <stdbool.h>

#define HIST_MAX 100



struct history_entry h_ent[HIST_MAX];
int history_count = 0;
int start = 0;
int end = -1;
int tempIndex = 0;


/**
 * Prints the history
 */
void print_history() {
    /* This function should print history entries */
    if(end < start)
    {
        for(int i = start; i < HIST_MAX; i++){
            printf("%d %s" ,(int) h_ent[i].cmd_pos, h_ent[i].cmd_line);
        }
        for(int i = 0; i <= end; i++){
            printf("%d %s" , (int) h_ent[i].cmd_pos, h_ent[i].cmd_line);
        }
          fflush(stdout);
    }
    else
    {
        for(int i = 0; i < history_count; i++){
              printf("%d %s" ,(int) h_ent[i].cmd_pos, h_ent[i].cmd_line);
          }
        LOGP("implemented.\n");
        fflush(stdout);
    }
}


/**
 *Checks if given string is empty
 *@return 1 for empty 0 for not empty
 */
int is_empty(char *s) {
  while (*s != '\0') {
      if (!isspace((unsigned char)*s)){
      return 0;
      }
    s++;
  }
  return 1;
}


/**
 *Reruns the very last command ran
 *@return - gives the last command ran
 */
char* runlastcmnd(){
    // get the most recent cmmnd
    char* last_cmmnd_ran = NULL;
     if(end < start)
        {
          last_cmmnd_ran = h_ent[end-1].cmd_line;
        }else{
            last_cmmnd_ran = h_ent[history_count-1].cmd_line;
        }
    return last_cmmnd_ran;


}

/**
 * Adds entry to array of history entry
 * @param cmd_pos - the id of the command
 * @param line - the line to add to the array
 * @return int - the length of the array
 */
int add_entry(unsigned int cmd_pos, char* line) {
    if (is_empty(line) != 1) {

    char* line_dup = strdup(line);
    
    if(end == HIST_MAX){ // if we are at the end then loop back and increment startm
        end = end % HIST_MAX;
        start++;
    }
    if(history_count < HIST_MAX) { // if History is not full yet
        h_ent[end+1].cmd_pos = cmd_pos;
        h_ent[end+1].cmd_line = line_dup;
        history_count++;
        end++;
        tempIndex++;
    }
    else if (history_count >= HIST_MAX) { // if history is already full
//        start++;//move start
        start = (start+1) % HIST_MAX; // keep under 100
        end = (end+1) % HIST_MAX;
//        end++;
        tempIndex = history_count % HIST_MAX;
        h_ent[end].cmd_pos = cmd_pos;
        h_ent[end].cmd_line = line_dup;
       }
    return history_count;
    }
    return 0;
    
}

/**
 * Checks if str is a number
 * @param s - the string to check
 * @return 0 if false number if true
 */
int isNumeric (const char * s)
{
    if (s == NULL || *s == '\0' || isspace(*s))
      return 0;
    char * p;
    strtod (s, &p);
    return *p == '\0';
}

/**
 * Finds the given parameter by searching the history_entry array for string the starts with given parameter
 * @param arg1 - the string prefix to search for
 * @return - the whole command that was found
 */
char* findbychar (char* arg1){
    // iterate array and find cmmnd number
   if(end < start)
   {
    for(int i = end; i >= 0; i--) { //from the end of the array
       if(startswith(h_ent[i].cmd_line, arg1)){
            return h_ent[i].cmd_line;
       }
    }
    for(int i = HIST_MAX-1; i >= start; i--){
        if(startswith(h_ent[i].cmd_line, arg1)){
                return h_ent[i].cmd_line;
            }
        }
        return NULL;
    }
   else
   {
        for(int i = history_count-1; i >= 0; i--){
              if(startswith(arg1, h_ent[i].cmd_line)){
                  return h_ent[i].cmd_line;
              }
          }
        return NULL;

    }
}


/**
 * Process's the argument array if bang is found checks for either prefix or id
 * @param args[] -the argument array bang is found in
 * @return char - the command found after bang
 */
char* bangFound(char* args[]) {
    //a single bang has been found
    char* arg1 = args[0] + 1;
    char*cmnd;
    
    if(isNumeric(arg1) > 0 ){
        cmnd = findbyid(atoi(arg1));
        return cmnd;
    }
    else{
        cmnd = findbychar(arg1);
        return cmnd;
    }
    return NULL;
}

/**
 * Checks if the given string starts with a given prefix
 * @param pre - the prefix to check
 * @param str - the string to compare with
 * @return bool - true if starts with prefix, other wise false
 */
bool startswith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
    lenstr = strlen(str);
    return lenstr < lenpre ? false : memcmp(pre, str, lenpre) == 0;
}


/**
 * Finds the command given a command number.
 * @param cmndnum - the id to search for
 * @return the command found at given id
 */
char* findbyid(int cmndnum) {
    // iterate array and find cmmnd number
//   int largestPos = largestPosition();
   if(end < start)
    {
    for(int i = end; i >= 0; i--) { //from the end of the array
       if(h_ent[i].cmd_pos == cmndnum){
            return h_ent[i].cmd_line;
       }
    }
    for(int i = HIST_MAX-1; i >= start; i--){
        if(h_ent[i].cmd_pos == cmndnum){

                return h_ent[i].cmd_line;
            }
        }
       
        return NULL;
    }
   else
   {
        for(int i = history_count-1; i >= 0; i--){
              if(h_ent[i].cmd_pos == cmndnum){
                  return h_ent[i].cmd_line;
              }
          }
        return NULL;

    }
}


