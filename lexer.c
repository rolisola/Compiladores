/**
 * @file lexer.c
 * @brief Contém os autômatos responsáveis pela tokenização da calculadora.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <tokens.h>
#include <lexer.h>

// Variáveis globais
char lexeme[MAXLEN + 1];
int line = 1;
int column = 1;

/**
 * @brief Reconhece identificadores no formato Pascal estendido.
 *
 * A expressão regular correspondente é `ID = [A-Za-z][A-Za-z0-9]*`. Palavras
 * reservadas (`exit`, `quit`, `ans`) são convertidas em seus respectivos
 * tokens. Qualquer outra combinação alfanumérica iniciada por letra retorna
 * `ID`.
 *
 * @param tape Fluxo de entrada a ser lido.
 * @return Token identificado ou zero em caso de falha.
 */
int isID(FILE *tape) {

    int i = 0;
    int token = 0;

    // Se o primeiro caractere for uma letra, considera ID
    if (isalpha(lexeme[i] = getc(tape))) {
        i++;
        while (isalnum(lexeme[i] = getc(tape)))
            i++;
        token = ID;
    }

    ungetc(lexeme[i], tape);
    lexeme[i] = 0;

    // Caso seja um comando reservado, retorna o respectivo token
    if (strcmp(lexeme, "exit") == 0)
        token = EXIT;
    if (strcmp(lexeme, "quit") == 0 || strcmp(lexeme, "q") == 0)
        token = QUIT;
    if (strcmp(lexeme, "ans") == 0 || strcmp(lexeme, "ANS") == 0)
        token = ANS;

    return token;
}

/**
 * @brief Avalia se a sequência corrente representa um número decimal.
 *
 * A expressão regular utilizada é `DEC = [1-9][0-9]* | '0'`. Cadeias que se
 * iniciam por zero são tratadas como casos triviais. Em sucesso devolve o token
 * `DEC` e armazena o lexema encontrado.
 *
 * @param tape Fluxo de entrada monitorado.
 * @return `DEC` quando reconhecido, caso contrário zero.
 */
int isDEC(FILE *tape) {

    int i = 0;
    int token = 0;

    if (isdigit(lexeme[i] = getc(tape))) {

        // Caso venha 0, ja retorna o numero
        if (lexeme[i] == '0') {
            lexeme[i + 1] = 0;
            return DEC;
        }
        i++;

        // Lê a sequência de digitos
        while (isdigit(lexeme[i] = getc(tape)))
            i++;
        token = DEC;
    }

    ungetc(lexeme[i], tape);
    lexeme[i] = 0;
    return token;
}

/**
 * @brief Detecta a presença de notação científica em números.
 *
 * A forma reconhecida segue `EE = [eE][+\-]?[0-9]+`. Caso a marcação de expoente
 * não complete a gramática, desfaz leituras excedentes para permitir que outros
 * autômatos processem o trecho.
 *
 * @param tape Fluxo de caracteres compartilhado entre os autômatos.
 * @return `EE` quando válido, zero caso contrário.
 */
int isEE(FILE *tape) {

    int token = 0;
    int hassignal = 0;
    int i = strlen(lexeme);

    if (toupper(lexeme[i] = getc(tape)) == 'E') {
        i++;

        // Lê o sinal e faz uma nova leitura (por ele ser algo opcional)
        if ((lexeme[i] = getc(tape)) == '+' || lexeme[i] == '-') {
            hassignal++;
            i++;
            lexeme[i] = getc(tape);
        }

        // Avalia se temos digitos apos o E
        if (isdigit(lexeme[i])) {
            i++;
            while (isdigit(lexeme[i] = getc(tape)))
                i++;
            token = EE;
        }

        // Caso nao seja EE e tenha vindo sinal, devolve este
        if (hassignal && token != EE) {
            ungetc(lexeme[i], tape);
            i--;
        }
    }

    ungetc(lexeme[i], tape);
    lexeme[i] = 0;
    return token;
}

/**
 * @brief Reconhece números decimais ou em ponto flutuante.
 *
 * Agrupa as gramáticas de inteiros, frações iniciadas por ponto e casas
 * decimais seguidas de notação científica. Promove `DEC` a `FLT` quando um
 * expoente válido é identificado.
 *
 * @param tape Fluxo de entrada.
 * @return `FLT`, `DEC` ou zero conforme o reconhecimento.
 */
