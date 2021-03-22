#include <stdio.h>
#include <string.h>
#include "parser.tab.h"
#include "global.c"
enum CMD { BYE, ERRORS, OK };

enum CMD getCommand() {
    // init scanner and parser here
    printf("%s", "TEST");
    if (yyparse()) { // parsing error
        printf("%s", "ERROR");
        return ERRORS;
    } else {
        printf("%s", "OK");
        return OK;
    }
}

int main() {
    enum CMD type;
    while (1) {

        printf("%s", "Hello, world: ");
        yyparse();
        
        printf("%s", *arr[0].name);
        /*
        switch (type = getCommand()) {
            case BYE: {
                // bye command issued, exit loop
                return 0;
            }
            case OK: {
                printf("%s\n", "Working ok");
            }
        }*/
    }
    return 0;
}