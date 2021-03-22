/* [mmn 14 : symbolTable.c]:
in this file:  prototypes of functions which are operating on the symbol-table or are related to it. 

author: Uri K.H,   Lihi Haim       Date: 21.3.2021 
ID: 215105321,     313544165       Tutor: Danny Calfon */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "symbolTable.h"
#include "util.h"
#include "label.h"
#include "privateSymTabFuncs.h"

/*typedef struct{
    int address;
    char *name;
} extFileRow;*/

static symbolTable *symTab;
static int symSize;
/*static extFileRow **externals;*/

static boolean definedAs(char *, char *);
static boolean addAttribToSymTab(char *, char *);

boolean pushExtern(char *line, int *lInd, int lineCnt)
{
    char *label;
    boolean added;

    jumpSpaces(line, lInd);
    label = readWord(line, lInd);
    if (!isValidLabel(label, lineCnt, FALSE) || isIlegalName(label, lineCnt) || wasDefined(label, lineCnt) || !(added = addToSymTab(label, "external", lineCnt)))
        return FALSE;
    if (added == ERROR)
        return ERROR;
    if(!isBlank(line, *lInd)){
        printf("error [line %d]: extranous text after end of command\n", lineCnt);
        return FALSE;
    }
    return TRUE;
}

boolean pushEntry(char *line, int* lInd, int lineCnt){
    char *operand, *operation;
    boolean added;
    int temp;

    jumpSpaces(line, lInd);
    temp = *lInd;
    if (!(operation = readWord(line, lInd)))
        return ERROR;
    if(strcmp(operation, ".entry") == 0){
        jumpSpaces(line, lInd);
        if (!(operand = readWord(line, lInd)))
            return ERROR;
        if(definedAs(operand, "external")){
            printf("error [line %d]: operand cnnot be named \".extern\" and \".entry\" at the same time\n", lineCnt);
            return FALSE;
        }
        if(!(added = addAttribToSymTab(operand, "entry")))
            return FALSE;
        if(added == ERROR)
            return ERROR;
        return TRUE;
    }
    else if(strcmp(operation, ".string") == 0 && strcmp(operation, ".data") == 0 && strcmp(operation, ".extern") == 0){
        *lInd = strlen(line);
        return TRUE;
    }
    return TRUE;
}

boolean checkEntry(char *line, int *lInd, int lineCnt){
    char *operand;
    jumpSpaces(line, lInd);
    if (!(operand = readWord(line, lInd)))
        return ERROR;
    if(strcmp(operand, "\0") == 0){ /* could not get a label */
        printf("error [line %d]: missing operand after directive \".entry\"\n", lineCnt);
        return FALSE;
    }
    if(!isValidLabel(operand, lineCnt, FALSE))
        return FALSE;
    if(!isBlank(line, *lInd)){
        printf("error [line %d]: extranous text after end of command\n", lineCnt);
        return FALSE;
    }
    return TRUE;
}

boolean addToSymTab(char *name, char *attrib, int lineCnt)
{
    static int size = 0;
    if (size == 0)
        symTab = malloc(sizeof(symbolTable));
    else
        symTab = realloc(symTab, (size++) * sizeof(symbolTable));
    if (!isAlloc(symTab))
        return ERROR;
    if (isIlegalName(name, lineCnt) || wasDefined(name, lineCnt))
        return FALSE;
    strcpy(symTab[size].symbol, name);
    symTab[size].attribute1 = malloc(strlen(attrib) + 1);
    if (!isAlloc(symTab[size].attribute1))
        return ERROR;
    strcpy(symTab[size].attribute1, attrib);
    if (strcmp(attrib, "data") == 0)
        symTab[size].address = getDC();
    else if (strcmp(attrib, "code") == 0)
        symTab[size].address = getIC();
    else if (strcmp(attrib, "external") == 0)
        symTab[size].address = 0;
    symSize = size;
    return TRUE;
}

static boolean addAttribToSymTab(char *sym, char *attrib)
{
    int i;
    for (i = 0; i < symSize; i++)
    {
        if (strcmp(symTab[i].symbol, sym) == 0)
        {
            symTab[i].attribute2 = malloc(strlen(attrib) + 1);
            if (!isAlloc(symTab[i].attribute2))
                return ERROR;
            strcpy(symTab[i].attribute2, attrib);
            return TRUE;
        }
    }
    return FALSE;
}

void encPlusIC(){
    int i;
    for(i = 0; i < symSize; i++)
        symTab[i].address += OS_MEM;
}

boolean wasDefined(char *sym, int lineCnt)
{
    int i;
    for (i = 0; i < symSize; i++)
    {
        if (strcmp(symTab[i].symbol, sym) == 0)
        {
            printf("error [line %d]: label has been already defined\n", lineCnt);
            return TRUE;
        }
    }
    return FALSE;
}

/* is the symbol already in the symbol table */
static boolean definedAs(char *sym, char *attrib){
    int i;
    boolean wasFound = FALSE;
    for (i = 0; i < symSize; i++){
        if (strcmp(symTab[i].symbol, sym) == 0){
            wasFound = TRUE;
            if (strcmp(symTab[i].attribute1, attrib) == 0 || strcmp(symTab[i].attribute2, attrib) == 0)
                return TRUE;
        }
    }
    if (!wasFound)
        return ERROR;
    return FALSE;
}

/* getFromSymTab(): searches for a matching symbol name in the symbol-table for a name
 and returns the row of the symbol. If the symbol is not defined in the symbol-table 
 a row will be returned with a symbol name is a NULL pointer.
 parameters: sym - the symbol name to search for 
 */
symbolTable getFromSymTab(char *sym){
    static int i = 0;
    symbolTable error;
    error.symbol = NULL;

    while (i < symSize){
        if (strcmp(symTab[i].symbol, sym) == 0)
            return symTab[i];
        i++;
    }
    return error;
}

/* entryExist(): returns if there is an entry variable in the symbol-table */
boolean entryExist(){
    int i;
    for (i = 0; i < symSize; i++){
        if (strcmp(symTab[i].attribute2, "entry") == 0)
            return TRUE;
    }
    return FALSE;
}

/* externalExist(): returns if there is an external variable in the symbol-table */
boolean externalExist(){
    int i;
    for (i = 0; i < symSize; i++){
        if (strcmp(symTab[i].attribute1, "extern") == 0)
            return TRUE;
    }
    return FALSE;
}

void pushEntryToFile(FILE *fd){
    int i;
    for(i = 0; i < symSize; i++){
        if (strcmp(symTab[i].attribute2, "entry") == 0)
            fprintf(fd, "%s %d\n", symTab[i].symbol, symTab[i].address);
        i++;
    }
    return;
}