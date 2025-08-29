HDIR = .
CFLAGS = -g -I$(HDIR)

mybc: lexer.o main.o parser.o
	$(CC) $^ -o $@

clean:
	$(RM) *.o

mostlyclean: clean
	$(RM) *~

targz: # pra entregar pro prof
	tar zcvf mybc.tar.gz Makefile *.[ch]
# pra extrair:
# 	tar zxvf mybc.tar.gz
