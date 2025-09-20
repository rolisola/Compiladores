//Definições:
#define ERRTOKEN -0x10000000

//Declarações:
extern int lookahead;
extern char lexeme[]; //definido no lexer
extern int line;      //definido no lexer
extern int column;    //definido no lexer

extern FILE *source;  //definido na main

extern void match(int);

extern int gettoken(FILE *); //definido no lexer

extern void E(void);
extern void T(void);
extern void F(void);
extern void Q(void);
extern void R(void);

