//Definições:
#define ERRTOKEN -0x10000000

//Declarações:
extern int lookahead;

extern FILE *source;  // a ser definido na main

extern void match(int);

extern int gettoken(FILE *); // a ser definido no lexer

extern void E(void);
extern void T(void);
extern void F(void);
extern void Q(void);
extern void R(void);

