HDIR = .
CFLAGS = -g -I$(HDIR)

mybc: lexer.o main.o parser.o # adicao de "<arquivos>.o" para automatizar a criação de mybc
	$(CC) $(CFLAGS) $^ -o $@

lexer.o: 
	$(CC) $(CFLAGS) -c lexer.c -o $@

main.o:
	$(CC) $(CFLAGS) -c main.c -o $@

parser.o:
	$(CC) $(CFLAGS) -c parser.c -o $@

clean:
	$(RM) *.o

mostlyclean: clean
	$(RM) *~

compact: clean # para entregar ao professor $(RM) mybc.tar.gz
	rm -rf professor
	tar zcvf mybc.tar.gz Makefile *.[ch]

extract: # para extrair do professor
	mkdir -p professor
	tar zxvf mybc.tar.gz -C ./professor
	mv mybc.tar.gz ./professor