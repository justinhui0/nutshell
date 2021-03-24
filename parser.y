%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.c"
int yylex();
int counter =0;
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
%type <string> idn
//%nterm <statement_list_t*>  value_list


%%

command:
    %empty 
    | IDENTIFIER
    
idn:
    IDENTIFIER {  addins($1); counter = counter+1;} ;
