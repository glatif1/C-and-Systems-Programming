/**
* @file pipeline.h
* @author Ghufran Latif
* @date 1 Nov. 2019
* @Initializes the pipeline function
*/
#include <stddef.h>
#ifndef _PIPELINE_H_
#define _PIPELINE_H_

struct command_line;
void execute_pipeline(struct command_line *cmds, int index);
void parseargs(char *tokenize_args[], int line_sz, struct command_line *cmds);
void execute(char *tokenize_args[], int line_sz);


#endif

