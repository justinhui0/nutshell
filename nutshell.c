#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <assert.h>

#include "global.h"
#include "parser.tab.h"

enum CMD { BYE, ERRORS, OK };
struct alias {
    char* alias;
    char* original;
};
int aliasindex = 0;
struct alias ali = {"", ""};
struct alias aliasarr[100];
char HOME[256];

extern char **environ;

// source: https://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c
char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

void initialize() {
    // initialize alias array
    for (int i = 0; i < 100; i++)
    {
        aliasarr[i] = ali;
    }

    // initialize command array
    for (int i = 0; i < 100; i++)
    {
        struct inst j = {""};
        arr[i] = j;
    }

    // store cwd in HOME
    if (getcwd(HOME, sizeof(HOME)) == NULL)
    {
        // error
    }

    // initialize HOME
    if (setenv("HOME", HOME, 1) == -1)
    {
        printf("HOME not initialized\n");
    }

    // initialize PATH
    if (setenv("PATH", ".:/bin", 1) == -1)
    {
        printf("PATH not initialized\n");
    }

    // print initial prompt
    printf("%s> ", HOME);
}

enum CMD getCommand() {
    // init scanner and parser here    
    if (yyparse()) { // parsing error               
        return ERRORS;
    } else {
        return OK;
    }
}
void doit() { 
    // reference: https://stackoverflow.com/questions/19461744/how-to-make-parent-wait-for-all-child-processes-to-finish
    pid_t child_pid, wpid;
    int retStatus;
    if (child_pid = fork() == 0)
    {
        char *PATH = getenv("PATH");
        // parse path and split on : delimiters
        char** pathArr = str_split(PATH, ':');

        char *args[counter + 1];

        for (int i = 0; *(pathArr + i); i++) {
            // first arg = PATH + /executable
            size_t len = strlen(*arr[0].name) + strlen("/") + strlen(*(pathArr + i));
            /* allocate memory for the new string */
            char* str = malloc(len + 1);

            strcpy(str, *(pathArr + i));
            strcat(str, "/");
            strcat(str, *arr[0].name); 

            args[0] = str;

            for (int i = 1; i < counter; i++)
            {
                args[i] = *arr[i].name;
            }

            args[counter] = NULL;

            execv(args[0],args);
        }
        exit(0);
    }
    else if (child_pid == -1)
        perror("fork");
    else {
        waitpid(child_pid, &retStatus, 0);
    }
}

void process_command() {
    char* foundAlias = "";
    for(int i = 0; i < 100; i++) {
        if(!strcmp(*arr[0].name, aliasarr[i].alias)) {
            foundAlias = aliasarr[i].original;
        }
    }
    if(!strcmp(*arr[0].name, "setenv")||!strcmp(foundAlias, "setenv")) {
        if (strcmp(*arr[2].name, "") != 0) { 
            if(setenv(*arr[1].name,*arr[2].name, 1) == -1) {
                printf("setenv error\n");
            }
        }
        else {
            printf("value required\n");
        }
    }
    else if (!strcmp(*arr[0].name, "printenv")||!strcmp(foundAlias, "printenv")) { 
        int i = 1;
        char *s = *environ;
        for (; s; i++) {
            printf("%s\n", s);
            s = *(environ+i);
        }      
    } 
    else if (!strcmp(*arr[0].name, "unsetenv")||!strcmp(foundAlias, "unsetenv")) {
        unsetenv(*arr[1].name);
        //printf("implement %s\n", *arr[0].name);
    }
    else if (!strcmp(*arr[0].name, "cd") || !strcmp(foundAlias, "cd")) {
        chdir(*arr[1].name);
    }
    else if (!strcmp(*arr[0].name, "alias")) {
        if(!strcmp(*arr[1].name , "")) {
            printf("All aliases and their assignments \n");
            for(int i = 0; i < aliasindex; i++) {
                printf("%s %s\n", aliasarr[i].alias, aliasarr[i].original);
            }
        }
        struct alias al = {*arr[1].name, *arr[2].name};
        aliasarr[aliasindex] = al;
        aliasindex = aliasindex+1;
    }
    else if (!strcmp(*arr[0].name, "unalias")) {
        for(int i = 0; i < aliasindex; i++) {
            if(!strcmp(*arr[1].name, aliasarr[i].alias)) {
                aliasarr[i] = ali;
            }
        }
    }
    else if (!strcmp(*arr[0].name, "bye")) {
        exit(1);
    } else {
        doit();
    }
}

int main()
{
    initialize();
    enum CMD type;
    while (1) {
        
        switch (type = getCommand()) {
            case BYE: {
                // bye command issued, exit loop
                return 0;
            }
            case OK: {
                process_command();
                for(int i = 0; i < 100; i++) {
                    struct inst j = {""};
                    arr[i] = j;
                }
                counter = 0;
                printf("%s> ",  getcwd(HOME, 256));
                break;
            }
            case ERRORS: {
                printf("%s\n", "something went wrong");
                // return 0;
            }
        }

    } // printf("%s\n", "something went wrong");

    return 0;

}
