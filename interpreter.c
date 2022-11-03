#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "interpreter.h"
#include "machine.h"

static int line = 0;
void next_line(void) {
  ++line;
}

#define TOKEN_LIMIT 100
static char token[TOKEN_LIMIT];

#define BRANCH_LIMIT 100
static size_t branch_stack[BRANCH_LIMIT];
size_t branch_pos;

void initialize_branch_stack() {
  branch_pos = sizeof(branch_stack)/sizeof(size_t) - 1;
}

void push_branch_stack(size_t address) {
  if (branch_pos < 0) {
    fprintf(stderr, "if stack overflow\n");
    exit(1);
  }
  branch_stack[branch_pos] = address;
  --branch_pos;
}

size_t pop_branch_stack(void) {
  if (branch_pos >= BRANCH_LIMIT) {
    fprintf(stderr, "if stack underflow\n");
    exit(1);
  }
  ++branch_pos;
  return branch_stack[branch_pos];
}

int is_number_string(char *str) {
  for (char *p = str; *p != '\x0'; p++) {
    if (!isdigit(*p)) {
      return 0;
    }
  }
  return 1;
}


#define WORD_HEAP_SIZE 100
static char word_heap[WORD_HEAP_SIZE];
static int word_heap_index;

int register_word_name(char *str) {
  int reg_pos = word_heap_index;
  char *p = str;
  while (*p != '\x0') {
    if (word_heap_index >= WORD_HEAP_SIZE) {
      fprintf(stderr, "word name overflow\n");
      exit(1);
    }
    word_heap[word_heap_index] = *p;
    ++p;
    ++word_heap_index;
  }
  if (word_heap_index >= WORD_HEAP_SIZE) {
    fprintf(stderr, "word name overflow\n");
    exit(1);
  }
  word_heap[word_heap_index] = '\x0';
  ++word_heap_index;
  return reg_pos;
}

typedef struct word_proc_table {
  int word_pointer;
  int proc_pointer;
} WORD_PROC_TABLE;
#define WORD_PROC_SIZE 100

static WORD_PROC_TABLE word_proc[WORD_PROC_SIZE];
static int word_proc_head;
/**
 * @brief ワード名と処理アドレスを関連づける
 *
 * @param[in] name 登録ワード名 
 */ 
void assoc_word_name(char *name) {
  if (word_proc_head >= WORD_PROC_SIZE) {
    fprintf(stderr, "word proc overflow\n");
    exit(1);
  }
  word_proc[word_proc_head].word_pointer = register_word_name(name);
  word_proc[word_proc_head].proc_pointer = get_program_counter();
  ++word_proc_head;
}

int search_word_name(char *name) {
  for (int i = 0; i < word_proc_head; ++i) {
    int word_pointer = word_proc[i].word_pointer;
    if (strcmp(name, &word_heap[word_pointer]) == 0) {
      return word_proc[i].proc_pointer;
    }
  }
  return -1;
}

