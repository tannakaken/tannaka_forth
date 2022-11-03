#ifndef MACHINE_H
#define MACHINE_H

#define CODE_END 0
#define CODE_PUSH -1
#define CODE_DUP -2
#define CODE_SWAP -3
#define CODE_ROT -4
#define CODE_PLUS -5
#define CODE_SUB -6
#define CODE_MULT -7
#define CODE_DIV -8
#define CODE_EQ -9
#define CODE_NEQ -10
#define CODE_IF -11
#define CODE_ELSE -12
#define CODE_THEN -13
#define CODE_DO -14
#define CODE_LOOP -15
#define CODE_I -16
#define CODE_CR -17
#define CODE_CALL -18
#define CODE_RET -19
#define CODE_PRINT -20
#define CODE_SHOW -21
#define CODE_EXIT -22

void set_program_counter(int new_count);
int get_program_counter();
void show_program();
void set_code(int code, size_t index);
void initialize_machine(void);
void process_program(void);

#endif
