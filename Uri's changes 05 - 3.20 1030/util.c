#include <ctype.h>
#include "util.h"

boolean isBlank(char *line, int ind)
{
    for (; !isspace(line[ind]) && line[ind] != '\0'; ind++);
    if (line[ind] == '\0')
        return TRUE;
    return FALSE;
}

void jumpSpaces(char *line, int *ind)
{
    char curr;
    while ((curr = line[*ind]) != '\0' && isspace(curr))
        (*ind)++;
    return;
}

boolean isThereComma(char *line, int *lInd, int lineCnt)
{
    int firstComma = 0;
    char curr;
    while ((curr = line[lInd]) != '\0' && (isspace(curr) || curr == ','))
    {
        if (curr == ',')
            firstComma++;
        if (firstComma > 1)
        {
            printf("error [line %d]: between parameters must seperate one comma only\n", lineCnt);
            return FALSE;
        }
        (*lInd)++;
    }
    if (curr == '\0' && firstComma > 1)
    {
        printf("error [line %d]: extranous comma after the last operand in the line\n");
        return FALSE;
    }
    else if (firstComma == 0)
    {
        printf("error [line %d]: between parameters must seperate a comma\n", lineCnt);
        return FALSE;
    }
    return TRUE;
}

boolean isAlloc(void *p)
{
    if (!p)
    {
        printf("error : memory allocation failed, could not continue the compilation process\n");
        return FALSE;
    }
    return TRUE;
}
