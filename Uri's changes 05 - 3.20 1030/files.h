#include <stdio.h>
#define LINE_LEN 81
#define MAX_SUFIX_LEN 4

FILE *openf(char *, char *);
char *readLine(FILE *, int);
char *addSufix(char *, char *, char *);