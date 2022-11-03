#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <stdio.h>
#include <stdbool.h>

void get_token_from_file();
bool end_of_file();
bool parse_file(FILE *fp);
void get_token_from_string();
bool end_of_string();
bool parse_string(char *code);
void initialize_branch_stack(void);
void next_line(void);

#endif
