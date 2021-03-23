%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.c"
int yylex();
void addins(char* inst) {
    struct inst i = {inst};
    arr[0] = i;
}

void yyerror(char* e) {
    printf("Error: %s\n", e);
}
%}

%union {
    char* string;
}

%token <string> IDENTIFIER 
%token NUMBER
%type <string> idn
//%nterm <statement_list_t*>  value_list


%%


idn:
    IDENTIFIER {  $$ = $1; addins($$); } ;
