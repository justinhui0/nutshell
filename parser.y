%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "global.h"
int yylex();
void addins(char* inst) {
    struct inst i = {inst};
    arr[counter] = i;
    counter++;
}

void yyerror(char* e) {
    printf("Error: %s\n", e);
}
%}

%union {
    char* string;
}

%token <string> IDENTIFIER
%token <string> QUOTE
%token <string> TAB

%nterm <string> idn
//%nterm <statement_list_t*>  value_list


%%

input:
    %empty
    | input idn { };

    
idn:
   QUOTE idn IDENTIFIER QUOTE { }
    | IDENTIFIER {  if(!strcmp($1, "<<EOF>>")) {
                        addins("EOF");
                        }else {addins($1);} };
    | TAB {  addins("TAB"); };
