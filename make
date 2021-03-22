bison -d PARSER.y

flex LEXER.l 

gcc main.c PARSER.tab.c lex.yy.c -o MAIN.o -lfl