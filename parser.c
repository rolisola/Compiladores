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
			#ifndef DEBUG
				putchar('\n');
			#endif

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

	int new_term = 0;
	int new_expression = 0;

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
					/*Ação Semântica 1*/print_lexeme(objcode);/**/
					match('(');
					E();
					/*Ação Semântica 2*/print_lexeme(objcode);/**/
					match(')');
					break;

				case DEC:
					/*Ação Semântica 3*/acc = atoi(lexeme);/**/
					match(DEC); break;

				case OCT:
					/*Ação Semântica 4*/print_lexeme(objcode);/**/
					match(OCT); break;

				case HEX:
					/*Ação Semântica 5*/print_lexeme(objcode);/**/
					match(HEX); break;

				case FLT:
					/*Ação Semântica 6*/acc = atof(lexeme);/**/
					match(FLT); break;

				case ROMAN:
					/*Ação Semântica 7*/print_lexeme(objcode);/**/
					match(ROMAN); break;

				default:
					/*Ação Semântica 8*/print_lexeme(objcode);/**/
					match(ID);
			}

			/*Ação Semântica 9*/
			if(is_multsymbol) {
				if (lookahead == '*') {
					stack[sp] *= acc;
				} else {
					stack[sp] /= acc;
				}
				acc = stack[sp--];
				is_multsymbol = 0;
			}
			/**/

			if(lookahead == '*' || lookahead == '/') {
				/*Ação Semântica 10*/ is_multsymbol = lookahead;/**/
				/*Ação Semântica 11*/ stack[++sp] = acc;/**/
				match(lookahead);
			}

		} T_END;

		/*Ação Semântica 12*/
		if (is_negsymbol) {
			acc = -acc;
			is_negsymbol = 0;
		}
		/**/

		/*Ação Semântica 13*/
		if(is_addsymbol) {
			if (lookahead == '+') {
				stack[sp] += acc;
			} else {
				stack[sp] -= acc;
			}
			acc = stack[sp--];
			is_addsymbol = 0;
		}
		/**/

		if(lookahead == '+' || lookahead == '-') {
			/*Ação Semântica 14*/is_addsymbol = lookahead;/**/
			/*Ação Semântica 15*/ stack[++sp] = acc;/**/
			match(lookahead);
		}

	} E_END;
}

//////////////////////////// parser components /////////////////////////////////

int sp = -1; //Ponteiro da Pilha
double acc = 0; //Pseudo-Registrador
int lookahead = 0; //Vê o próximo token de nosso fluxo
double stack[STACKSIZE]; //Pilha para armazenamento


/* Verifica se o token esperado está em lookahead e lê o próximo token.
 * Parâmetros:	(int) input, (FILE*) out
 * Retorno:		(void)
 */
void match(int expected) {

	//Caso não seja o esperado, imprime um erro.
	if (lookahead != expected) {
		fprintf(stderr, "Erro de Sintaxe (Ln %d, Cl %d). Esperava-se ", line, (int)(column - strlen(lexeme)));

		print_token(expected, stderr);

		fprintf(stderr,", ao invés de ");

		print_token(lookahead, stderr);

		fprintf(stderr,".\n");
	}
	lookahead = gettoken(source);
}

//TODO: Transformar em tabela de tokens e acessar como tokens[lookahead-ID]
/* Imprime o valor do token em um arquivo de saída.
 * Parâmetros:	(int) input, (FILE*) out
 * Retorno:		(void)
 */
void print_token(int input, FILE* out) {
	switch (input) {
		case DEC:
			fprintf(out,"DEC");break;

		case OCT:
			fprintf(out,"OCT");break;

		case HEX:
			fprintf(out,"HEX");break;

		case FLT:
			fprintf(out,"FLT");break;

		case ROMAN:
			fprintf(out,"ROMAN");break;

		case ID:
			fprintf(out,"ID");break;

		default:
			fprintf(out,"%c",input);break;
	}
}

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