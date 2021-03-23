%{
#include "parser.tab.h"
%}


DIGIT [0-9]
char [A-Za-z]
QUOTE ["]
NOTQUOTE [^"]
%%



[ \t\r]+ { /* whitespace - do nothing */  }
[\n]+  return -1;
{char}+ { yylval.string = strdup(yytext); return IDENTIFIER;};
{DIGIT}+   return NUMBER;