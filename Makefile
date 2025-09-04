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

# Limpa os realocáveis
clean:
	$(RM) *.o

# Limpa os realocáveis, os temporários, o executável e a pasta "professor", se houver
veryclean: clean
	$(RM) *~ mybc
	rm -rf professor

# Limpa e comprime o projeto para enviar ao professor
compact: veryclean
	tar zcvf mybc.tar.gz Makefile *.[ch]

# Cria uma pasta chamada "professor", extrai os arquivos do comprimido e o move para a pasta criada
extract:
	mkdir -p professor
	tar zxvf mybc.tar.gz -C ./professor
	mv mybc.tar.gz ./professor