# Compiladores

Repositório para a disciplina de Compiladores do Bacharel de Ciências da Computação, turma 2025. Os arquivos neste ambiente são partes da criação de uma calculadora, envolvendo a análise léxica e sintática de sentenças, as levando para formas pós-fixadas.

## O que faz

- Analisa e avalia expressões aritméticas infixas
- Suporta literais: decimal (`42`), octal (`0` seguido de octais, ex.: `077`), hexadecimal (`0xFF`/`0XFF`), ponto flutuante (`3.14`, `.5`, `1e3`, `2.5E-2`) e romano clássico (`MD`, `CDXLIV`, `MMXXV`)
- Operações: soma `+`, subtração `-`, multiplicação `*`, divisão `/`
- Variáveis com atribuição (`:=`) e leitura posterior; símbolo especial `ans` para o último resultado
- Mensagens de erro com linha e coluna; comandos de saída `exit`, `quit` ou `q`

## Como usar

Compilar:
 
```bash
make mybc
```

Executar:
 
```bash
./mybc
```

Sair: `exit`, `quit` ou `q`.


### Exemplos rápidos

```text
2 + 3 * 4;            -> 14
x := 10; y := x + 5;  -> (imprime 10 na primeira, 15 na segunda)
0xFF + 077;           -> 318
3.14e1 + .5;          -> 31.9
MD;                   -> 1500
ans + 1;              -> 1501
```

## Estrutura

- `main.c` – inicializa I/O e laço de leitura
- `lexer.c` – analisador léxico (tokens: `ID`, `DEC`, `OCT`, `HEX`, `FLT`, `ROMAN`, `ASGN`, `ANS`, `EXIT`, `QUIT`)
- `parser.c` – analisador sintático/semântico (parser LL(1), avaliação e armazenamento)
- `tokens.h` – enumeração dos tokens
- `lexer.h`, `parser.h`, `main.h` – interfaces
- `Makefile` – automação de build/limpeza/empacotamento
- `docs/documentacao.pdf` – documentação do projeto (gerada a partir de LaTeX)

## Comandos do Makefile

- `make mybc` – compila o projeto
- `make clean` – remove arquivos objeto (`*.o`)
- `make veryclean` – remove objetos, executável e temporários
- `make compact` – gera um tar.gz datado com os fontes

O `compact` cria um arquivo no formato `mybc_grp9_YYYYMMDD.tar.gz` contendo `Makefile` e os arquivos `*.c`/`*.h` do diretório raiz.

## Notas

- Números romanos seguem a gramática clássica com notação subtrativa (ex.: `CM`, `XL`, `IV`).
- `ans` (ou `ANS`) retorna o último resultado avaliado.
- Erros de sintaxe apontam linha e coluna para facilitar a correção.

## Autores

![Contribuidores](https://contributors-img.web.app/image?repo=rolisola/Compiladores)
