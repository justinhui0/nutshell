#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>

#include "global.h"
#include "parser.tab.h"

enum CMD { ERRORS, OK };

char HOME[256];

struct alias ali = {"", ""};
extern char **environ;
bool background = false;

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

char *getAlias(char *name)
{
    for (int i = 0; i < aliasindex; i++)
    {
        if (strcmp(aliasarr[i].alias, name) == 0)
        {
            return aliasarr[i].original;
        }
    }
    return name;
}

void initialize() {
    firstWord = true;
    aliasindex = 0;
    pipeCtr = 1;
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
    pid_t child_pid, wpid;
    int retStatus;
    bool success = false;
    if (child_pid = fork() == 0)
    {   // reference: https://stackoverflow.com/questions/19461744/how-to-make-parent-wait-for-all-child-processes-to-finish
        char *PATH = getenv("PATH");
        // parse path and split on : delimiters
        char** pathArr = str_split(PATH, ':');
        
            for (int i = 0; *(pathArr + i); i++)
            {
                int in = 0;
                int out = 0;
                char *args[counter + 1];
                char *args_clean[counter + 1]; // source: https://stackoverflow.com/questions/52939356/redirecting-i-o-in-a-custom-shell-program-written-in-c
                int cleanIndex = 0;
                // first arg = PATH + /executable
                size_t len = strlen(*arr[0].name) + strlen("/") + strlen(*(pathArr + i));
                /* allocate memory for the new string */
                char *str = malloc(len + 1);

                strcpy(str, *(pathArr + i));
                strcat(str, "/");
                strcat(str, *arr[0].name);

                args[0] = str;
                args_clean[cleanIndex++] = str;
                bool expanded = false;
                for (int i = 1; i < counter; i++)
                {
                    args[i] = *arr[i].name;
                }
                args[counter + 1] == NULL;
                
                for (int i = 1; i < counter; i++)
                {
                    if (!strcmp(args[i], "*"))
                    {
                        int toadd = 0;
                        int index = i;
                        expanded = true;
                        DIR *d;
                        struct dirent *dir;
                        d = opendir(".");
                        char *matched[100];
                        if (d)
                        {
                            while ((dir = readdir(d)) != NULL)
                            {
                                if (strstr(dir->d_name, args[index + 1]) != NULL)
                                {
                                    matched[toadd] = dir->d_name;
                                    toadd++;
                                }
                            }
                            closedir(d);
                        }
                        char *arrexpand[index + toadd + 1];
                        arrexpand[0] = str;
                        for (int j = 1; j < index; j++)
                        {
                            arrexpand[j] = *arr[j].name;
                        }
                        int ind = 0;
                        for (int j = index; j < toadd + 1; j++)
                        {
                            arrexpand[j] = matched[ind];
                            ind++;
                        }
                        arrexpand[index + toadd + 1] = NULL;
                        execv(arrexpand[0], arrexpand);
                    }
                    else if (!strcmp(args[i], "?"))
                    {
                        int toadd = 0;
                        int index = i;
                        expanded = true;
                        DIR *d;
                        struct dirent *dir;
                        d = opendir(".");
                        char *matched[100];
                        if (d)
                        {
                            while ((dir = readdir(d)) != NULL)
                            {
                                if (strstr(dir->d_name, args[index + 1]) != NULL && (strlen(dir->d_name) - strlen(args[index + 1]) == 1))
                                {
                                    matched[toadd] = dir->d_name;
                                    toadd++;
                                }
                            }
                            closedir(d);
                        }
                        char *arrexpand[index + toadd + 1];
                        arrexpand[0] = str;
                        for (int j = 1; j < index; j++)
                        {
                            arrexpand[j] = *arr[j].name;
                        }
                        int ind = 0;
                        for (int j = index; j < toadd + 1; j++)
                        {
                            arrexpand[j] = matched[ind];
                            ind++;
                        }
                        arrexpand[index + toadd + 1] = NULL;
                        execv(arrexpand[0], arrexpand);
                    }
                    else if (!strcmp(args[i], "<"))
                    {
                        ++i;
                        if ((in = open(args[i], O_RDONLY)) < 0)
                        {
                            printf("error opening file\n");
                        }
                        dup2(in, STDIN_FILENO);
                        close(in);
                        continue;
                    }
                    else if (!strcmp(args[i], ">"))
                    {
                        ++i;
                        out = creat(args[i], 0644);
                        dup2(out, STDOUT_FILENO);
                        close(out);
                        continue;
                    }
                    else if (!strcmp(args[i], ">>"))
                    {
                        ++i;
                        int append = open(args[i], O_CREAT | O_RDWR | O_APPEND, 0644);
                        dup2(append, STDOUT_FILENO);
                        close(append);
                        continue;
                    }
                    else if (!strcmp(args[i], "<<"))
                    {
                        ++i;
                        int append = open(args[i], O_CREAT | O_RDWR | O_APPEND, 0644);
                        dup2(append, STDIN_FILENO);
                        close(append);
                        continue;
                    }
                    else if (!strcmp(args[i], "|"))
                    {
                        //PIPES
                        printf("pipes");
                        int pd[2];

                        if (pipe(pd) == -1)
                        {
                            printf("Pipe failed");
                        }
                        if (!fork())
                        {
                            dup2(pd[1], 1);
                            execv(args[0], args);
                            abort();
                        }
                        dup2(pd[0], 0);
                        close(pd[1]);
                    }
                    args_clean[cleanIndex++] = args[i];
                }
                if (!expanded)
                {
                    args_clean[cleanIndex] = NULL;
                    execv(args_clean[0], args_clean);
                }
            }

        exit(0);

    }
     if (child_pid == -1)
        perror("fork");
    else { // parent
        if (!background) {
            waitpid(child_pid, &retStatus, 0);
        }     
    }
}