int isNUM(FILE *tape) {

    int token = isDEC(tape);
    int i = strlen(lexeme);

    if (token == DEC) {

        // Promoção a float após ter um '.'
        if ((lexeme[i] = getc(tape)) == '.') {
            i++;
            while (isdigit(lexeme[i] = getc(tape)))
                i++;
            token = FLT;
        }
        ungetc(lexeme[i], tape);
        lexeme[i] = 0;
    } else {

        // Caso venha '.', pode ser float (exemplo .14)
        if ((lexeme[i] = getc(tape)) == '.') {
            i++;
            if (isdigit(lexeme[i] = getc(tape))) {
                token = FLT;
                i++;
                while (isdigit(lexeme[i] = getc(tape)))
                    i++;
            } else {
                ungetc(lexeme[i], tape);
                i--;
            }
        }
        ungetc(lexeme[i], tape);
        lexeme[i] = 0;
    }

    // Se for um decimal, e tiver notação exponencial, se torna float
    if (token && isEE(tape))
        token = FLT;

    return token;
}

/**
 * @brief Verifica números escritos em base oito.
 *
 * A gramática aceita `OCT = '0'[0-7]+`. A função consome todos os dígitos
 * octais consecutivos após o prefixo zero.
 *
 * @param tape Fluxo a ser analisado.
 * @return `OCT` em caso de sucesso, zero caso contrário.
 */
