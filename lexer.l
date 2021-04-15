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
bool ifEnvironment(char* name) {
    if (getenv(name) == NULL) {
        return false;
    }
    return true;
}
char* parseString(char** arr) {
    char* res = malloc(100 * sizeof(char));
    for (int i = 0; *(arr + i); i++) {
        int len = strlen(*(arr + i));
        if ((*(*(arr + i)) == '$') && (*(*(arr + i) + 1) == '{') && (*(*(arr + i) + len - 1)) == '}') {
            char env[len - 3];
            for (int j = 2; j < len - 1; j++) {
                env[j - 2] = *(*(arr + i) + j);
            }
            printf("%s\n", env);
            if (ifEnvironment(env)) {
                strcat(res, getenv(env));
                strcat(res, " ");
                continue;
            } else {
                printf("env variable does not exist\n");
                break;
            }
        }
        else {
            strcat(res, *(arr + i));
            strcat(res, " ");
        }
    }
    res[strlen(res) - 1] = '\0';
    return res;
}
%}

%x EXPECT_ENVIRONMENT
%x EXPECT_STRING

/* char [*A-Za-z./~&\-] */
char [0-9A-Za-z.|/><&-]
star [*]
QUOTE ["]
NOTQUOTE [^"]
tilde [~]

q [?]
%%

{QUOTE}    					    BEGIN(EXPECT_STRING);
<EXPECT_STRING>{NOTQUOTE}*      { char** arr = str_split(yytext, ' '); yylval.string = strdup(parseString(arr)); return IDENTIFIER; }
<EXPECT_STRING>{QUOTE}          BEGIN(INITIAL);                      


[ \r]+                        { };

[\n]                            return -1;

[$]\{                           BEGIN(EXPECT_ENVIRONMENT);
<EXPECT_ENVIRONMENT>{char}+\}   {   yytext[strlen(yytext) - 1] = '\0';
                                    if (ifEnvironment(yytext)) {
                                        char* yycopy = strdup( getenv(yytext) );
                                        for (int i = strlen( getenv(yytext) ) - 1; i >= 0; --i) {
                                            unput(yycopy[i]);
                                        }
                                        free(yycopy);
                                    }
                                    BEGIN(INITIAL);
                                }

{char}+                         {   if(ifAlias(yytext) && firstWord) {
                                        char *yycopy = strdup( subAliases(yytext) );
                                        for ( int i = strlen(subAliases(yytext)) - 1; i >= 0; --i )
                                            unput( yycopy[i] );
                                        free( yycopy );
                                    } else {
                                        yylval.string = strdup(yytext);
                                        firstWord = false;
                                        return IDENTIFIER;
                                    }
                                }
{star}                          { yylval.string = strdup(yytext); return IDENTIFIER;}
{q}                          { yylval.string = strdup(yytext); return IDENTIFIER;}
[\x1B]+              { yylval.string = strdup("tab"); return TAB;}
[~]             { yylval.string = strdup(yytext); return IDENTIFIER;}

