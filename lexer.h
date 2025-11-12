//Definições:
#pragma once
#define MAXLEN 128
#define TAB_SPACE 4

//Declarações de variáveis:
extern char lexeme[MAXLEN+1];
extern int line;
extern int column;

//Declarações de funções e procedimentos:
extern int isID(FILE *);
extern int isDEC(FILE *);
extern int isEE(FILE *);
extern int isNUM(FILE* );
extern int isOCT(FILE *);
extern int isHEX(FILE *);
extern int isROMAN(FILE *);
extern int isASGN(FILE* );

extern void skipspaces(FILE *);
extern int gettoken(FILE *);