bool parse(void (*get_token)(), bool (*end)()) {
  static int idx = 0;
  int previous_idx = idx;
  get_token();
  int proc_pointer;
  while (token[0] != '\x0' || !end()) {
    if ((proc_pointer = search_word_name(token)) != -1) {
      set_code(CODE_CALL, idx);
      set_code(proc_pointer, idx+1);
      idx += 2;
    } else if (strcmp(token, "dup") == 0) {
      set_code(CODE_DUP, idx);
      ++idx;
    } else if (strcmp(token, "swap") == 0) {
      set_code(CODE_SWAP, idx);
      ++idx;
    } else if (strcmp(token, "rot") == 0) {
      set_code(CODE_ROT, idx);
      ++idx;
    } else if (strcmp(token, "show") == 0) {
      set_code(CODE_SHOW, idx);
      ++idx;
    } else if (strcmp(token, "+") == 0) {
      set_code(CODE_PLUS, idx);
      ++idx;
    } else if (strcmp(token, "-") == 0) {
      set_code(CODE_SUB, idx);
      ++idx;
    } else if (strcmp(token, "*") == 0) {
      set_code(CODE_MULT, idx);
      ++idx;
    } else if (strcmp(token, "/") == 0) {
      set_code(CODE_DIV, idx);
      ++idx;
    } else if (strcmp(token, "=") == 0) {
      set_code(CODE_EQ, idx);
      ++idx;
    } else if (strcmp(token, "!=") == 0) {
      set_code(CODE_NEQ, idx);
      ++idx;
    } else if (strcmp(token, ".") == 0) {
      set_code(CODE_PRINT, idx);
      ++idx;
    } else if (strcmp(token, "if") == 0) {
      set_code(CODE_IF, idx);
      push_branch_stack(idx + 1);
      idx += 2;
    } else if (strcmp(token, "else") == 0) {
      set_code(CODE_ELSE, idx);
      set_code(idx + 2, pop_branch_stack());
      push_branch_stack(idx + 1);
      idx += 2;
    } else if (strcmp(token, "then") == 0) {
      set_code(CODE_THEN, idx);
      set_code(idx + 1, pop_branch_stack());
      ++idx;
    } else if (strcmp(token, "do") == 0) {
      set_code(CODE_DO, idx);
      push_branch_stack(idx + 1);
      ++idx;
    } else if (strcmp(token, "loop") == 0) {
      set_code(CODE_LOOP, idx);
      set_code(pop_branch_stack(), idx+1);
      idx += 2;
    } else if (strcmp(token, "i") == 0) {
      set_code(CODE_I, idx);
      ++idx;
    } else if (strcmp(token, "cr") == 0) {
      set_code(CODE_CR, idx);
      ++idx;
    } else if (strcmp(token, ":") == 0) {
      get_token();
      assoc_word_name(token);
    } else if (strcmp(token, ";") == 0) {
      set_code(CODE_RET, idx);
      ++idx;
      set_program_counter(idx);
    } else if (strcmp(token, "exit") == 0) {
      set_code(CODE_EXIT, idx);
      ++idx;
    } else if (is_number_string(token)) {
      set_code(CODE_PUSH, idx);
      set_code(atoi(token), idx+1);
      idx += 2;
    } else {
      fprintf(stderr, "[%d] unknown token: %s\n", line, token);
      idx = previous_idx;
      return false;
    }
    get_token();
  }
  set_code(CODE_END, idx);
  return true;
}

static FILE *current_file;

bool parse_file(FILE *fp) {
  current_file = fp;
  return parse(get_token_from_file, end_of_file);
}

bool end_of_file() {
  return feof(current_file);
}

void get_token_from_file() {
  int c;
  int j = 0;
  token[0] = '\x0';
  c = fgetc(current_file);
  while (c == ' ' || c == '\t' || c == '\n') {
    if (c == '\n') {
      next_line();
    }
    c = fgetc(current_file);
  }
  while (c != ' ' && c != '\t' && c != '\n' && !feof(current_file)) {
    token[j] = c;
    j++;
    if (j == TOKEN_LIMIT - 1) {
      fprintf(stderr, "%d: token too long\n", line);
      exit(1);
    }
    c = fgetc(current_file);
  }
  if (c == '\n') {
    next_line();
  }
  token[j] = '\x0';
  // printf("get token: %s\n", token);
}

static char **current_code;

bool end_of_string() {
   return **current_code == '\x0';
}

bool parse_string(char *code) {
  current_code = &code;
  return parse(get_token_from_string, end_of_string);
}


void get_token_from_string() {
  int c;
  int j = 0;
  token[0] = '\x0';
  c = **current_code;
  while (c == ' ' || c == '\t' || c == '\n') {
    ++(*current_code);
    c = **current_code;
  }
  while (c != ' ' && c != '\t' && c != '\n' &&  c != '\x0') {
    token[j] = c;
    j++;
    if (j == TOKEN_LIMIT - 1) {
      fprintf(stderr, "token too long\n");
      exit(1);
    }
    ++(*current_code);
    c = **current_code;
  }
  token[j] = '\x0';
  // printf("get token from string: %s\n", token);
}
