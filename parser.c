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

//TODO: Revisar arquivo .h e ver se itens estão coerentes/comentados
//TODO: Adicionar Comentários e testar função
void mybc(void) {

	cmd();

	while (lookahead != EOF) {
		
		if (lookahead == '\n' || lookahead == ';') {
			if (!errors) {
				fprintf(objcode,"%lg\n", acc);
				answers++;
				errors--; // Deixa o erro como -1 para evitar repetições de respostas
			}
	
			if (lookahead == '\n' && (errors >0 || answers >0)) {
				errors>0? (line += errors) : 0;
				answers>0? (line += answers) : 0;
				column = 1;
				answers = 0;
			}
		}

		match(lookahead);
		cmd();
	}
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
		case ANS:
		case ID:
		case '+':
		case '-':
		case '(':
			E();

		default:
			;
	}
}

//TODO: Adicionar Tabela de Símbolos e Memoria Virtual

/* Expressão do analisador sintático, aprimorada pelo diagrama sintático. 
 * Parâmetros:	(void)
 * Retorno:		(void)
 */
void E(void) {

	/*Ação Semântica*/int is_negsymbol = 0;/**/
	/*Ação Semântica*/int is_addsymbol = 0;/**/
	/*Ação Semântica*/int is_multsymbol = 0;/**/
	/*Ação Semântica*/errors = 0;/**/

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
					/*Ação Semântica 1*/acc = (double) atoi(lexeme);/**/
					match(DEC); break;

				//TODO: Testar conversão
				case OCT:
					/*Ação Semântica 2*/acc = (double) strtol(lexeme,NULL,0);/**/
					match(OCT); break;

				//TODO: Testar conversão
				case HEX:
					/*Ação Semântica 3*/acc = (double) strtol(lexeme,NULL,0);/**/
					match(HEX); break;

				case FLT:
					/*Ação Semântica 4*/acc = (double) atof(lexeme);/**/
					match(FLT); break;

				//TODO: Testar conversão
				case ROMAN:
					/*Ação Semântica 5*/acc = (double) rmntoi(lexeme);/**/
					match(ROMAN); break;

				case ANS:
					/*Ação Semântica 6*/acc = acc;/**/
					match(ANS); break;

				//TODO: Adicionar variável na tabela de símbolos e, se tiver algum valor previamente atribuido a ela, colocar em acc
				default:
					/*Ação Semântica 7*/;/**/
					match(ID);
			}

			//Realização de multiplicação e divisão numéricas
			/*Ação Semântica 8*/
			if(is_multsymbol) {
				
				//Se não houver erros, realiza a operação; caso contrário limpa-se a pilha
				if (!errors) {

					if (is_multsymbol == '*') {
						stack[sp] *= acc;
					} else {
						stack[sp] /= acc;
					}
					acc = stack[sp--];

				} else {

					//Limpeza da pilha
					while (sp>-1) {
						stack[sp--] = 0;
					}
				}
				is_multsymbol = 0;
			}
			/**/

			//Caso venha o sinal de '*' ou '/', continua no termo e adiciona acumulador à pilha
			if((lookahead == '*' || lookahead == '/')) {
				/*Ação Semântica 8*/ is_multsymbol = lookahead;/**/
				/*Ação Semântica 9*/ stack[++sp] = acc;/**/
				match(lookahead);
			}

		} T_END;

		//Realização da negação numérica
		/*Ação Semântica 10*/
		if (is_negsymbol && !errors) {
			acc = -acc;
			is_negsymbol = 0;
		}
		/**/

		//Realização da soma e subtração numéricas
		/*Ação Semântica 11*/
		if(is_addsymbol) {

			//Se não houver erros, realiza a operação; caso contrário limpa-se a pilha
			if (!errors) {

				if (is_addsymbol == '+') {
					stack[sp] += acc;
				} else {
					stack[sp] -= acc;
				}
				acc = stack[sp--];

			} else {

				//Limpeza da pilha
				while (sp>-1) {
					stack[sp--] = 0;
				}
			}
			is_addsymbol = 0;
		}
		/**/

		//Caso venha o sinal de '+' ou '-', continua na expressão e adiciona acumulador à pilha
		if((lookahead == '+' || lookahead == '-')) {
			/*Ação Semântica 12*/is_addsymbol = lookahead;/**/
			/*Ação Semântica 13*/ stack[++sp] = acc;/**/
			match(lookahead);
		}

	} E_END;
}

//////////////////////////// parser components /////////////////////////////////

int sp = -1; //Ponteiro da Pilha
double acc = 0; //Pseudo-Registrador Acumulador
int errors = -1; //Quantidade de Erros
int answers = 0; //Quantidade de Respostas
int lookahead = 0; //Vê o próximo token de nosso fluxo
double stack[STACKSIZE]; //Pilha de execução para armazenamento
char token_string[][TOKEN_WORDSIZE] = {"ID","DEC","OCT","HEX","EE","FLT","ROMAN","EXIT","QUIT"}; //Tabela de conversão de tokens

