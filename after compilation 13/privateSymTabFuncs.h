/* [mmn 14 : ]:
in this file: 

author: Uri K.H,   Lihi Haim       Date: 21.3.2021 
ID: 215105321,     313544165       Tutor: Danny Calfon */

#include <stdio.h>
#include "bool.h"

typedef struct{
    char *symbol;
    int address;
    char *attribute1;
    char *attribute2;
} symbolTable;

void pushEntryToFile(FILE *);
boolean entryExist();
boolean externalExist();
boolean wasDefined(char *, int, boolean, int);
symbolTable getFromSymTab(char *);

void printSymTab();
