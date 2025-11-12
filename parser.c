#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <lexer.h>
#include <parser.h>
#include <setjmp.h>

//Criação de macros para facilitar a abstração no nosso analisador sintático
#define T_BEGIN do
#define T_END while(is_multsymbol)
#define E_BEGIN do
#define E_END while(is_addsymbol)
#define FACTOR switch(lookahead)

//Permite que este arquivo use a variável definida no main.c
extern jmp_buf error_recovery_point;

void mybc(void) {

    while (lookahead != EOF) {

        //Tenta processar a linha
        if (setjmp(error_recovery_point) == 0) {
            
            cmd();

            if (lookahead == '\n' || lookahead == ';') {
                fprintf(objcode, "%lg\n", acc);
                match(lookahead);
            }
			else {
                //Se o comando terminou e nao temos um '\n'
                if (lookahead != EOF) {
                    match('\n');
                }
            }
        }
        
        //"catch": O match() deu erro (longjmp)
        else {
            // Limpa o buffer de entrada até o fim da linha
            int c;
            while ((c = fgetc(source)) != '\n' && c != EOF);
            if (c == EOF) break;

            // Prepara o parser para a próxima linha
            lookahead = gettoken(source);
            sp = -1;
            acc = 0;
        }
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


//Função aux para obter o valor de um dígito romano.
int roman_value(char r) {
    r = toupper(r); //Garantir que o caractere seja maiúsculo.
    switch (r) {
        case 'I': return 1;
        case 'V': return 5;
        case 'X': return 10;
        case 'L': return 50;
        case 'C': return 100;
        case 'D': return 500;
        case 'M': return 1000;
        default:  return 0;
    }
}

//Função para converter uma string de num romano em double.
double roman_to_double(char *s) {
    int result = 0;
    int prev_value = 0;

    //Percorre a string de trás para frente.
    for (int i = strlen(s) - 1; i >= 0; i--) {
        int current_value = roman_value(s[i]);

        if (current_value < prev_value) {
            result -= current_value;
        } else {
            result += current_value;
        }
        prev_value = current_value;
    }
    return (double)result;
}

//Estrutura para uma entrada da tabela de símbolos.
typedef struct {
    char *name;
    double value;
} Symbol;

//Tabela de Símbolos
#define MAX_SYMBOLS 100
Symbol symbol_table[MAX_SYMBOLS];
int symbol_count = 0;

// Procura um símbolo na tabela pelo nome.
Symbol* lookup_symbol(char *name) {
    for (int i = 0; i < symbol_count; i++) { 	//Percorre a tabela.
        if (strcmp(symbol_table[i].name, name) == 0) { 
            return &symbol_table[i];
        } 
    }
    return NULL;
}

//Adiciona um novo símbolo à tabela.  
Symbol* add_symbol(char *name) {
    if (symbol_count >= MAX_SYMBOLS) {
        fprintf(stderr, "Erro: Tabela de símbolos cheia!\n");
        exit(1);
    }
    Symbol *new_symbol = &symbol_table[symbol_count++];
    new_symbol->name = strdup(name); 	//strdup() aloca e copia a string
    new_symbol->value = 0.0;
    return new_symbol;
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

				case OCT:
					/*Ação Semântica 2*/acc = (double) strtol(lexeme,NULL,8);/**/
					match(OCT); break;

				case HEX:
					/*Ação Semântica 3*/acc = (double) strtol(lexeme,NULL,16);/**/
					match(HEX); break;

				case FLT:
					/*Ação Semântica 4*/acc = atof(lexeme);/**/
					match(FLT); break;

				case ROMAN:
                    /*Ação Semântica 5*/acc = roman_to_double(lexeme);/**/
                    match(ROMAN); break;

				default: {
                    /*Ação Semântica 6*/
                    Symbol *s = lookup_symbol(lexeme);
                    if (s == NULL) {
                        s = add_symbol(lexeme); 
                    }
                    acc = s->value; //Variável no acumulador
                    /**/
                    
                    match(ID);
                    break;

                }
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

/*
 IMPLEMENTAÇÃO DE RECUPERAÇÃO DE ERRO (setjmp/longjmp)
 Parser agora implementa recuperação de erro usando setjmp/longjmp
 
 -match() (em parser.c): chama longjmp() ("throw") ao detectar um erro de sintaxe.

 -mybc() (em parser.c): usa setjmp() ("catch") para capturar o "throw", limpar o buffer de entrada e resetar o estado.
 
 - Arq: main.c: Apenas define a 'jmp_buf' global.
  */

void match(int expected) {
	if (lookahead == expected) {
			lookahead = gettoken(source);
		} 
	else {
		//Imprime o erro
		fprintf(stderr, "Erro de Sintaxe (Ln %d, Cl %d). ", line, (int)(column - strlen(lexeme)));
		const char* expected_str = (expected >= ID) ? token_string[expected - ID] : (char[]){expected, '\0'};
		const char* lookahead_str = (lookahead >= ID) ? token_string[lookahead - ID] : (char[]){lookahead, '\0'};
		fprintf(stderr, "Espera-se '%s', ao inves de '%s'.\n", expected_str, lookahead_str);

		//Salto de erro; vai pular para 'if(setjmp...)' no main.c
		longjmp(error_recovery_point, 1); 
	}
}

// void print_lexeme(FILE* out) removida