#include <stdlib.h>
#include "files.h"
#include "util.h"
#include "privateDataFuncs.h"
#include "privateInstFuncs.h"
#include "privateSymTabFuncs.h"

#define RAM_SIZE 4096

/* openf(): Open a given file in a given status and returns the pointer to the file. */
FILE *openf(char *file, char *status){
     FILE *fd = fopen(file, status);
     if(!fd)
          printf("error : could not open file: \"%s\"\n",file);
     return fd;
}

/* readLine(): Reads a line (length 80 characters) from a given file pointer - fd, 
 to a dinamic array and returns the array. */
char *readLine(FILE *fd, int len){
     char *line = calloc(len,sizeof(char));
     fgets(line,len,fd);
     return line;
}

void buildOutputFiles(char *fileName){
     FILE *fd;
     int i;
     char *tempFileName = malloc(strlen(fileName) + MAX_SUFIX_LEN + 1);
     if(getIC() + getDC() > RAM_SIZE){
          printf("error: file is too long\n");
          free(tempFileName);
          return;
     }
     fd = openf(addSufix(fileName, ".ob", tempFileName), "w");
     fprintf(fd, "\t%d %d\n", getIC()-OS_MEM, getDC());
     pushInstsToFile(fd);
     pushDataToFile(fd);
     fclose(fd);
     
     if(entryExist()){
          fd = openf((addSufix(fileName, ".ent", tempFileName)));
          
          fclose(fd);
     }
     if(externalExist()){
          fd = openf((addSufix(fileName, ".ext", tempFileName)));

          fclose(fd);
     }
     return;
}

char *addSufix(char *dest, char *suffix, char *tempNam){
     strcpy(tempNam, dest);
     strcat(tempNam, suffix);
     return tempNam;
}