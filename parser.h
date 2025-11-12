//Definições:
#define STACKSIZE 1024
#define TOKEN_WORDSIZE 6

//Declarações de variáveis:
extern int lookahead;
extern char lexeme[]; //definido no lexer

extern int sp;
extern double stack[STACKSIZE];

extern double acc;
extern char token_string[][TOKEN_WORDSIZE];

extern int line;    //definido no lexer
extern int column;  //definido no lexer

extern FILE *source;  //definido na main
extern FILE *objcode; //definido na main

//Declarações de funções e procedimentos
extern int gettoken(FILE *); //definido no lexer

extern void E(void);
extern void match(int);
extern void print_token(int input, FILE* out);
extern void cmd(void);
// extern void print_lexeme(void);