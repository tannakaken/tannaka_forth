#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

#define STACK_LENGTH 100

static char stack[STACK_LENGTH];
static int pos;

void initialize_stack(void) {
  pos = sizeof(stack)/sizeof(char) - 1;
}

void push_stack(char val) {
  if (pos < 0) {
    fprintf(stderr, "stack overflow\n");
    exit(1);
  }
  stack[pos] = val;
  pos--;
}

/**
 * @brief (a b c) -> (a a b c)
 */
void dup_stack() {
  char a = pop_stack();
  push_stack(a);
  push_stack(a);
}

/**
 * @brief (a b c) -> (b a c)
 */
void swap_stack() {
  char a = pop_stack();
  char b = pop_stack();
  push_stack(a);
  push_stack(b);
}

/**
 * @brief (a b c) -> (b c a)
 */
void rot_stack() {
  char a = pop_stack();
  char b = pop_stack();
  char c = pop_stack();
  push_stack(a);
  push_stack(c);
  push_stack(b);
}

char pop_stack(void) {
  if (pos >= STACK_LENGTH) {
    fprintf(stderr, "stack underflow\n");
    exit(1);
  }
  pos++;
  return stack[pos];
}

void pop_and_show(void) {
  printf("%d", pop_stack());
}

void show_stack(void) {
  for (int i = pos + 1; i < STACK_LENGTH; ++i) {
    printf("%d ", stack[i]);
  }
  printf("\n");
}

void plus_stack(void) {
  char a, b, c;
  a = pop_stack();
  b = pop_stack();
  c = a + b;
  push_stack(c);
}

void sub_stack(void) {
  char a, b, c;
  a = pop_stack();
  b = pop_stack();
  c = b - a;
  push_stack(c);
}

void mult_stack(void) {
  char a, b, c;
  a = pop_stack();
  b = pop_stack();
  c = a * b;
  push_stack(c);
}

void div_stack(void) {
  char a, b, c;
  a = pop_stack();
  b = pop_stack();
  c = b / a;
  push_stack(c);
}

void eq_stack(void) {
  char a, b;
  a = pop_stack();
  b = pop_stack();
  if (a == b) {
    push_stack(1);
  } else {
    push_stack(0);
  }
}

void neq_stack(void) {
  char a, b;
  a = pop_stack();
  b = pop_stack();
  if (a != b) {
    push_stack(1);
  } else {
    push_stack(0);
  }
}
