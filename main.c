#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "interpreter.h"
#include "machine.h"
#include "stack.h"

void usage(FILE *fp) {
  char *usage_string =
  "Usage: forth [switches] [--] [programfile]\n"
  "-e program        one line of program (several -e's allowed, omit programfile)\n";
  fprintf(fp, "%s", usage_string);
}

int main(int argc, char *argv[]) {
  char *code = NULL;
  bool code_mode = false;
  int		opt;
  while ((opt = getopt(argc, argv, "he:")) != -1) {
    switch (opt) {
      case 'e':
        code_mode = true;
        code = optarg;
        break;
      case 'h':
        usage(stdout);
        exit(0);
      default:
        fprintf(stderr, "Unrecognized switch: -%c  (-h will show valid options).", opt);
        exit(1);
    }
  }

  if (argc == 1) {
    char *line = NULL;
    printf("tannaka forth 0.0.1 <tannakaken@gmail.com> 2022~\n");
    initialize_machine();
    while (1) {
      // show_stack();
      line = readline("> ");
      if (line == NULL) {
        printf("bye\n");
        free(line);
        exit(0);
      }
      add_history(line);
      if (!parse_string(line)) {
        free(line);
        continue;
      }
      process_program();
      fflush(stdout);
      free(line);
    }
  } else if (code_mode) {
    initialize_machine();
    if (!parse_string(optarg)) {
      exit(1);
    }
    process_program();
  } else if (argc == 2) {
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
      fprintf(stderr, "can not open: %s\n", argv[1]);
      exit(1);
    }
    initialize_machine();
    if (!parse_file(fp)) {
      exit(1);
    }
    process_program();
  } else {
    usage(stderr);
    exit(1);
  }
}
