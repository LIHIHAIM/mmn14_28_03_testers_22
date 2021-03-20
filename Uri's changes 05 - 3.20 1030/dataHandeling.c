#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "word.h"
#include "label.h"
#include "privateDataFuncs.h"

static int DC;
static word *dataImage[1];

/* pushing words to the memory image */
boolean pushData(char *line, int *lInd, int lineCnt){
    int size = 1;
    boolean error = FALSE;

    jumpSpaces(line, lInd);
    while(line[*lInd] != '\0'){
        if(DC == 0) /* pushing the data to the data image */ 
            *dataImage = calloc(1, sizeof(word));
        else
            *dataImage = realloc(*dataImage, DC * sizeof(word));
        if(!isAlloc(*dataImage))
            return ERROR;
        if(!(dataImage[DC] = scanDataParams(line, lInd))) /* scanning data from the line */
            error = TRUE;
        DC++;
        if(!isThereComma(line, lInd, lineCnt))
            return FALSE;
        if(error == TRUE)
            continue;
    }
    return TRUE;
    /*int size = 1, i = 0, j;
    word *params = calloc(size, sizeof(word));
    boolean error = FALSE;

    if(!isAlloc(params))
        return ERROR;

    while(line[*lInd] != '\0'){
        word *scaned;
        params = realloc(params, (++size) * sizeof(paramType));

        if(!isAlloc(params))
            return ERROR; 
        if(!isThereComma(line, lInd, lineCnt))
            return FALSE;
        if(!(scaned = scanDataParams(line, lInd))) /* scanning data from the line 
            error = TRUE;
        if(error == TRUE)
            continue;

        j = 0;
        while(scaned[j] != NULL){
            if(DC == OS_MEM) /* pushing the data to the data image *
                dataImage = calloc(1, sizeof(word));
            else
                dataImage = realloc(dataImage, (DC - OS_MEM) * sizeof(word));
            if(!isAlloc(params))
                return ERROR;
            dataImage[DC - OS_MEM] = newData;
            DC++;
            j++;
        }
    }
    return TRUE;*/
}

void cleanDC(boolean terminate){
     int i = 0;
    while(1){
        if(i < DC)
            free(dataImage[i]);
        i++;
    }
    DC = 0;
    if(terminate){
        free(dataImage);
        return;
    }
    *dataImage = NULL;
    return;
}

int getDC(){
    return DC;
}

pushDataToFile(FILE *fd){
    int i;
    for(i = 0; i < DC; i++)
        fprintf(fd, "%04d %x %c\n", getIC()+i+1, (*dataImage)[i].wrd, (*dataImage)[i].ARE);
    return;
}