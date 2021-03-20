/* [mmn 14: compiler.c]:
This file contiains the main function in the compilation process of the assembly file.
The compile() function will compile the wnated assmebly file into three optput files: 
fileName.ob, fileName.ent, fileName.ext (if possible)

This file also conatins the sub functions nedded for the main 'compile()' fucntion.
*/

#include <stdlib.h>
#include <string.h>
#include "ineterface.h"
#include "files.h"
#include "line.h"
#include "util.h"
#include "label.h"
#include "instructionHandeling.h"
#include "dataHandeling.h"
#include "symbolTable.h"
#include "statmentType.h"

#define FOREVER 1 

/* compile(): The function compiles an Assembly file and make one to three files in addition:
 file.ob - a binary file, which includes the memory....
 file.ent - an "entry" file, which includes the names of all the variable which other files can see and
            their address in the RAM.
 file.ext - an "extern" file, which includes the names of all the external variables (from other files) 
            and their calling address in the RAM.
*/
boolean compile(char *fileName){
     /* files variables */
     FILE *fd;
     char *tempFileName = malloc(strlen(fileName) + MAX_SUFIX_LEN + 1);
     int lineCnt = 0;
     /* line variables */
     char line[LINE_LEN];             /* array for handling a line of assembly code */
     int *lInd = malloc(sizoef(int)); /* the Line-Index */
     char *tempLine, *optLabel[1];
     /* label and compilation variables and falgs */
     statType type;
     boolean compSuc = TRUE, labelExist = FALSE, pushed = TRUE;

     if (!isAlloc(lInd) || !isAlloc(tempFileName)){
          free(lInd);
          free(tempFileName);
          return ERROR;
     }

     fd = openf(addSufix(fileName, ".as", tempFileName), "r");
     if (!fd)
          return TRUE;
     clean(FALSE); /* setting IC to 100 and DC to 0 */
     while (FOREVER){
          lineCnt++;
          tempLine = readLine(fd, LINE_LEN); /* reading a line from the file */
          if (*tempLine == '\0') /* end of file */
               break;
          strcpy(line, tempLine);
          free(tempLine);

          if (!(*optLabel = readWord(line, lInd))){
               free(lInd);
               free(tempFileName);
               return ERROR;
          }
          if(!strcmp(*optLabel, "\0") && isLabel(*optLabel)){
               labelExist = TRUE;
               if(!isValidLabel(optLabel, lineCnt)) /* could not get a label */
                    compSuc = FALSE;
          }

          switch ((type = getStatType(line))){ /* finding the statment type (instruction/ directive/ balnk line/ comment) */
          case blank:
          case comment:
               continue;
          case directive:
               int command = getDirCom(command);
               switch (command){
               case data:
               case string:
                    if (labelExist){
                         if (isIlegalName(*optLabel, lineCnt) || wasDefined(optLabel, lineCnt)) /* if label was all ready defined */
                              compSuc = FALSE;
                         else if (command == data){
                              if (!(pushed = addToSymTab(*optLabel, "data", lineCnt)))
                                   return FALSE;
                              if(pushed == ERROR){
                                   clean(FALSE);
                                   return ERROR;
                              }
                         }
                         else if (command == string){
                              if (!(pushed = addToSymTab(*optLabel, "string", lineCnt)))
                                   return FALSE;
                              if(pushed == ERROR){
                                   clean(FALSE);
                                   return ERROR;
                              }
                         }
                    }
                    if (!(pushed = pushData(line, lInd)))
                         compSuc = FALSE;
                    else if (pushed == ERROR){
                         clean(FALSE);
                         return ERROR;
                    }
                    continue;
               case external:
                    if (labelExist){
                         isIlegalName(*optLabel, lineCnt);
                         wasDefined(*optLabel, lineCnt);
                         printf("warnning [line %d]: a label before the directive \".extern\" is meaningless\n", lineCnt);
                    }
                    if (!(pushed = pushExtern(line, lInd)))
                         compSuc = FALSE;
                    else if (pushed == ERROR){
                         clean(FALSE);
                         return ERROR;
                    }
                    continue;
               case entry:
                    continue;
               default:
                    compSuc = FALSE;
                    break;
               }
          case order:
               if (labelExist){
                    if (isIlegalName(*optLabel, lineCnt) || wasDefined(optLabel, lineCnt) || !(pushed = addToSymTab(*optLabel, "code", lineCnt)))
                         compSuc = FALSE;
                    if(pushed == ERROR){
                         clean(FALSE);
                         return ERROR;
                    }
               }
               if(!(pushed = pushInstFirst(line, lInd)))
                    compSuc = FALSE;
               else if(pushed == ERROR){
                    clean(FALSE);
                    return ERROR;
               }
               break;
          }
     }
     if (compSuc == FALSE)
          return FALSE;
     rewind(fd);
     while (FOREVER){
          lineCnt++;
     }
     fclose(fd);
     free(tempFileName);
}

void clean(boolean terminate){
     cleanDC(terminate);
     cleanIC(terminate);
     cleanSymTab(terminate);
}