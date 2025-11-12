// Definições:
#pragma once

// Declarações de variáveis:
extern FILE *source;
extern FILE *objcode;
extern int line; // Definido no lexer
extern int column; // Definido no lexer
extern int lookahead; // Definido no parser

// Declarações de funções e procedimentos:
extern int main(void);
extern void handle_sigint(int);
extern void mybc(void); // Definido no parser
extern int gettoken(FILE *); // Definido no lexer
