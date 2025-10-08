#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <lexer.h>

char lexeme[MAXLEN+1];
int line = 1;
int column = 1;

/* Versão extendida de identificador Pascal.
 * Regex:		ID = [A-Za-z][A-Za-z0-9]*
 * Parâmetros:	(FILE*) tape
 * Retorno:		(int) token
 */
int isID(FILE *tape) {

	int i = 0;
	int token = 0;

	//Se o primeiro caractere for uma letra, considera ID
	if ( isalpha(lexeme[i] = getc(tape)) ) {	
		i++;
		while ( isalnum( lexeme[i] = getc(tape) ) ) i++;
		token = ID;
	}

	ungetc(lexeme[i], tape);
	lexeme[i] = 0;

	//Caso seja um comando reservado, retorna o respectivo token
	if ( strcmp(lexeme,"exit") == 0 ) {
		token = EXIT;
	}

	if ( strcmp(lexeme,"quit") == 0 || strcmp(lexeme,"q") == 0) {
		token = QUIT;
	}

	return token;
}

/* Averigua se o número é decimal.
 * Regex:		DEC = [1-9][0-9]* | '0'
 * Parâmetros:	(FILE*) tape
 * Retorno:		(int) token
 */
int isDEC(FILE *tape) {

	int i = 0;
	int token = 0;

	if ( isdigit(lexeme[i] = getc(tape)) ) {

		//Caso venha 0, ja retorna o numero
		if (lexeme[i] == '0') {
			lexeme[i+1] = 0;
			return DEC;
		}
		i++;

		//Lê a sequência de digitos
		while ( isdigit(lexeme[i] = getc(tape)) ) i++;
		token = DEC;
	}

	ungetc(lexeme[i], tape);
	lexeme[i] = 0;
	return token;
}

/* Averigua se existe a notação exponencial (notação científica).
 * Regex:		EXP [eE][+\-]?[0-9]+
 * Parâmetros:	(FILE*) tape
 * Retorno:		(int) token
 */
int isEE(FILE *tape) {

	int token = 0;
	int hassignal = 0;
	int i = strlen(lexeme);

	if (toupper(lexeme[i] = getc(tape)) == 'E') {
		i++;

		//Lê o sinal e faz uma nova leitura (por ele ser algo opcional)
		if ( (lexeme[i] = getc(tape)) == '+' || lexeme[i] == '-') {
			hassignal++;
			i++;
			lexeme[i] = getc(tape);
		}

		//Avalia se temos digitos apos o E
		if ( isdigit(lexeme[i]) ) {
			i++;
			while ( isdigit(lexeme[i] = getc(tape)) ) i++;
			token = EE;
		}

		//Caso nao seja EE e tenha vindo sinal, devolve este
		if (hassignal && token != EE) {
			ungetc(lexeme[i], tape);
			i--;
		}
	}

	ungetc(lexeme[i], tape);
	lexeme[i] = 0;
	return token;
}

/* Averigua se o número é decimal ou ponto flutuante.
 * Regex:		FLT [0-9]+\.[0-9]*|\.[0-9]+ (?:EE) | DEC EE
 * Parâmetros:	(FILE*) tape
 * Retorno:		(int) token
 */
int isNUM(FILE* tape) {

	int token = isDEC(tape);
	int i = strlen(lexeme);

	if (token == DEC) {
		
		//Promoção a float após ter um '.'
		if ( (lexeme[i] = getc(tape)) == '.' ) {
			i++;
			while ( isdigit( lexeme[i] = getc(tape) ) ) i++;
			token = FLT;
		}
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
	} else {

		//Caso venha '.', pode ser float (exemplo .14)
		if ( (lexeme[i] = getc(tape)) == '.') {
			i++;
			if ( isdigit( lexeme[i] = getc(tape) ) ) {
				token = FLT;
				i++;
				while ( isdigit( lexeme[i] = getc(tape) ) ) i++;
			} else {
				ungetc(lexeme[i], tape);
				i--;
			}
		}
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
	}
	
	//Se for um decimal, e tiver notação exponencial, se torna float
	if (token && isEE(tape))
		token = FLT;

	return token;
}

/* Averigua se o número é octal.
 * Regex:		OCT = '0'[0-7]+
 * Parâmetros:	(FILE*) tape
 * Retorno:		(int) token
 */
