/**
 * @file main.h
 * @brief Declara variáveis globais e funções compartilhadas entre os módulos.
 */

// Definições:
#pragma once

// Declarações de variáveis globais consultadas por outros módulos:
extern FILE *source;
extern FILE *objcode;
extern int line;      // Definido no lexer
extern int column;    // Definido no lexer
extern int lookahead; // Definido no parser

// Declarações de funções e procedimentos auxiliares:
extern int main(void);
extern void handle_sigint(int);
extern void mybc(void);      // Definido no parser
extern int gettoken(FILE *); // Definido no lexer
