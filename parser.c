#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lexer.h>
#include <tokens.h>
#include <parser.h>

/*********** Macros ***********/

//Criação de macros para facilitar a abstração no nosso analisador sintático
#define T_BEGIN do
#define T_END while(is_multsymbol)
#define E_BEGIN do
#define E_END while(is_addsymbol)
#define FACTOR switch(lookahead)

////////////////////////////////// Parser ///////////////////////////////////////

/* Função de inicialização da calculadora.
 * Parâmentros: (void)
 * Retorno:		(void)
 */
void mybc(void) {

	cmd();

	//Loop principal do programa, que termina se receber EOF (Ctrl+D) ou por comandos do cmd()
	while (lookahead != EOF) {
		
		//Em caso de termino de instruções, avalia a impressão e a contagem de linhas
		if (lookahead == '\n' || lookahead == ';') {

			//Se não tiver erros, imprime o resultado, salva o ans e altera a quantidade de respostas
			if (!errors) {
				fprintf(objcode,"%lg\n", acc);
				ans = acc;
				answers++;
				errors--;
			}

			//Verifica se pode vir mais instruções para salvar a quantidade de erros e não falhar na contagem de linhas
			if (lookahead == ';' && errors >0) {
				answers += errors;
				errors = -1;
			}
	
			//Caso tenha terminado a linha, checa quantas respostas e erros tiveram para compensar na contagem linhas/colunas
			if (lookahead == '\n' && (errors >0 || answers >0)) {
				errors>0? (line += errors) : 0;
				answers>0? (line += answers) : 0;
				column = 1;
				answers = 0;
			}
		}

		//Avança a leitura do lookahead e trata o comando no cmd()
		match(lookahead);
		cmd();
	}
}

/* Função seletora de comandos da calculadora.
 * Parâmentros: (void)
 * Retorno:		(void)
 */
void cmd(void) {

	switch(lookahead) {

		//Caso de saída do programa
		case EXIT:
		case QUIT:
			exit(0);

		//Caso de inicialização da expressão
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
			errors = 0;
			expression();

		//Caso não venha um argumento válido
		default:
			;
	}
}

/* Expressão do analisador sintático, aprimorada pelo diagrama sintático. 
 * Parâmetros:	(void)
 * Retorno:		(void)
 */
