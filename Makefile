HDIR = .
CFLAGS = -g -I$(HDIR)

# Link-edita os realocáveis
mybc: lexer.o main.o parser.o # automação dos realocáveis para acelerar a criação de mybc
	$(CC) $(CFLAGS) $^ -o $@

lexer.o: 
	$(CC) $(CFLAGS) -c lexer.c -o $@

main.o:
	$(CC) $(CFLAGS) -c main.c -o $@

parser.o:
	$(CC) $(CFLAGS) -c parser.c -o $@

# Usado para depuração de erros em tokens
debug: lexer.o main.o debug_parser.o
	$(CC) $(CFLAGS) $^ -o $@

debug_parser.o:
	$(CC) $(CFLAGS) -DDEBUG -c parser.c -o $@

# Limpa os realocáveis
clean:
	$(RM) *.o

# Limpa os realocáveis, os temporários, o executável e a pasta "extracted", se houver
veryclean: clean
	$(RM) *~ mybc debug debug_parser.o
	rm -rf extracted

# TODO: Atualizar README.md
# TODO: Adicionar data e número do grupo à geração do tar.gz
# Limpa e comprime o projeto para enviar ao professor
compact: veryclean
	tar zcvf mybc.tar.gz Makefile *.[ch]

# Cria uma pasta chamada "extracted", extrai os arquivos do comprimido e o move para a pasta criada
extract:
	mkdir -p extracted
	tar zxvf mybc.tar.gz -C ./extracted
	mv mybc.tar.gz ./extracted