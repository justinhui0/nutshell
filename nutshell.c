#include <stdio.h>
#include <string.h>
#include "parser.tab.h"
#include "global.c"

// test
enum CMD { BYE, ERRORS, OK };

enum CMD getCommand() {
    // init scanner and parser here    

    if (yyparse()) { // parsing error
        return ERRORS;
    } else {
        return OK;
    }
}
/*void doit(char* cmd) {

}
void process_command(char* cmd) {
    if(builtin) {
        doit(cmd);
    }    
    else {
        execute();
    }
}*/

int main() {
    enum CMD type;
    while (1) {

       // printf("%s", "Hello, world: ");
        
        switch (type = getCommand()) {
            case BYE: {
                // bye command issued, exit loop
                return 0;
            }
            case OK: {
                //process_command;
                printf("%sBRUH\n", *arr[0].name);
            }
        }
    }
    return 0;
}