void expression(void) {

	/*Ação Semântica*/int is_negsymbol = 0;/**/
	/*Ação Semântica*/int is_addsymbol = 0;/**/
	/*Ação Semântica*/int is_multsymbol = 0;/**/

	//Aquisição do sinal frontal à expressão
	if(lookahead == '+' || lookahead == '-') {
		if (lookahead == '-') {
			is_negsymbol = lookahead;
		}
		match(lookahead);
	}

	E_BEGIN {
		T_BEGIN {
			FACTOR {

				//Avaliação dos casos possíveis dentro do fator
				case '(':
					match('(');
					expression();
					match(')');
					break;

				case DEC:
					/*Ação Semântica 1*/acc = (double) atoi(lexeme);/**/
					match(DEC); break;

				case OCT:
					/*Ação Semântica 2*/acc = (double) strtol(lexeme,NULL,0);/**/
					match(OCT); break;

				case HEX:
					/*Ação Semântica 3*/acc = (double) strtol(lexeme,NULL,0);/**/
					match(HEX); break;

				case FLT:
					/*Ação Semântica 4*/acc = (double) atof(lexeme);/**/
					match(FLT); break;

				case ROMAN:
					/*Ação Semântica 5*/acc = (double) rmntoi(lexeme);/**/
					match(ROMAN); break;

				case ANS:
					/*Ação Semântica 6*/acc = ans;/**/
					match(ANS); break;

				case '\n':
				case ';':
					break;

				default: {

					//Gera variável para armazenar o ID salvo em lexeme
					char varname[MAXLEN+1];
					/*Ação Semântica 7*/strcpy(varname, lexeme); /**/
					match(ID);

					if (lookahead == ASGN) {
						match(ASGN);
						expression(); //Resgata valor da expressão através do acumulador
						/*Ação Semântica 8*/ store(varname); /**/ //Armazena no endereço associado à variável (varname)
					} else {
						//Caso não esteja atribuindo, usa-se seu valor salvo para o acumulador
						/*Ação Semântica 9*/ acc = recall(varname); /**/
            		}
				} break;
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

//////////////////////////// Componentes do Parser /////////////////////////////////

/*********** Variáveis Globais ***********/

int sp = -1; //Ponteiro da Pilha
double stack[STACKSIZE]; //Pilha de execução para armazenamento

double acc = 0; //Pseudo-Registrador Acumulador
double ans = 0; //Valor Anterior do Acumulador

int lookahead = 0; //Vê o próximo token de nosso fluxo
char token_string[][TOKEN_WORDSIZE] = {"ID","DEC","OCT","HEX","EE","FLT","ROMAN","EXIT","QUIT","ASGN","ANS"}; //Tabela de conversão de tokens

int errors = -1; //Quantidade de Erros
int answers = 0; //Quantidade de Respostas

int address = 0; //Endereço temporário na tabela de símbolos
int symtab_next_entry = 0; //Próximo índice da tabela de símbolos

char symtab[MAX_ST_ENTRIES][MAXLEN + 1]; //Tabela de símbolos
double vmem[MAX_ST_ENTRIES]; //Memória de valores associados às variáveis

/*********** Funções Auxiliares ***********/

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

/* Procura de nome da variável na tabela de símbolos.
 * Parâmetros:	(char *) name
 * Retorno:		(double)
 */
double recall(char const *name) {

    // Busca pela tabela de símbolos
    for (address = symtab_next_entry-1; address > -1; address--) {
        if (strcmp(symtab[address], name) == 0) {
            return vmem[address];
        }
    }

    // Se não encontrou, cria nova entrada
    address = symtab_next_entry++;
    strcpy(symtab[address], name);
    return 0; //Retorna 0 em ponto flutuante
}

/* Armazena valor do acumulador na memória virtual.
 * Parâmetros:	(char *) name
 * Retorno:		(void)
 */
void store(char const * name) {
	recall(name);
	vmem[address] = acc;
}

/* Converter uma string com números romanos em um inteiro.
 * Parâmetros:	(char*) string
 * Retorno:		(int) numero
 */
int rmntoi(char* string) {

	int numero = 0;
	int size = strlen(string);
	int count = 0;

	while (count<size && string[count] == 'M') {
		numero += 1000;
		count++;
	}

	if (count<size && (string[count] == 'C' || string[count] == 'D')) {

		if (string[count] == 'C') {

			if (count+1<size) {
				switch (string[count+1]) {
					case 'M':
						numero += 900;
						count += 2;
						break;
					case 'D':
						numero += 400;
						count += 2;
						break;
					case 'C':
						numero += 100;
						count++;
						if (count+1<size && string[count+1] == 'C') {
							numero += 100;
							count++;
						}
					default:
						numero += 100;
						count++;
				}
			} else {
				numero += 100;
				count++;
			}

		} else {
			numero += 500;
			count++;
			while (count<size && string[count] == 'C') {
				numero += 100;
				count++;
			}
		}
	}

	if (count<size && (string[count] == 'X' || string[count] == 'L')) {

		if (string[count] == 'X') {

			if (count+1<size) {
				switch (string[count+1]) {
					case 'C':
						numero += 90;
						count += 2;
						break;
					case 'L':
						numero += 40;
						count += 2;
						break;
					case 'X':
						numero += 10;
						count++;
						if ((count+1)<size && string[count+1] == 'X') {
							numero += 10;
							count++;
						}
					default:
						numero += 10;
						count++;
				}
			} else {
				numero += 10;
				count++;
			}

		} else {
			numero += 50;
			count++;
			while (count<size && string[count] == 'X') {
				numero += 10;
				count++;
			}
		}
	}

	if (count<size && (string[count] == 'I' || string[count] == 'V')) {

		if (string[count] == 'I') {

			if ((count+1)<size) {
				switch (string[count+1]) {
					case 'X':
						numero += 9;
						count += 2;
						break;
					case 'V':
						numero += 4;
						count += 2;
						break;
					case 'I':
						numero += 1;
						count++;
						if ((count+1)<size && string[count+1] == 'I') {
							numero += 1;
							count++;
						}
					default:
						numero += 1;
						count++;
				}
			} else {
				numero += 1;
				count++;
			}

		} else {
			numero += 5;
			count++;
			while (count<size && string[count] == 'I') {
				numero += 1;
				count++;
			}
		}
	}
	return numero;
}
