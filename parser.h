/**
 * @file parser.h
 * @brief Declara o analisador sintático e estruturas de execução.
 */

// Definições:
#pragma once
#define STACKSIZE 1024
#define MAX_ST_ENTRIES 4096
#define TOKEN_WORDSIZE 6

// Declarações de variáveis globais consumidas pelo parser:
extern int lookahead;
extern char token_string[][TOKEN_WORDSIZE];

extern char lexeme[]; // definido no lexer
extern int line;      // definido no lexer
extern int column;    // definido no lexer

extern int sp;
extern double stack[STACKSIZE];

extern double acc;
extern double ans;

extern int address;
extern int symtab_next_entry;

extern char symtab[MAX_ST_ENTRIES][MAXLEN + 1];
extern double vmem[MAX_ST_ENTRIES];

extern int errors;
extern int answers;

extern FILE *source;  // definido na main
extern FILE *objcode; // definido na main

// Declarações de funções e procedimentos
extern int gettoken(FILE *); // definido no lexer

extern void cmd(void);
extern void mybc(void);
extern void expression(void);

extern void match(int);
extern double recall(char const *);
extern void store(char const *);
extern int rmntoi(char *);
