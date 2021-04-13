#include <stdbool.h>
struct inst {
    char* name[50]; 
};

int counter;
struct inst arr[100];

struct alias {
    char* alias;
    char* original;
};
int aliasindex;
struct alias aliasarr[100];
bool firstWord;
int pipeCtr;
char *thecwd;

char** str_split(char* a_str, const char a_delim);
void addins(char *inst);