/* Verifica se o token esperado está em lookahead e lê o próximo token.
 * Parâmetros:	(int) input, (FILE*) out
 * Retorno:		(void)
 */
void match(int expected) {

	//Caso não seja o esperado, imprime um erro.
	if (lookahead == expected) {
		lookahead = gettoken(source);
	} else {

		//Se não for o fim da expressão, imprime o erro de sintaxe e busca o próximo token
		if (lookahead != ';' && lookahead != '\n') {
			fprintf(stderr,"Erro de Sintaxe (Ln %d, Cl %d). Esperava-se ", line, (int)(column - strlen(lexeme)));
			fprintf(stderr,"%s, ao invés de %s.\n", (expected>=ID) ? token_string[expected-ID] : ((char[]){expected,'\0'}), (lookahead>=ID) ? token_string[lookahead-ID] : ((char[]){lookahead,'\0'}));
			lookahead = gettoken(source);
		} else {

			//No caso de ser o fim da expressão, indica a incompletude da instrução aritmética
			fprintf(stderr,"Instrução incompleta.\n");
		}
		errors++;
	}
}

//TODO: Criar conversão para números romanos
/* Converter uma string com números romanos em um inteiro.
 * Parâmetros:	(char*) string
 * Retorno:		(int) numero
 */
int rmntoi(char* string) {

	int numero;

	return numero;
}

/*
int isROMAN(FILE *tape) {

	int i = 0;

	//Milhar (de 0 a 3 M)
	if ((lexeme[i] = getc(tape)) == 'M') {
		i++;
		int j = 0;
		while(lexeme[i] = getc(tape) == 'M' && j<2) {i++; j++;}
	}
	ungetc(lexeme[i], tape);

	//Centenas (caso tenha C ou D no número)
	if ((lexeme[i] = getc(tape)) == 'D' || lexeme[i] == 'C') {

		if (lexeme[i] == 'C') {
			i++;

			//Resolve casos de CM, CD e CCC
			if ((lexeme[i] = getc(tape)) == 'M' || lexeme[i] == 'D' || lexeme[i] == 'C') {

				if ((lexeme[i+1] = getc(tape)) == 'C' && lexeme[i] == 'C') {
					i=i+2;
					lexeme[i] = getc(tape);
				} else {
					i++;
				}
			}

		} else {
			i++;

			//Resolve casos de D até DCCC
			int j = 0;
			while(lexeme[i] = getc(tape) == 'C' && j<3) {i++; j++;}
		}
	}
	ungetc(lexeme[i], tape);

	//Dezenas (Caso tenha X ou L no número)
	if ((lexeme[i] = getc(tape)) == 'L' || lexeme[i] == 'X') {

		if (lexeme[i] == 'X') {
			i++;

			//Resolve casos de XC, XL e XXX
			if ((lexeme[i] = getc(tape)) == 'C' || lexeme[i] == 'L' || lexeme[i] == 'X') {

				if ((lexeme[i+1] = getc(tape)) == 'X' && lexeme[i] == 'X') {
					i=i+2;
					lexeme[i] = getc(tape);
				} else {
					i++;
				}
			}
			
		} else {
			i++;

			//Resolve casos de L até LXXX
			int j = 0;
			while((lexeme[i] = getc(tape)) == 'X' && j<3) {i++; j++;}
		}
	}
	ungetc(lexeme[i], tape);

	//Unidades (Caso tenha I e V no número)
	if ((lexeme[i] = getc(tape)) == 'V' || lexeme[i] == 'I') {

		if (lexeme[i] == 'I') {
			i++;

			//Resolve casos de IX, IV e III
			if ((lexeme[i] = getc(tape)) == 'X' || lexeme[i] == 'V' || lexeme[i] == 'I') {

				if ((lexeme[i+1] = getc(tape)) == 'I' && lexeme[i] == 'I') {
					i=i+2;
					lexeme[i] = getc(tape);
				} else {
					i++;
				}
			}
			
		} else {
			i++;

			//Resolve casos de V até VIII
			int j = 0;
			while((lexeme[i] = getc(tape)) == 'I' && j<3) {i++; j++;}
		}
	}
	ungetc(lexeme[i], tape);

	int result = ROMAN;
	
	//Se tiver um alfanumerico ou digito na frente, significa ID, ou seja, temos que devolver tudo.
	if (isalpha(lexeme[i]=getc(tape)) || isdigit(lexeme[i]) || i==0) {
		result = 0;
		while(i>0) {
			ungetc(lexeme[i], tape);
			i--;
		}
	}
	ungetc(lexeme[i], tape);

	lexeme[i]=0;
	return result;
}
*/
