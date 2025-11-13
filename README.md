# Compiladores

Repositório para a disciplina de Compiladores do Bacharel de Ciências da Computação, turma 2025. Os arquivos neste ambiente são partes da criação de uma calculadora, envolvendo a análise léxica e sintática de sentenças, as levando para formas pós-fixadas.

## O que faz

- Analisa expressões aritméticas
- Suporta números em decimal, octal, hexadecimal e romano
- Faz operações básicas: soma, subtração, multiplicação e divisão
- Permite usar variáveis com atribuição (`:=`)
- Mostra erros com linha e coluna quando tem algo errado

## Como usar

Compilar:
```
make mybc
```

Executar:
```
./mybc
```

Para sair: `exit`, `quit` ou `q`

## Arquivos

- `main.c` - inicializa o programa
- `lexer.c` - analisador léxico (identifica tokens)
- `parser.c` - analisador sintático (valida expressões)
- `tokens.h` - definição dos tokens
- `lexer.h`, `parser.h`, `main.h` - headers
- `Makefile` - automação da compilação

## Comandos do Makefile

- `make mybc` - compila o projeto
- `make clean` - remove arquivos objeto
- `make veryclean` - remove tudo (objetos, executável, temporários)
- `make compact` - compacta o projeto em tar.gz com a data

## Observações

No arquivo Makefile, terão automações para a compilação do projeto, bem como a extração e compactação para zips .tar.gz. Erros são marcados com linha e coluna, a fim de facilitar na correção de código. Fora feito o tratamento de setas e o tratamento do comando Ctrl+C a fim de adicionar funcionalidades extras a nossa calculadora.

## Autores

![Contribuidores](https://contributors-img.web.app/image?repo=rolisola/Compiladores)
