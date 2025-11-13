WDIR = .
CFLAGS = -g -I$(WDIR)

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

# Limpa os realocáveis, os temporários, o executável e a pasta "extracted", se houver
veryclean: clean
	$(RM) *~ mybc
	rm -rf extracted

# Limpa e comprime o projeto para enviar ao professor
compact: veryclean
	tar zcvf mybc_grp9_`date "+%Y%m%d"`.tar.gz Makefile *.[ch]

# Cria uma pasta chamada "extracted", extrai os arquivos do comprimido e o move para a pasta criada
extract:
	mkdir -p extracted
	tar zxvf mybc_grp9`date "+%Y%m%d"`.tar.gz -C ./extracted
	mv mybc_grp9`date "+%Y%m%d"`.tar.gz ./extracted