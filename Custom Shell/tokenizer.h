/**
* @file tokenizer.h
* @author Ghufran Latif
* @date 1 Nov. 2019
* @Header file for tokenizer.c
*/
#include <stddef.h>
#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

char *next_token(char **str_ptr, const char *delim);
int tokenize_args(char *args[], char *line, size_t line_sz, char *line_ptr);


#endif
