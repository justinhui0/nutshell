%{
#include <stdio.h>

int yylex();

void yyerror(char* e) {
    printf("Error: %s\n", e);
}
%}

%union {
    char *string;
    int num;
}

%token <string> IDENTIFIER 
%token STRING 
%token <num> NUMBER

//%nterm <statement_list_t*>  value_list


%%

start : value 
    | start value 
    | idn 
    | start idn  
    ;

value: 
    NUMBER { printf("%s","num\n");  };

idn:
    IDENTIFIER { printf( "%s", $1);} ;