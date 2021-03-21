/* [mmn 14 : ]:
in this file: 

author: Uri K.H,   Lihi Haim       Date: 21.3.2021 
ID: 215105321,     313544165       Tutor: Danny Calfon */

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "word.h"
#include "label.h"
#include "privateDataFuncs.h"

#define MAX_DEC_LEN 5

static int DC;
static word *dataImage;

static word *scanDataParams(char *, int *, char *, int);

/* pushing words to the memory image */
boolean pushData(char *line, int *lInd, char *type, int lineCnt){
    boolean error = FALSE;
    int i = 0;
    static word *tempDataImage;
    word *scaned;

    jumpSpaces(line, lInd);
    while(line[*lInd] != '\0'){
        if(DC == 0) /* pushing the data to the data image */ 
            tempDataImage = calloc(1, sizeof(word));
        else
            tempDataImage = realloc(tempDataImage, DC * sizeof(word));
        if(!isAlloc(tempDataImage)){
            free(scaned);
            free(tempDataImage);
            return ERROR;
        }
        if(!(scaned = scanDataParams(line, lInd, type, lineCnt))) /* scanning data from the line */
            error = TRUE;
        while(scaned[i].ARE != BLANK_ARE){
            tempDataImage = realloc(tempDataImage, DC * sizeof(word));
            if(!isAlloc(tempDataImage)){
                free(scaned);
                free(tempDataImage);
                return ERROR;
            }
            (tempDataImage)[DC] = scaned[i];
            DC++;
        }
        free(scaned);
        if(!isThereComma(line, lInd, lineCnt))
            return FALSE;
        if(error == TRUE)
            continue;
    }
    dataImage = tempDataImage;
    printf("pushed!\n");
    return TRUE;
}
    

/*reads the parameters and returns an array of words*/
static word *scanDataParams(char *line, int *lInd, char *type, int lineCnt)
{
    int size = 1, decimal, temp;
    char curr, currentWrd[MAX_DEC_LEN];
    word *dataForPush = calloc(size, sizeof(word));

    if (!isAlloc(dataForPush)){
        free(dataForPush);
        return NULL;
    }
    /*There must be parameters after .data or .stirng in Directive*/
    if (line[*lInd] == '\0'){
        printf("error [line %d]: There must be parameters after the directives \".data\" or \".stirng\"\n", lineCnt);
        free(dataForPush);
        return NULL;
    }
    /*if it's a string case:*/
    if (strcmp(type, "string") == 0){
        if (line[*lInd] != '\"'){
            printf("error [line %d]: the operand of the directive \".string\" must start with a \"\n", lineCnt);
            free(dataForPush);
            return NULL;
        }
        (*lInd)++; /* the beginning of the string */
        size = 2;
        while ((curr = line[*lInd]) != '\0' && curr != '\"'){
            if (curr == '\\' && line[*lInd + 1] == '\"'){
                dataForPush = realloc(dataForPush, ++size * sizeof(word));
                if (!isAlloc(dataForPush))
                    return NULL;
                dataForPush[size - 3].wrd = '\"';
                (*lInd) += 2;
                continue;
            }
            dataForPush = realloc(dataForPush, ++size * sizeof(word));
            if (!isAlloc(dataForPush))
                return NULL;
            dataForPush[size - 3].wrd = curr;
            dataForPush[size - 3].ARE = 'A';
            (*lInd)++;
        }
        if (curr == '\0'){ /* string ended unaturaly at the end of the array and not by: " */
            printf("error [line %d]: There must be \" at the end of a string\n", lineCnt);
            free(dataForPush);
            return NULL;
        }
        (*lInd)++;
        if (!isBlank(line, *lInd)){
            free(dataForPush);
            return NULL;
        }
        dataForPush[size - 2].wrd = '\0';
        dataForPush[size - 2].ARE = 'A';
        dataForPush[size - 1].ARE = BLANK_ARE;
        return dataForPush;
    }

    if (strcmp(type, "data") != 0){
        free(dataForPush);
        return NULL;
    }
    if (((curr = line[*lInd]) != '+' && curr != '-' && !isdigit(curr))){
        printf("error [line %d]: the decimal number of \".data\" operand must start with a sign (+/-) or a digit\n", lineCnt);
        free(dataForPush);
        return NULL;
    }
    temp = *lInd;
    while ((curr = line[*lInd]) != '\0' && !isspace(curr) && *lInd - temp < MAX_DEC_LEN){
        if (isdigit(curr))
            currentWrd[*lInd - temp] = curr;
        else {
            printf("error [line %d]: the decimal number of the \".data\" directive operand must include only digits (and an optioanl sign: +/- at its start)\n", lineCnt);
            free(dataForPush);
            return NULL;
        }
        (*lInd)++;
    }
    currentWrd[*lInd - temp] = '\0';
    if (isdigit(line[*lInd + 1]) || (decimal = atoi(currentWrd)) < MIN_WORD_VAL || decimal > MAX_WORD_VAL)
    {
        printf("error [line %d]: the decimal number of the \".data\" directive operand is too %s\n", lineCnt, (decimal > MAX_WORD_VAL || isdigit(line[*lInd + 1]) ? "big" : "small"));
        free(dataForPush);
        return NULL;
    }
    dataForPush = realloc(dataForPush, ++size * sizeof(word));
    if (!isAlloc(dataForPush))
        return NULL;
    dataForPush[size - 2].wrd = decimal;
    dataForPush[size - 2].ARE = 'A';
    dataForPush[size - 1].ARE = BLANK_ARE;
    return dataForPush;
}

void cleanDC(){
    /*int i = 0;
    while(1){
        if(i < DC)
            free(dataImage[i]);
        i++;
    }*/
    DC = 0;
    dataImage = NULL;
    return;
}

int getDC(){
    return DC;
}

void pushDataToFile(FILE *fd){
    int i;
    for(i = 0; i < DC; i++)
        fprintf(fd, "%04d %x %c\n", getIC()+i+1, dataImage[i].wrd, dataImage[i].ARE);
    return;
}

