/*reads the parameters and returns an array of words*/

word *scanDataParam(char *line, int *lInd)
{
     int temp, i,size=1;
     //flag for a \" case in STRING or a case of --/++ in DATA
     int check=FALSE;
     *lInd = jumpSpaces(line, *lInd);
     temp = *lInd;
     char currentwrd[82]='\0';
     word *dataForPush = calloc(size, sizeof(word));
     int i;
     int size = 1;
     //There must be parameters after .data or .stirng in Directive
     if (line[*lInd] == '\0')
     {
          printf("error : There must be parameters after .data or .stirng in Directive\n");
          return NULL;
     }
     //if it's a string case:
     if (line[*lInd] == '"')
     {
          *lInd++; /* the beginning of the string */
          i = 0;
          while (line[*lInd] != '\0' || ((line[*lInd] != '"')||check))
          {
               check=FALSE;
               if(line[*lInd]=='\\')
               {
                    check=TRUE;
               }
               dataForPush = (char *)realloc(++size, sizeof(word));
               dataForPush[size - 1].wrd = line[*lInd];
               *lInd++;
          }
          
          if (line[*lInd] != '"')
          {
               printf("error : There must be \" at the end of a string\n");
               return NULL;
          }
          }
          if (lInd != '\0')
          {
               jumpSpaces(line, *lInd);
               if (lInd != '\0')
                    return NULL;
          }
          //do i need realloc?
          //i removed the atoi, it's a string not an integer, right?
          return (dataForPush);
     }
     //if it's DATA case:
     while (line[*lInd] != '\0')
     {
          //word
          //currentword word nullify:
          strcpy(currentwrd,'\0');
          check=FALSE;
          jumpSpaces(line, *lInd);
          // should prevent a case of '22 2, 3'
          if(!isThereComma())
          {
               return NULL;
          }
          dataForPush = (char *)realloc(++size, sizeof(word));
          while ((line[*lInd] != ',' || isdigit(line[*lInd]))&&((line[*lInd]=='-'||line[*lInd]=='+')&&(!check)))
          {
               //every char into a full string of a number
               // fix cases of ++ or --
               if(line[*lInd]=='-'||line[*lInd]=='+')
               {
                    check=TRUE;
               }
               strcat(currentwrd,line[*lInd]);
               *lInd++;
          }
          if(line[*lInd]=='-'||line[*lInd]=='+')
          {
               printf("error : There can't be more than one sign of inc or dec \n");
               return NULL;
          }
          dataForPush[size - 1].wrd = atoi(currentwrd);
          size++;
          //a number should be between -2048 to 2047 to be a word
          if ((dataForPush[size-1]>2047)||(dataForPush[size-1]<-2048))
          {
               printf("error : The parameters in .data Directive are invalid\n");
               return NULL;
          }
     }
}