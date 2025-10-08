/*	Grupo 9:
	 André Luis Dias Nogueira,
	 Vitor Marchino Rolisola,
	 Lucas Abdala
*/
#include <stdio.h>
#include <stdlib.h>
#include <main.h>
#include <signal.h>

FILE *source;
FILE *objcode;

//TODO: Testar função
//Tratamento do Sinal do Ctrl+C
void handle_sigint(int sig) {
    ungetc('\n',objcode);
}

//TODO: Criar documentação completa e comentários elucidativos ao código
int main(void) {

	lookahead = gettoken(source = stdin);
	objcode = stdout;

	signal(SIGINT, handle_sigint);

	mybc();

	return 0;
}
