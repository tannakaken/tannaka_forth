#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "machine.h"
#include "interpreter.h"

#define CODE_LIMIT 100

static char program[CODE_LIMIT];
static int program_counter;

void set_program_counter(int new_count) {
  program_counter = new_count;
}

int get_program_counter() {
  return program_counter;
}

void set_code(int code, size_t address) {
  if (address >= CODE_LIMIT) {
    fprintf(stderr, "program too long\n");
    exit(1);
  }
  // printf("set code: %d at %zu\n", code, address);
  program[address] = code;
}

void show_program() {
  for (int i = 0; program[i]; ++i) {
    if (program[i] == CODE_PUSH || program[i] == CODE_CALL) {
      printf("%d %d ", program[i], program[i+1]);
      ++i;
    } else {
      printf("%d ", program[i]);
    }
  }
  printf("\n");
}

#define LOOP_LIMIT 100
static char loop_counter_stack[LOOP_LIMIT];
static char loop_end_stack[LOOP_LIMIT];
static size_t loop_pos;

void call_do(void) {
  loop_pos--;
  if (loop_pos < 0) {
    fprintf(stderr, "too deep loop\n");
    exit(1);
  }
  loop_counter_stack[loop_pos] = pop_stack();
  loop_end_stack[loop_pos] = pop_stack();
}

void call_loop(void) {
  loop_counter_stack[loop_pos]++;
  if (loop_counter_stack[loop_pos] >= loop_end_stack[loop_pos]) {
    loop_pos++;
    program_counter += 2;
  } else {
    program_counter = program[program_counter + 1];
  }
}

void initialize_loop(void) {
  loop_pos = sizeof(loop_counter_stack)/sizeof(char) - 1;
}

#define CALL_LIMIT 100
static char call_stack[CALL_LIMIT];
static size_t call_pos;

void initialize_call(void) {
  call_pos = sizeof(call_stack)/sizeof(char) - 1;
}

void push_call_stack(size_t address) {
  if (call_pos < 0) {
    fprintf(stderr, "call stack overflow\n");
    exit(1);
  }
  call_stack[call_pos] = address;
  --call_pos;
}

size_t pop_call_stack(void) {
  if (call_pos >= CALL_LIMIT) {
    fprintf(stderr, "call stack underflow\n");
    exit(1);
  }
  ++call_pos;
  return call_stack[call_pos];
}

void initialize_machine(void) {
  initialize_branch_stack();
  initialize_loop();
  initialize_stack();
  initialize_call();
}

void process_program(void) {
  // show_program();
  while (program[program_counter] != CODE_END) {
    // printf("%d: ", program_counter);
    switch (program[program_counter]) {
      case CODE_PUSH:
        // printf("push %d\n", program[program_counter + 1]);
        push_stack(program[program_counter + 1]);
        program_counter += 2;
        break;
      case CODE_DUP:
        // printf("dup\n");
        dup_stack();
        ++program_counter;
        break;
      case CODE_SWAP:
        // printf("swap\n");
        swap_stack();
        ++program_counter;
        break;
      case CODE_ROT:
        // printf("rot\n");
        rot_stack();
        ++program_counter;
        break;
      case CODE_PLUS:
        // printf("plus\n");
        plus_stack();
        ++program_counter;
        break;
      case CODE_SUB:
        // printf("sub\n");
        sub_stack();
        ++program_counter;
        break;
      case CODE_MULT:
        // printf("mult\n");
        mult_stack();
        ++program_counter;
        break;
      case CODE_DIV:
        // printf("div\n");
        div_stack();
        ++program_counter;
        break;
      case CODE_EQ:
        eq_stack();
        ++program_counter;
        break;
      case CODE_NEQ:
        neq_stack();
        ++program_counter;
        break;
      case CODE_IF: {
        // printf("if %d\n", val);
        char val = pop_stack();
        if (val) {
          program_counter += 2;
        } else {
          program_counter = program[program_counter + 1];
        }
        break;
      }
      case CODE_ELSE:
        // printf("else\n");
        program_counter = program[program_counter + 1];
        break;
      case CODE_THEN:
        // printf("then\n");
        ++program_counter;
        break;
      case CODE_DO:
        // printf("do\n");
        call_do();
        ++program_counter;
        break;
      case CODE_LOOP:
        // printf("loop\n");
        call_loop();
        break;
      case CODE_I:
        // printf("i\n");
        push_stack(loop_counter_stack[loop_pos]);
        ++program_counter;
        break;
      case CODE_CR:
        // printf("cr\n");
        printf("\n");
        ++program_counter;
        break;
      case CODE_CALL:
        // printf("call\n");
        push_call_stack(program_counter);
        program_counter = program[program_counter + 1];
        break;
      case CODE_RET:
        //  printf("return\n");
        program_counter = pop_call_stack();
        program_counter += 2;
        break;
      case CODE_PRINT:
        // printf("print\n");
        pop_and_show();
        ++program_counter;
        break;
      case CODE_SHOW:
        // printf("show\n");
        show_stack();
        ++program_counter;
        break;
      case CODE_EXIT: {
        // printf("exit\n");
        char val = pop_stack();
        exit(val);
      }
    }
  }
}
