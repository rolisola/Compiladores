/**
 * @file main.c
 * @brief Responsável pela configuração inicial e interação com o terminal.
 */

/*	Grupo 9:
     André Luis Dias Nogueira,
     Vitor Marchino Rolisola,
     Lucas Abdala
*/

#include <stdio.h>
#include <stdlib.h>
#include <main.h>
#include <signal.h>

// Variáveis globais
FILE *source;
FILE *objcode;

/**
 * @brief Trata interrupções enviadas pelo terminal (Ctrl+C).
 *
 * Reinsere uma quebra de linha no fluxo de entrada para manter o parser em
 * estado consistente, além de limpar os caracteres residuais impressos pelo
 * terminal. A contagem de linha/coluna é corrigida para refletir a quebra
 * antecipada.
 *
 * @param sig Código numérico do sinal capturado.
 */
void handle_sigint(int sig) {

    // Retira os caracteres imprimíveis do Ctrl+C do buffer de saída
    fprintf(objcode, "\b \b\b \b\n");

    // Atualiza a contagem de linhas e colunas
    column = 1;
    line++;

    // Adiciona o caractere '\n' no buffer de entrada
    ungetc('\n', source);
}

/**
 * @brief Ponto de entrada da calculadora interativa.
 *
 * Inicializa fluxos de entrada/saída, registra o tratador de sinal e avança o
 * primeiro token para o analisador sintático. A execução segue em laço dentro
 * de `mybc` até o usuário solicitar o término ou enviar EOF.
 */
int main(void) {

    // Configura entrada/saída como padrão
    source = stdin;
    objcode = stdout;

    // Habilita tratamento de sinal ao Ctrl+C
    signal(SIGINT, handle_sigint);

    // Adquire o primeiro token
    lookahead = gettoken(source);

    // Inicializa a calculadora
    mybc();

    return 0;
}
