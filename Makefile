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

veryclean: clean
	$(RM) *~ mybc
	rm -rf professor

compact: veryclean # para entregar ao professor
	tar zcvf mybc.tar.gz Makefile *.[ch]

extract: # para extrair do professor
	mkdir -p professor
	tar zxvf mybc.tar.gz -C ./professor
	mv mybc.tar.gz ./professor