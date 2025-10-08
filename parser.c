#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lexer.h>
#include <parser.h>

//Criação de macros para facilitar a abstração no nosso analisador sintático
#define T_BEGIN do
#define T_END while(is_multsymbol)
#define E_BEGIN do
#define E_END while(is_addsymbol)
#define FACTOR switch(lookahead)

//TODO: Adicionar Comentários e testar função
void mybc(void) {

	cmd();

	while(lookahead != EOF) {
		if (lookahead == '\n' || lookahead == ';') {
			fprintf(objcode,"%lg\n", acc);
			cmd();
		}
	}
	match(EOF);
}

//TODO: Adicionar Comentários e testar função
void cmd(void) {
	switch(lookahead) {

		case EXIT:
		case QUIT:
			exit(0);

		case DEC:
		case FLT:
		case HEX:
		case OCT:
		case ROMAN:
		case ID:
		case '+':
		case '-':
		case '(':
			E();
			putchar('\n');

		default:
			;
	}
}

/* Função inicial do analisador sintático, aprimorada pelo diagrama sintático. 
 * Parâmetros:	(void)
 * Retorno:		(void)
 */
void E(void) {

	/*Ação Semântica*/int is_negsymbol = 0;/**/
	/*Ação Semântica*/int is_addsymbol = 0;/**/
	/*Ação Semântica*/int is_multsymbol = 0;/**/

	if(lookahead == '+' || lookahead == '-') {
		if (lookahead == '-') {
			is_negsymbol = lookahead;
		}
		match(lookahead);
	}

	E_BEGIN {
		T_BEGIN {
			FACTOR {
				case '(':
					match('(');
					E();
					match(')');
					break;

				case DEC:
					/*Ação Semântica 1*/acc = atoi(lexeme);/**/
					match(DEC); break;

				//TODO: Substituir ação semântica pela conversão de octal para double
				case OCT:
					/*Ação Semântica 2*/print_lexeme(objcode);/**/
					match(OCT); break;

				//TODO: Substituir ação semântica pela conversão de hexadecimal para double
				case HEX:
					/*Ação Semântica 3*/print_lexeme(objcode);/**/
					match(HEX); break;

				case FLT:
					/*Ação Semântica 4*/acc = atof(lexeme);/**/
					match(FLT); break;

				//TODO: Substituir ação semântica pela conversão de numero romano para double
				case ROMAN:
					/*Ação Semântica 5*/print_lexeme(objcode);/**/
					match(ROMAN); break;

				//TODO: Adicionar variável na tabela de símbolos e, se tiver algum valor previamente atribuido a ela, colocar em acc
				default:
					/*Ação Semântica 6*/print_lexeme(objcode);/**/
					match(ID);
			}

			/*Ação Semântica 7*/
			if(is_multsymbol) {
				if (is_multsymbol == '*') {
					stack[sp] *= acc;
				} else {
					stack[sp] /= acc;
				}
				acc = stack[sp--];
				is_multsymbol = 0;
			}
			/**/

			if(lookahead == '*' || lookahead == '/') {
				/*Ação Semântica 8*/ is_multsymbol = lookahead;/**/
				/*Ação Semântica 9*/ stack[++sp] = acc;/**/
				match(lookahead);
			}

		} T_END;

		/*Ação Semântica 10*/
		if (is_negsymbol) {
			acc = -acc;
			is_negsymbol = 0;
		}
		/**/

		/*Ação Semântica 11*/
		if(is_addsymbol) {
			if (is_addsymbol == '+') {
				stack[sp] += acc;
			} else {
				stack[sp] -= acc;
			}
			acc = stack[sp--];
			is_addsymbol = 0;
		}
		/**/

		if(lookahead == '+' || lookahead == '-') {
			/*Ação Semântica 12*/is_addsymbol = lookahead;/**/
			/*Ação Semântica 13*/ stack[++sp] = acc;/**/
			match(lookahead);
		}

	} E_END;
}

//////////////////////////// parser components /////////////////////////////////

int sp = -1; //Ponteiro da Pilha
double acc = 0; //Pseudo-Registrador
int lookahead = 0; //Vê o próximo token de nosso fluxo
double stack[STACKSIZE]; //Pilha para armazenamento
char token_string[][TOKEN_WORDSIZE] = {"ID","DEC","OCT","HEX","EE","FLT","ROMAN","EXIT","QUIT"}; //Tabela de conversão de tokens

//TODO: Dar um jeito para que, após o erro, tenha um reset no prompt para voltar à função CMD sem executar tais contas/instruções
/* Verifica se o token esperado está em lookahead e lê o próximo token.
 * Parâmetros:	(int) input, (FILE*) out
 * Retorno:		(void)
 */
void match(int expected) {

	//Caso não seja o esperado, imprime um erro.
	if (lookahead != expected) {
		perror("Erro de Sintaxe (Ln %d, Cl %d). Esperava-se ", line, (int)(column - strlen(lexeme)));
		perror("%s, ao invés de %s.\n", (expected>=ID) ? token_string[expected-ID] : ((char[]){expected,'\0'}), (lookahead>=ID) ? token_string[lookahead-ID] : ((char[]){lookahead,'\0'}));
	}
	lookahead = gettoken(source);
}

//TODO: Remover essa função se não tiver mais utilidade no código
/* Imprime o valor de lexeme no arquivo de saída padrão (stdout).
 * Parâmetros:	(void)
 * Retorno:		(void)
 */
void print_lexeme(FILE* out) {
	fprintf(out, "%s ", lexeme);

	// Depuração de tokens, caso desejado:
	#ifdef DEBUG
		printf("->[");
		print_token(lookahead, out);
		printf("]\n");
	#endif
}