int isOCT(FILE *tape) {

	//Caso comece com 0, é candidato a octal
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

/* Averigua se o número é hexadecimal.
 * Regex:		HEX = '0'[Xx][0-9A-Fa-f]+
 * Parâmetros:	(FILE*) tape
 * Retorno:		(int) token
 */
int isHEX(FILE *tape) {

	//Se contém 0x, é candidato a hexadecimal
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

/* Averigua se o número é romano.
 * Regex:		M{0,3}(CM|CD|D?C{0,3})(XC|XL|L?X{0,3})(IX|IV|V?I{0,3})
 * Parâmetros:	(FILE*) tape
 * Retorno:		(int) token
 */
int isROMAN(FILE *tape) {

	int i = 0;
	int exit = 0;

	//Milhar (de 0 a 3 M)
	if (toupper(lexeme[i] = getc(tape)) == 'M') {
		i++;
		int j = 0;
		while(toupper(lexeme[i] = getc(tape)) == 'M' && j<2) {i++; j++;}
	}
	ungetc(lexeme[i], tape);

	//Centenas (caso tenha C ou D no número)
	if (toupper(lexeme[i] = getc(tape)) == 'D' || toupper(lexeme[i]) == 'C') {

		if (toupper(lexeme[i]) == 'C') {
			i++;

			//Resolve casos de CM, CD e CCC
			if (toupper(lexeme[i] = getc(tape)) == 'M' || toupper(lexeme[i]) == 'D' || toupper(lexeme[i]) == 'C') {

				if (toupper(lexeme[i+1] = getc(tape)) == 'C' && toupper(lexeme[i]) == 'C') {
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
			while(toupper(lexeme[i] = getc(tape)) == 'C' && j<3) {i++; j++;}
		}
	}
	ungetc(lexeme[i], tape);

	//Dezenas (Caso tenha X ou L no número)
	if (toupper(lexeme[i] = getc(tape)) == 'L' || toupper(lexeme[i]) == 'X') {

		if (toupper(lexeme[i]) == 'X') {
			i++;

			//Resolve casos de XC, XL e XXX
			if (toupper(lexeme[i] = getc(tape)) == 'C' || toupper(lexeme[i]) == 'L' || toupper(lexeme[i]) == 'X') {

				if (toupper(lexeme[i+1] = getc(tape)) == 'X' && toupper(lexeme[i]) == 'X') {
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
			while(toupper(lexeme[i] = getc(tape)) == 'X' && j<3) {i++; j++;}
		}
	}
	ungetc(lexeme[i], tape);

	//Unidades (Caso tenha I e V no número)
	if (toupper(lexeme[i] = getc(tape)) == 'V' || toupper(lexeme[i]) == 'I') {

		if (toupper(lexeme[i]) == 'I') {
			i++;

			//Resolve casos de IX, IV e III
			if (toupper(lexeme[i] = getc(tape)) == 'X' || toupper(lexeme[i]) == 'V' || toupper(lexeme[i]) == 'I') {

				if (toupper(lexeme[i+1] = getc(tape)) == 'I' && toupper(lexeme[i]) == 'I') {
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

/* Ignora espaços em branco e conta linhas/colunas.
 * Parâmetros:	(FILE*) tape
 * Retorno:		(void)
 */
void skipspaces(FILE *tape) {
	int head;
	while ( isspace(head = getc(tape)) ) {

		switch(head) {
			case'\r':
				column = 1;
				break;

			case'\t':
				column += TAB_SPACE;
				break;
			case'\n':
				column = 1;
				line++; 
				break;
			default:
				column++;
		}

		//Permite capturar o caractere '\n' para uso futuro
		if (head == '\n') {
			break;
		}
	}
	ungetc(head, tape);
}

//TODO: Ignorar setinhas (esquerda e direita), e dar utilidade para setinhas cima e baixo
/* Obtém token possível através de demais autômatos.
 * Parâmetros:	(FILE*) source
 * Retorno:		(int) token
 */
int gettoken(FILE *source) {
	int token;

	skipspaces(source);

	if ( !(token = isROMAN(source))) {
		if ( !(token = isID(source)) ) {
			if ( !(token = isHEX(source)) ) {
				if ( !(token = isOCT(source)) ) {
					if ( !(token = isNUM(source)) ) {
						//Retorno do caractere em ASCII
						lexeme[0] = (token = getc(source));
						lexeme[1] = 0;
					}
				}
			}
		}
	}

	column += strlen(lexeme);
	return token;
}
