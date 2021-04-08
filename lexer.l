%{
#include "parser.tab.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "global.h"

char* subAliases(char* name){
    for (int i = 0; i < aliasindex; i++) {
        if(strcmp(aliasarr[i].alias, name) == 0) {
            return aliasarr[i].original;
        }
    }
    return name;
}
bool ifAlias(char* name){
    if (counter != 0) {
        return false;
    }
    for (int i = 0; i < aliasindex; i++) {
        if(strcmp(aliasarr[i].alias, name) == 0) {
            return true;
        }
    }
    return false;
}
%}

%x EXPECT_ENVIRONMENT
%x EXPECT_STRING

/* char [*A-Za-z./~&\-] */
char [A-Za-z./><~&-]
star [*]
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

{char}+                         {   if(ifAlias(yytext)) {
                                        char *yycopy = strdup( subAliases(yytext) );
                                        for ( int i = strlen(subAliases(yytext)) - 1; i >= 0; --i )
                                            unput( yycopy[i] );
                                        free( yycopy );
                                    } else {
                                        yylval.string = strdup(yytext); 
                                        return IDENTIFIER;
                                    }
                                }
{star}                          { yylval.string = strdup(yytext); return IDENTIFIER;}
