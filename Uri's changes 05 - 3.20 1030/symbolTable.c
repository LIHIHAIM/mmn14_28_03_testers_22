#include <stdio.h>
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

boolean pushExtern(char *line, int *lInd, int lineCnt)
{
    int size = 1;
    char *label[1];
    boolean added;

    jumpSpaces(line, lInd);
    *label = readWord(line, lInd);
    if (!isValidLabel(label, lineCnt) || isIlegalName(*label, lineCnt) || wasDefined(*label, lineCnt) || !(added = addToSymTab(*label, "external", lineCnt)))
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
    if(strcmp(operation, ".entry")){
        jumpSpaces(line, lInd);
        if (!(operand = readWord(line, lInd)))
            return ERROR;
        if(definedAs(operand, "external")){
            printf("error [line %d]: operand cnnot be named \".extern\" and \".entry\" at the same time\n");
            return FALSE;
        }
        if(!(added = addAttribToSymTab(operand, "entry")))
            return FALSE;
        if(added == ERROR)
            return ERROR;
        return TRUE;
    }
    else if(!strcmp(operation, ".string") && !strcmp(operation, ".data") && !strcmp(operation, ".extern")){
        *lInd = strlen(line);
        return TRUE;
    }
    *lInd = temp;
    return TRUE;
}

boolean checkEntry(char *line, int *lInd, int lineCnt){
    char *operand;
    jumpSpaces(line, lInd);
    if (!(operand = readWord(line, lInd)))
        return ERROR;
    if(strcmp(operand, "\0")){ /* could not get a label */
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
    if (!isAlloc(*symTab))
        return ERROR;
    if (isIlegalName(label, lineCnt) || wasDefined(label, lineCnt))
        return FALSE;
    symTab[size].symbol = name;
    symTab[size].attribute1 = malloc(strlen(attrib) + 1);
    if (!isAlloc(symTab[size].attribute1))
        return ERROR;
    strcpy(symTab[size].attribute1, attrib);
    if (strcmp(attrib, "data"))
        symTab[size].address = getDC();
    else if (strcmp(attrib, "code"))
        symTab[size].address = getIC();
    else if (strcmp(attrib, "external"))
        symTab[size].address = 0;
    symSize = size;
    return TRUE;
}

static boolean addAttribToSymTab(char *sym, char *attrib)
{
    int i;
    for (i = 0; i < symSize; i++)
    {
        if (strcmp(symTab[i].symbol, sym))
        {
            symTab[i].attribute2 = malloc(strlen(attrib) + 1);
            if (!isAlloc(symTab[i].attribute2))
                return ERROR;
            strcpy(symTab[i].attribute2, attrib);
            return TRUE
        }
    }
    return FALSE;
}

void encPlusIC(){
    int i;
    for(i = 0; i < OS_MEM; i++)
        symTab[i].address += OS_MEM;
}

boolean wasDefined(char *sym, int lineCnt)
{
    int i;
    for (i = 0; i < symSize; i++)
    {
        if (strcmp(symTab[i].symbol, sym))
        {
            printf("error [line %d]: label has been already defined\n", lineCnt);
            return TRUE;
        }
    }
    return FALSE;
}

/* is the symbol already in the symbol table */
static boolean definedAs(char *sym, char *attrib)
{
    int i;
    boolean wasFound = FALSE;
    for (i = 0; i < symSize; i++)
    {
        if (strcmp(symTab[i].symbol, sym))
        {
            wasFound = TRUE;
            if (strcmp(symTab[i].attribute1, attrib) || strcmp(symTab[i].attribute2, attrib))
                return TRUE;
        }
    }
    if (!wasFound)
        return ERROR;
    return FALSE;
}

/* searching for a symbol name in the symbol table and returns the row of the symbol */
symbolTable getFromSymTab(char *sym)
{
    int i = 0;
    symbolTable error;
    error.symbol = NULL;

    while (i < symSize)
    {
        if (strcmp(symTab[i].symbol, sym))
            return symTab[i];
        i++;
    }
    return error;
}

boolean entryExist()
{
    int i;
    for (i = 0; i < symSize; i++)
    {
        if (strcmp(symTab[i].attribute1, "entry") || strcmp(symTab[i].attribute2, "entry"))
            return TRUE;
    }
    return FALSE;
}

boolean externalExist()
{
    int i;
    for (i = 0; i < symSize; i++)
    {
        if (strcmp(symTab[i].attribute1, "extern") || strcmp(symTab[i].attribute2, "extern"))
            return TRUE;
    }
    return FALSE;
}

void cleanSymTab()
{
    free(symTab);
    return;
}