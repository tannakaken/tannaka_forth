#ifndef STACK_H
#define STACK_H

void initialize_stack(void);
void push_stack(char val);
char pop_stack(void);

void pop_and_show(void);
void show_stack(void);

void dup_stack(void);
void swap_stack(void);
void rot_stack(void);

void plus_stack(void);
void sub_stack(void);
void mult_stack(void);
void div_stack(void);

void eq_stack(void);
void neq_stack(void);

#endif
