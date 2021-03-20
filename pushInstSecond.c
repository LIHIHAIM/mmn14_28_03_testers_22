
word *JumpToBlankCell()
{
    int i = 0;
    while (instImage[1][i].ARE != '\0')
    {
        i++;
    }
    return (&instImage[1][i]);
}

boolean pushInstSecond(char *line, int *lInd)
{
    word *blankIC;
    char *operand;
    symbolTable symTabRow;
    while (!isblank(line,lInd))
    {
        isThereComma(line, lInd);
        if (!(getAddressingMethod(line, lInd) == immidiate))
        {
            blankIC = JumpToBlankCell();
            operand = readWord(line[*lInd]);
            symTabRow = getFromSymTab(line, lInd);
            if (wasDefined(operand))
            {
                if (!strcmp(symTabRow.attribute1, "external"))
                {
                    blankIC->ARE = 'R';
                }
                else
                {
                    blankIC->ARE = 'E';
                }
                blankIC->wrd=symTabRow.address;
                return TRUE;
            }
            return FALSE;
        }
    }
}