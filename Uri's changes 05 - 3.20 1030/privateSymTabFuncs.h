#include "bool.h"

typedef struct{
    char *symbol;
    int address;
    char *attribute1;
    char *attribute2;
} symbolTable;

boolean entryExist();
boolean externalExist();
boolean wasDefined(char *, int);
symbolTable getFromSymTab(char *);