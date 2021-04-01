%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
int yylex();
void addins(char* inst) {
    struct inst i = {inst};
    arr[counter] = i;
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
%nterm <string> idn
//%nterm <statement_list_t*>  value_list


%%

input:
    %empty
    | input idn { };

    
idn:
    IDENTIFIER "||" IDENTIFIER {addins($1); counter = counter+1;addins("||"); counter = counter+1;addins($3); counter = counter+1;}
    | IDENTIFIER {addins($1); counter = counter+1;};
