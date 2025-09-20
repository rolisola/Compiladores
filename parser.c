#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lexer.h>
#include <parser.h>

#define T_BEGIN do
#define T_END while(is_multsymbol)
#define E_BEGIN do
#define E_END while(is_addsymbol)
#define FACTOR switch(lookahead)

int lookahead; // este é o olho do compilador

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

void print_lexeme() {
	printf("%s ", lexeme);

	// Depuração:
	// printf("->[");
	// print_token(lookahead, stdout);
	// printf("]\n");
}

// E é o símbolo inicial da gramática LL(1) de expressões simplificadas
// E -> T R
void E(void) {

	/*Semantic Action*/int is_negsymbol = 0;/**/
	/*Semantic Action*/int is_addsymbol = 0;/**/
	/*Semantic Action*/int is_multsymbol = 0;/**/

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
					/*Semantic Action*/print_lexeme();/**/
					match('(');
					E();
					/*Semantic Action*/print_lexeme();/**/
					match(')');
					break;

				case DEC:
					/*Semantic Action*/print_lexeme();/**/
					match(DEC); break;

				case OCT:
					/*Semantic Action*/print_lexeme();/**/
					match(OCT); break;

				case HEX:
					/*Semantic Action*/print_lexeme();/**/
					match(HEX); break;

				case FLT:
					/*Semantic Action*/print_lexeme();/**/
					match(FLT); break;

				case ROMAN:
					/*Semantic Action*/print_lexeme();/**/
					match(ROMAN); break;

				default:
					/*Semantic Action*/print_lexeme();/**/
					match(ID);
			}

			/*Semantic Action*/
			if(is_multsymbol) {
				printf("%c ", is_multsymbol);
				is_multsymbol = 0;
			}
			/**/

			if(lookahead == '*' || lookahead == '/') {
				/*Semantic Action*/ is_multsymbol = lookahead;/**/
				match(lookahead);
			}

		} T_END;

		/*Semantic Action*/
		if (is_negsymbol) {
			printf("%c ", is_negsymbol);
			is_negsymbol = 0;
		}
		/**/

		/*Semantic Action*/
		if(is_addsymbol) {
			printf("%c ", is_addsymbol);
			is_addsymbol = 0;
		}
		/**/

		if(lookahead == '+' || lookahead == '-') {
			/*Semantic Action*/is_addsymbol = lookahead;/**/
			match(lookahead);
		}

	} E_END;
}

//////////////////////////// parser components /////////////////////////////////

//TODO: Adicionar contador de linha e coluna | Aprimorar mensagem de erro
void match(int expected) {
	if (lookahead != expected) {
		fprintf(stderr, "Erro de Sintaxe (Ln %d, Cl %d). Esperava-se ", line, (int)(column - strlen(lexeme)));
		print_token(expected, stderr);
		fprintf(stderr,", ao invés de ");
		print_token(lookahead, stderr);
		fprintf(stderr,".\n");
	}
	lookahead = gettoken(source);
}