int isOCT(FILE *tape) {

    // Caso comece com 0, é candidato a octal
    if ((lexeme[0] = getc(tape)) == '0') {
        if ((lexeme[1] = getc(tape)) >= '0' && lexeme[1] <= '7') {
            int i = 2;
            while ((lexeme[i] = getc(tape)) >= '0' && lexeme[i] <= '7')
                i++;
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

/**
 * @brief Identifica números em base dezesseis precedidos de `0x`.
 *
 * A gramática considerada é `HEX = '0'[Xx][0-9A-Fa-f]+`. Apenas sequências
 * contínuas de caracteres hexadecimais após o prefixo geram o token `HEX`.
 *
 * @param tape Fluxo compartilhado com os demais reconhecedores.
 * @return `HEX` quando a expressão é válida, zero em caso contrário.
 */
int isHEX(FILE *tape) {

    // Se contém 0x, é candidato a hexadecimal
    if ((lexeme[0] = getc(tape)) == '0') {
        if (toupper(lexeme[1] = getc(tape)) == 'X') {
            if (isxdigit(lexeme[2] = getc(tape))) {
                int i = 3;
                while (isxdigit(lexeme[i] = getc(tape)))
                    i++;
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

/**
 * @brief Reconhece numerais romanos clássicos.
 *
 * Implementa a gramática `M{0,3}(CM|CD|D?C{0,3})(XC|XL|L?X{0,3})(IX|IV|V?I{0,3})`,
 * rejeitando cadeias vazias ou seguidas por caracteres alfanuméricos que
 * descaracterizam o token. Em caso de falha, desfaz as leituras para preservar
 * o estado do fluxo.
 *
 * @param tape Fonte de caracteres.
 * @return `ROMAN` quando identificado, zero quando inválido.
 */
int isROMAN(FILE *tape) {

    int i = 0;

    // Milhar (de 0 a 3 M)
    if ((lexeme[i] = getc(tape)) == 'M') {
        i++;
        int j = 0;
        while (lexeme[i] = getc(tape) == 'M' && j < 2) {
            i++;
            j++;
        }
    }
    ungetc(lexeme[i], tape);

    // Centenas (caso tenha C ou D no número)
    if ((lexeme[i] = getc(tape)) == 'D' || lexeme[i] == 'C') {

        if (lexeme[i] == 'C') {
            i++;

            // Resolve casos de CM, CD e CCC
            if ((lexeme[i] = getc(tape)) == 'M' || lexeme[i] == 'D' || lexeme[i] == 'C') {

                if ((lexeme[i + 1] = getc(tape)) == 'C' && lexeme[i] == 'C') {
                    i = i + 2;
                    lexeme[i] = getc(tape);
                } else {
                    i++;
                }
            }

        } else {
            i++;

            // Resolve casos de D até DCCC
            int j = 0;
            while (lexeme[i] = getc(tape) == 'C' && j < 3) {
                i++;
                j++;
            }
        }
    }
    ungetc(lexeme[i], tape);

    // Dezenas (Caso tenha X ou L no número)
    if ((lexeme[i] = getc(tape)) == 'L' || lexeme[i] == 'X') {

        if (lexeme[i] == 'X') {
            i++;

            // Resolve casos de XC, XL e XXX
            if ((lexeme[i] = getc(tape)) == 'C' || lexeme[i] == 'L' || lexeme[i] == 'X') {

                if ((lexeme[i + 1] = getc(tape)) == 'X' && lexeme[i] == 'X') {
                    i = i + 2;
                    lexeme[i] = getc(tape);
                } else {
                    i++;
                }
            }

        } else {
            i++;

            // Resolve casos de L até LXXX
            int j = 0;
            while ((lexeme[i] = getc(tape)) == 'X' && j < 3) {
                i++;
                j++;
            }
        }
    }
    ungetc(lexeme[i], tape);

    // Unidades (Caso tenha I e V no número)
    if ((lexeme[i] = getc(tape)) == 'V' || lexeme[i] == 'I') {

        if (lexeme[i] == 'I') {
            i++;

            // Resolve casos de IX, IV e III
            if ((lexeme[i] = getc(tape)) == 'X' || lexeme[i] == 'V' || lexeme[i] == 'I') {

                if ((lexeme[i + 1] = getc(tape)) == 'I' && lexeme[i] == 'I') {
                    i = i + 2;
                    lexeme[i] = getc(tape);
                } else {
                    i++;
                }
            }

        } else {
            i++;

            // Resolve casos de V até VIII
            int j = 0;
            while ((lexeme[i] = getc(tape)) == 'I' && j < 3) {
                i++;
                j++;
            }
        }
    }
    ungetc(lexeme[i], tape);

    int result = ROMAN;

    // Se tiver um alfanumerico ou digito na frente, significa ID, ou seja, temos que devolver tudo.
    if (isalpha(lexeme[i] = getc(tape)) || isdigit(lexeme[i]) || i == 0) {
        result = 0;
        while (i > 0) {
            ungetc(lexeme[i], tape);
            i--;
        }
    }
    ungetc(lexeme[i], tape);

    lexeme[i] = 0;
    return result;
}

/**
 * @brief Reconhece o operador de atribuição `:=`.
 *
 * Considera apenas a combinação exata dos dois caracteres. Caso o `=` não
 * apareça logo após os dois-pontos, o caractere adicional é devolvido ao fluxo.
 *
 * @param tape Stream analisada.
 * @return `ASGN` quando detectado, zero caso contrário.
 */
int isASGN(FILE *tape) {

    int token = 0;
    int i = 0;

    // Se for :=, é considerado operador de atribuição
    if ((lexeme[i] = getc(tape)) == ':') {
        i++;

        if ((lexeme[i] = getc(tape)) == '=') {
            token = ASGN;
            lexeme[++i] = getc(tape);
        } else {
            ungetc(lexeme[i--], tape);
        }
    }

    ungetc(lexeme[i], tape);
    lexeme[i] = 0;
    return token;
}

/**
 * @brief Avança o cabeçote sobre espaços em branco e sequências de setas.
 *
 * Atualiza contadores globais de linha e coluna, tratando tabulação, retorno
 * de carro e comandos de movimentação enviados pelo terminal. Quebras de linha
 * são mantidas em buffer para permitir ao parser agir sobre elas.
 *
 * @param tape Origem dos caracteres.
 */
void skipspaces(FILE *tape) {
    int head;
    do {
        while (isspace(head = getc(tape))) {

            // Verifica qual é o caractere para atualizar as linhas/colunas de acordo
            switch (head) {
            case '\r':
                column = 1;
                break;

            case '\t':
                column += TAB_SPACE;
                break;
            case '\n':
                column = 0;
                line++;
                break;
            default:
                column++;
            }

            // Permite capturar o caractere '\n' para uso futuro
            if (head == '\n') {
                break;
            }
        }

        // Ignora caracteres das setas, tratando erros de deleções cometidas pelo usuário
        while (head == '\x1B') {
            head = getc(tape);
            if (head == '[') {
                head = getc(tape);
                if (head == 'A' || head == 'B' || head == 'C' || head == 'D') {
                    head = getc(tape);
                }
            }
        }

    } while (isspace(head) && head != '\n');

    ungetc(head, tape);
}

/**
 * @brief Orquestra a execução dos autômatos léxicos e devolve um token.
 *
 * Executa os reconhecedores em cascata, respeitando a ordem de precedência e
 * devolvendo caracteres caso nenhum padrão seja aceito. Atualiza a coluna de
 * acordo com o tamanho do lexema produzido.
 *
 * @param source Stream de caracteres original.
 * @return Código do token reconhecido ou o código ASCII do caractere lido.
 */
int gettoken(FILE *source) {

    int token;

    // Ignora espaços/setas
    skipspaces(source);

    // Averigua, de maneira identada, cada autômato
    if (!(token = isASGN(source))) {
        if (!(token = isROMAN(source))) {
            if (!(token = isID(source))) {
                if (!(token = isHEX(source))) {
                    if (!(token = isOCT(source))) {
                        if (!(token = isNUM(source))) {
                            // Retorno do caractere em ASCII
                            lexeme[0] = (token = getc(source));
                            lexeme[1] = 0;
                        }
                    }
                }
            }
        }
    }

    // Adiciona as colunas refetente ao token
    column += strlen(lexeme);
    return token;
}
