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
    if (!isValidLabel(label, lineCnt, FALSE) || isIlegalName(label, lineCnt) || wasDefined(label, lineCnt, TRUE, 1) || !(added = addToSymTab(label, "external", lineCnt)))
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

    jumpSpaces(line, lInd);
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
    else if(strcmp(operation, ".string") == 0 || strcmp(operation, ".data") == 0 || strcmp(operation, ".extern") == 0){
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
    int tabInd;
    if (symSize == 0){
        symTab = malloc(sizeof(symbolTable));
        symSize++;
    }
    else
        symTab = realloc(symTab, ++symSize * sizeof(symbolTable));
    if (!isAlloc(symTab))
        return ERROR;
    tabInd = symSize-1;
    if (isIlegalName(name, lineCnt) || wasDefined(name, lineCnt, TRUE, 1))
        return FALSE;
    symTab[tabInd].symbol = malloc(strlen(name) + 1);
    symTab[tabInd].attribute1 = malloc(strlen(attrib) + 1);
    symTab[tabInd].attribute2 = NULL;
    if (!isAlloc(symTab[tabInd].symbol) || !isAlloc(symTab[tabInd].attribute1)){
        free(symTab[tabInd].symbol);
        free(symTab[tabInd].attribute1);
        return ERROR;
    }
    strcpy(symTab[tabInd].symbol, name);
    strcpy(symTab[tabInd].attribute1, attrib);
    if (strcmp(attrib, "data") == 0)
        symTab[tabInd].address = getDC();
    else if (strcmp(attrib, "code") == 0)
        symTab[tabInd].address = getIC();
    else if (strcmp(attrib, "external") == 0)
        symTab[tabInd].address = 0;
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

boolean wasDefined(char *sym, int lineCnt, boolean printErr, int pass)
{   
    int i;
    for (i = 0; i < ((pass == 1)?(symSize-1):(symSize)); i++){
        if (strcmp(symTab[i].symbol, sym) == 0){
            if(printErr)
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
        if (symTab[i].attribute2 != NULL && strcmp(symTab[i].attribute2, "entry") == 0)
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

void cleanSymTab(){
    int i;
    symSize = 0;
    if(!symTab)
        return;
    for(i = 0; i < symSize; i++){
        free(symTab[i].attribute1);
        free(symTab[i].symbol);
    }
}

void printSymTab(){
    int i;
    for(i = 0; i < symSize; i++){
        printf("sym %d: %s\n", i, symTab[i].symbol);
    }
}