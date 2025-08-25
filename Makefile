HDIR = .
CFLAGS = -g -I$(HDIR)

mybc: lexer.o main.o parser.o
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

targz: # pra entregar pro prof
	tar zcvf mybc.tar.gz Makefile *.[ch]
# pra extrair:
# 	tar zxvf mybc.tar.gz
