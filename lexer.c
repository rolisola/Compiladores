#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <lexer.h>

char lexeme[MAXLEN+1];

/* Versão extendida de identificador Pascal
 * ID = [A-Za-z][A-Za-z0-9]*
 */
int isID(FILE *tape) {
	int i = 0;
	int token = 0;
	if ( isalpha(lexeme[i] = getc(tape)) ) {
		i++;
		while ( isalnum( lexeme[i] = getc(tape) ) ) i++;
		token = ID;
	}

	ungetc(lexeme[i], tape);
	lexeme[i] = 0;
	return token;
}

/*
 * DEC = [1-9][0-9]* | '0'
 */
int isDEC(FILE *tape) {
	int i = 0;
	int token = 0;
	if ( isdigit(lexeme[i] = getc(tape)) ) {
		if (lexeme[i] == '0') {
			lexeme[i+1] = 0;
			return DEC;
		}
		i++;
		while ( isdigit(lexeme[i] = getc(tape)) ) i++;
		token = DEC;
	}

	ungetc(lexeme[i], tape);
	lexeme[i] = 0;
	return token;
}

/*
 * EXP [eE][+\-]?[0-9]+
 */
// TODO: Testar funcao
int isEE(FILE *tape) {

	int i = strlen(lexeme);
	int hassignal = 0;
	int token = 0;

	if (toupper(lexeme[i] = getc(tape)) == 'E') {
		i++;
		if ( (lexeme[i] = getc(tape)) == '+' || lexeme[i] == '-') {
			hassignal++;
			i++;
			lexeme[i] = getc(tape);
		}

		if ( isdigit(lexeme[i]) ) {
			i++;
			while ( isdigit(lexeme[i] = getc(tape)) ) i++;
			token = EE;
		}

		if (hassignal && token != EE) {
			ungetc(lexeme[i], tape);
			i--;
		}
	}

	ungetc(lexeme[i], tape);
	lexeme[i] = 0;
	return token;
}

/*
 * FIX [0-9]+\.[0-9]*|\.[0-9]+
 * FLT FIX (?:EE) | DEC EE
 */
//TODO: Pegar dos arquivos do professor (juntar ponto fixo/flutuante e decimal)
int isNUM() {

	return 0;
}

/*
 * OCT = '0'[0-7]+
 */
int isOCT(FILE *tape) {
	if ((lexeme[0] = getc(tape)) == '0') {
		if ((lexeme[1] = getc(tape)) >= '0' && lexeme[1] <= '7') {
			int i = 2;
			while ((lexeme[i] = getc(tape)) >= '0' && lexeme[i] <= '7') i++;
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			return OCT;
		}
		ungetc(lexeme[1], tape);
	}
	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}

/*
 * HEX = '0'[Xx][0-9A-Fa-f]+
 */
int isHEX(FILE *tape) {
	if ( (lexeme[0] = getc(tape)) == '0' ) {
		if ( toupper(lexeme[1] = getc(tape)) == 'X' ) {
			if ( isxdigit(lexeme[2] = getc(tape)) ) {
				int i = 3;
				while ( isxdigit(lexeme[i] = getc(tape)) ) i++;
				ungetc(lexeme[i], tape);
				lexeme[i] = 0;
				return HEX;
			}
			ungetc(lexeme[2], tape);
		}
		ungetc(lexeme[1], tape);
	}
	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}

//TODO: Testar e Comentar números romanos
int isROMAN(FILE *tape) {

	int i = 0;
	int exit = 0;

	//Milhar
	if (toupper(lexeme[i] = getc(tape)) == 'M') {
		i++;
		int j = 0;
		while(toupper(lexeme[i] = getc(tape)) == 'M' && j<2) {i++; j++;}
	}
	ungetc(lexeme[i], tape);

	//Centenas
	if (toupper(lexeme[i] = getc(tape)) == 'D' || toupper(lexeme[i]) == 'C') {

		if (toupper(lexeme[i]) == 'C') {
			i++;

			if (toupper(lexeme[i] = getc(tape)) == 'M' || toupper(lexeme[i]) == 'D' || toupper(lexeme[i]) == 'C') {

				if (toupper(lexeme[i]) == 'C' && toupper(lexeme[i+1] = getc(tape)) == 'C') {
					i=i+2;
					lexeme[i] = getc(tape);
				} else {
					i++;
				}
			}

		} else {
			i++;
			int j = 0;
			while(toupper(lexeme[i] = getc(tape)) == 'C' && j<3) {i++; j++;}
		}
	}
	ungetc(lexeme[i], tape);

	//Dezenas
	if (toupper(lexeme[i] = getc(tape)) == 'L' || toupper(lexeme[i]) == 'X') {

		if (toupper(lexeme[i]) == 'X') {
			i++;

			if (toupper(lexeme[i] = getc(tape)) == 'C' || toupper(lexeme[i]) == 'L' || toupper(lexeme[i]) == 'X') {

				if (toupper(lexeme[i]) == 'X' && toupper(lexeme[i+1] = getc(tape)) == 'X') {
					i=i+2;
					lexeme[i] = getc(tape);
				} else {
					i++;
				}
			}
			
		} else {
			i++;
			int j = 0;
			while(toupper(lexeme[i] = getc(tape)) == 'X' && j<3) {i++; j++;}
		}
	}
	ungetc(lexeme[i], tape);

	//Unidades
	if (toupper(lexeme[i] = getc(tape)) == 'V' || toupper(lexeme[i]) == 'I') {

		if (toupper(lexeme[i]) == 'I') {
			i++;

			if (toupper(lexeme[i] = getc(tape)) == 'X' || toupper(lexeme[i]) == 'V' || toupper(lexeme[i]) == 'I') {

				if (toupper(lexeme[i]) == 'I' && toupper(lexeme[i+1] = getc(tape)) == 'I') {
					i=i+2;
					lexeme[i] = getc(tape);
				} else {
					i++;
				}
			}
			
		} else {
			i++;
			int j = 0;
			while(toupper(lexeme[i] = getc(tape)) == 'I' && j<3) {i++; j++;}
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

// Skip spaces
void skipspaces(FILE *tape) {
	int head;
	while ( isspace(head = getc(tape)) );
	ungetc(head, tape);
}

int gettoken(FILE *source) {
	int token;

	skipspaces(source);

	if ( (token = isROMAN(source)) ) return token;
	if ( (token = isID(source)) ) return token;
	if ( (token = isHEX(source)) ) return token;
	if ( (token = isOCT(source)) ) return token;
	if ( (token = isNUM(source)) ) return token;

	// retorna o caractere em ASCII
	token = getc(source);
	return token;
}
