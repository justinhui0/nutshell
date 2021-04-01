%{
#include "parser.tab.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
%}

%x EXPECT_ENVIRONMENT
%x EXPECT_STRING

DIGIT [0-9]
/* char [*A-Za-z./~&\-] */
char [A-Za-z.*/-]
QUOTE ["]
NOTQUOTE [^"]
%%

{QUOTE}						    BEGIN(EXPECT_STRING);
<EXPECT_STRING>{NOTQUOTE}*      { yylval.string = strdup(yytext); return IDENTIFIER; }
<EXPECT_STRING>{QUOTE}		    BEGIN(INITIAL);                         

[ \t\r]+                        { };

[\n]                            return -1;

[$]\{                           BEGIN(EXPECT_ENVIRONMENT);
<EXPECT_ENVIRONMENT>{char}+     { yylval.string = getenv(yytext); return IDENTIFIER; }
<EXPECT_ENVIRONMENT>\}          BEGIN(INITIAL);

{char}+                         { yylval.string = strdup(yytext); return IDENTIFIER; }
{DIGIT}+                        return NUMBER;
