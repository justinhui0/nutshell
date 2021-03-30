CC=/usr/bin/cc

all:  bison-config flex-config nutshell

bison-config:
	bison -d parser.y

flex-config:
	flex lexer.l 

nutshell: 
	gcc nutshell.c parser.tab.c lex.yy.c -o nutshell.o -lfl

clean:
	rm parser.tab.c parser.tab.h lex.yy.c nutshell.o