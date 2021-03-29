#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>

#include "global.h"
#include "parser.tab.h"
enum CMD { BYE, ERRORS, OK };
struct alias {
    char* alias;
    char* original;
};
struct alias aliasarr[100];
extern char **environ;

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
        // do something here
        exit(0);
    }
    else if (child_pid == -1)
        perror("fork");
    else {
        waitpid(child_pid, &retStatus, 0);
    }
}
void _ls(const char *dir,int op_a,int op_l)
{
	//Here we will list the directory
	struct dirent *d;
	DIR *dh = opendir(dir);
	if (!dh)
	{
		if (errno = ENOENT)
		{
			//If the directory is not found
			perror("Directory doesn't exist");
		}
		else
		{
			//If the directory is not readable then throw error and exit
			perror("Unable to read directory");
		}
		exit(EXIT_FAILURE);
	}
	//While the next entry is not readable we will print directory files
	while ((d = readdir(dh)) != NULL)
	{
		//If hidden files are found we continue
		if (!op_a && d->d_name[0] == '.')
			continue;
		printf("%s  ", d->d_name);
		if(op_l) printf("\n");
	}
	if(!op_l)
	printf("\n");
}
void process_command() {
    if(!strcmp(*arr[0].name, "setenv")) {
        if (strcmp(*arr[2].name, "") != 0) { 
            if(setenv(*arr[1].name,*arr[2].name, 0) == -1) {
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
        //printf("implement %s\n", *arr[0].name);
    }
    else if (!strcmp(*arr[0].name, "ls")) {
        _ls(".",0,0);
        
    }
    else if (!strcmp(*arr[0].name, "cd")) {
        chdir(*arr[1].name);
    }
    else if (!strcmp(*arr[0].name, "alias")) {
        printf("implement %s\n", *arr[0].name);
        // alias

        // alias name word
    }
    else if (!strcmp(*arr[0].name, "unalias")) {
        printf("implement %s\n", *arr[0].name);
    }
    else if (!strcmp(*arr[0].name, "bye")) {
        exit(1);
    } else {
        doit();
    }
}

int main()
{
    char HOME[256];

    if (getcwd(HOME, sizeof(HOME)) == NULL) {
        // error
    }

    for (int i = 0; i < 100; i++)
    {
        struct inst j = {""};
        arr[i] = j;
    }

    printf("%s> ", getcwd(HOME, 256));
 
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
