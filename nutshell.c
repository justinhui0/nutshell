#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "global.c"
#include "parser.tab.h"
enum CMD { BYE, ERRORS, OK };

enum CMD getCommand() {
    // init scanner and parser here    

    if (yyparse()) { // parsing error               
        return ERRORS;
    } else {
        return OK;
    }
}
void doit() {
    execlp("ls","ls",NULL);

}

void process_command() {
    
    if(strcmp(*arr[0].name, "ls") == 0) {
        doit();
    }    

}

int main() {
    char HOME[256];

    if (getcwd(HOME, sizeof(HOME)) == NULL) {
        // error
    } 

    enum CMD type;
    while (1) {

       printf("%s> ", HOME);
        
        switch (type = getCommand()) {
            case BYE: {
                // bye command issued, exit loop
                return 0;
            }
            case OK: {
                process_command();
                //break;
                // printf("%s\n", *arr[counter].name);
            }
            case ERRORS: {
                //printf("%s\n", "something went wrong");
                // return 0;
            }
        }
    }                     // printf("%s\n", "something went wrong");
          
    return 0;

}
