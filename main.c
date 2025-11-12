/*	Grupo 9:
	 André Luis Dias Nogueira,
	 Vitor Marchino Rolisola,
	 Lucas Abdala
*/
#include <stdio.h>
#include <stdlib.h>
#include <main.h>
#include <signal.h>

//Variáveis globais
FILE *source;
FILE *objcode;

/* Tratamento do sinal do comando Ctrl+C.
 * Parâmentros:	(int) sig
 * Retorno: 	(void)
 */
void handle_sigint(int sig) {

	//Retira os caracteres imprimíveis do Ctrl+C do buffer de saída
	fprintf(objcode,"\b \b\b \b\n");

	//Atualiza a contagem de linhas e colunas
	column = 1;
	line++;

	//Adiciona o caractere '\n' no buffer de entrada
	ungetc('\n',source);
}

int main(void) {

	//Configura entrada/saída como padrão
	source = stdin;
	objcode = stdout;

	//Habilita tratamento de sinal ao Ctrl+C
	signal(SIGINT, handle_sigint);

	//Adquire o primeiro token
	lookahead = gettoken(source);

	//Inicializa a calculadora
	mybc();

	return 0;
}