void process_command() {
    if(!strcmp(*arr[0].name, "TAB")) {
        int matched = 0;
        DIR *d;
        struct dirent *dir;
        d = opendir(".");
        if (d) {
            while ((dir = readdir(d)) != NULL) {                               
                if(strstr(dir->d_name, *arr[0].name) != NULL) {
                    matched++; 
                }
            }
            closedir(d);
        }                    
        if(matched == 1) {
            d = opendir(".");
            if (d) {
                while ((dir = readdir(d)) != NULL) {                               
                    if(strstr(dir->d_name, *arr[0].name) != NULL) {
                        *arr[0].name = dir->d_name;printf("%s", dir->d_name); 
                    }
                }
                closedir(d);
            }   
        }
    }
    if(!strcmp(*arr[0].name, "setenv")) {
        if (strcmp(*arr[2].name, "") != 0)
        {
            if (!strcmp(*arr[1].name, "PATH")) {
                char* path = getenv("PATH");
                char** newPaths = str_split(*arr[2].name, ':');

                for (int i = 0; *(newPaths + i); i++) { 
                    char* first = malloc(sizeof(char) * 2);
                    strncpy(first, *(newPaths + i), sizeof(char));
                    strcat(first, "\0");
                    if (!strcmp(first, "~")) {
                        char* expandedPath = getenv("HOME");
                        char* choppedStr = *(newPaths + i) + 1;
                        strcat(expandedPath, choppedStr);
                        strcpy(*(newPaths + i), expandedPath); // replace tilde string with full string
                    }
                    strcat(path, ":");
                    strcat(path, *(newPaths + i));
                }
                setenv("PATH", path, 1);
            }
            else if(setenv(*arr[1].name,*arr[2].name, 1) == -1) {
                printf("setenv error\n");
            }
        }
        else {
            printf("value required\n");
        }
    }
    else if (!strcmp(*arr[0].name, "printenv")) { 
        int i = 1;
        char *s = *environ;
        for (; s; i++) {
            printf("%s\n", s);
            s = *(environ+i);
        }      
    } 
    else if (!strcmp(*arr[0].name, "unsetenv")) {
        unsetenv(*arr[1].name);
        if (!strcmp(*arr[1].name, "PATH")) {
            setenv("PATH", ".", 1);
        }
        else if (!strcmp(*arr[1].name, "HOME")) {
            setenv("HOME", "NULL", 1);
        }
        //printf("implement %s\n", *arr[0].name);
    }
    else if (!strcmp(*arr[0].name, "cd"))
    {
        int i = chdir(*arr[1].name);
        if(i != 0) {
            printf("%s Directory not found\n", *arr[1].name );
        }        
    }
    else if (!strcmp(*arr[0].name, "alias")) {
        if(!strcmp(*arr[1].name , "")) { // alias with no args: print all aliases
            for(int i = 0; i < aliasindex; i++) {
                printf("alias %s=\'%s\'\n", aliasarr[i].alias, aliasarr[i].original);
            }
            return;
        }
        if (!strcmp(*arr[1].name, *arr[2].name)) { // name == word
            printf("Error, expansion of \"%s\" would create a loop.\n", *arr[1].name);
            return;
        }
        if (aliasindex > 0) {
            char *resToCheck = (char *) malloc(100);
            strcpy(resToCheck, *arr[1].name);
            char *curr = (char *) malloc(100);
            strcpy(curr, *arr[2].name);
            char *expanded = (char *)malloc(100);

            while (true)
            {
                strcpy(expanded, getAlias(curr));
                if (!strcmp(expanded, curr))
                { // did not change, exit loop
                    break;
                }
                else if (!strcmp(expanded, resToCheck))
                { // made it back to start, infinite loop
                    printf("infinite loop detected!!\n");
                    return;
                }
                strcpy(curr, expanded);
            }
        }

        for (int i = 0; i < aliasindex; i++)
        {
            if(!strcmp(aliasarr[i].alias, *arr[1].name)) { // alias already exists, overwrite it
                strcpy(aliasarr[i].original, *arr[2].name);
                return;
            }
        }
        // not found, we need to add a new entry.
        struct alias al = {*arr[1].name, *arr[2].name};
        aliasarr[aliasindex] = al;
        aliasindex = aliasindex+1; 
    }
    else if (!strcmp(*arr[0].name, "unalias")) {
        int i = 0;
        int found = 0;
        for(i; i < aliasindex; i++) {
            if(!strcmp(*arr[1].name, aliasarr[i].alias)) {
                aliasindex--;
                found = 1;
                break;
            }
        }
        // shift over to close empty spot
        if (found) {
            for (i; i < aliasindex; i++) {
                aliasarr[i] = aliasarr[i + 1];
            }
            aliasarr[++i] = ali;
        }
        else {
            printf("%s Alias not found\n", aliasarr[i].alias);
        }
    }
    else if (!strcmp(*arr[0].name, "bye")) {
        exit(1);
    } 
       
    if (!strcmp(*arr[counter - 1].name, "&")) {
        counter--;
        background = true;
    }
    doit();
    

}

int main()
{
    initialize();
    enum CMD type;
    while (1) {
        
        switch (type = getCommand()) {
            case OK: {
                process_command();
                printf("%s> ", getcwd(HOME, 256));
                background = false;

                for(int i = 0; i < 100; i++) {
                    struct inst j = {""};
                    arr[i] = j;
                }
                firstWord = true;
                counter = 0;
                pipeCtr = 1;
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

