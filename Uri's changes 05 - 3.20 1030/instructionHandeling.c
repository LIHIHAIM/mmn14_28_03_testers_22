/* [mmn 14 : ]:
in this file: 

author: Uri K.H,   Lihi Haim       Date: 21.3.2021 
ID: 215105321,     313544165       Tutor: Danny Calfon */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "instructionStat.h"
#include "label.h"
#include "util.h"
#include "word.h"
#include "buildWord.h"
#include "privateInstFuncs.h"
#include "privateSymTabFuncs.h"

/* memory macros */
#define REGISTERS 8     /* the number of general register the imagenry computer's CPU has */
#define INSTRUCTIONS 16 /* the amount of instructions in the imagenary computer's assembly language */

#define MVAM 3      /* Maximum Valid Addressing Method options - for any of the instruction commands */
#define MAX_WORDS 3 /* maximum words to build from an instruction command (operation, source operand, target operand) */
#define BLANK_ARE ' '
#define UNKNOWN_ARE '?' /* a flag in the ARE field at the memory image for unused cells (for later use) */
#define UNXERROR_ARE '\0'
#define ERROR_ARE '0' /* a flag in the ARE field at the meory image for an unexpected error in functions which \
return a pointer to a 'word' or a 'word' */
#define MIN_WORD_VAL (-pow(2, 11))
#define MAX_WORD_VAL (pow(2, 11) - 1)

#define MAKE_STRING(str) (#str)                  /* makes a string */
#define REGISTER_NAME(reg) (MAKE_STRING(r##reg)) /* makes a register name (string starts with 'r' and then a number) */
#define freeArr(arr, size)         \
    {                              \
        int i;                     \
        for (i = 0; i < size; i++) \
            free(arr[i]);          \
        free(arr);                 \
    }

typedef struct
{
    char *name;
    int opcode;
    int funct;
    int SVAMs[MVAM]; /* Source operand Valid Addressing Methods */
    int TVAMs[MVAM]; /* Target operand Valid Addressing Methods */
    int ops;         /* operands */
} inst;

static inst instsList[INSTRUCTIONS] = {
    /* the two operand instructions */
    {"mov", 0, 0, {0, 1, 3}, {1, 3, -1}, 2},
    {"cmp", 1, 0, {0, 1, 3}, {0, 1, 3}, 2},
    {"add", 2, 10, {0, 1, 3}, {1, 3, -1}, 2},
    {"sub", 2, 11, {0, 1, 3}, {1, 3, -1}, 2},
    {"lea", 4, 0, {1, -1}, {1, 3, -1}, 2},
    /* the one operand instructions */
    {"clr", 5, 10, {-1}, {1, 3, -1}, 1},
    {"not", 5, 11, {-1}, {1, 3, -1}, 1},
    {"inc", 5, 12, {-1}, {1, 3, -1}, 1},
    {"dec", 5, 13, {-1}, {1, 3, -1}, 1},
    {"jmp", 9, 10, {-1}, {1, 2, -1}, 1},
    {"bne", 9, 11, {-1}, {1, 2, -1}, 1},
    {"jsr", 9, 12, {-1}, {1, 2, -1}, 1},
    {"red", 12, 0, {-1}, {1, 3, -1}, 1},
    {"prn", 13, 0, {-1}, {0, 1, 3}, 1},
    /* the zero operand instructions */
    {"rts", 14, 0, {-1}, {-1}, 0},
    {"stop", 15, 0, {-1}, {-1}, 0}};

static enum addressing { immidiate,
                         direct,
                         relative,
                         registerDirect };
static int IC; /* Instructions-Counter, Data-Counter (index) */
static word *instImage[1];

static word *JumpToBlankCell();
static word *scanInstParams(char *, int *, int);
static boolean isValidOpsAmount(int, int, int);
static boolean isValidSAM(int, int, int);
static boolean isValidTAM(int, int, int);
static int getOperationIndex(char *, int);
static int getAddressingMethod(char *, int);

boolean pushInstFirst(char *line, int *lInd, int lineCnt)
{
    int temp = IC, i;
    word *toPush = scanInstParams(line, lInd, lineCnt);
    if (toPush->ARE == UNXERROR_ARE)
        return ERROR;
    if (toPush == NULL)
        return FALSE;
    if (temp == OS_MEM) /* pushing the data to the data image */
        *instImage = calloc(1, sizeof(word));
    else
        *instImage = realloc(*instImage, (IC - OS_MEM) * sizeof(word *));
    if (!isAlloc(*instImage))
        return ERROR;
    for (i = 0; i < IC - temp; i++)
        (*instImage)[i + temp] = toPush[i];
    return TRUE;
}

boolean pushInstSecond(char *line, int *lInd)
{
    word *blankIC;
    char *operand;
    symbolTable symTabRow;
    while (!isblank(line,lInd))
    {
        isThereComma(line, lInd);
        if (getAddressingMethod(line, lInd) != immidiate)
        {
            blankIC = JumpToBlankCell();
            operand = readWord(line[*lInd]);
            symTabRow = getFromSymTab(line, lInd);
            if (wasDefined(operand))
            {
                if (!strcmp(symTabRow.attribute1, "external"))
                    blankIC->ARE = 'R';
                else
                    blankIC->ARE = 'E';
                blankIC->wrd=symTabRow.address;
                return TRUE;
            }
            return FALSE;
        }
    }
}

static word *JumpToBlankCell(){
    static int i = 0;
    while (instImage[1][i].ARE != '\0')
        i++;
    return (&instImage[1][i]);
}

word getDecWord(char *toWord, int lineCnt)
{
    int i = 0, decimal;
    char *temp = toWord + 1, curr = temp[i];
    word finalWrd, error;
    error.ARE = ERROR_ARE;

    if (curr == '+' && curr == '-')
        i++;
    for (; i < strlen(temp); i++)
    {
        curr = temp[i];
        if (!isdigit(curr))
        {
            printf("error [line %d]: in the immediate addressing method can be writen only the numericl characters\n", lineCnt);
            printf("\t(and '+' or '-' in the start of the number)\n");
            return error;
        }
    }
    decimal = atoi(temp);
    if (decimal < MIN_WORD_VAL || decimal > MAX_WORD_VAL)
    {
        printf("error [line %d]: decimal numbers can be handled only in range: %d to %d\n", lineCnt, MIN_WORD_VAL, MAX_WORD_VAL);
        return error;
    }
    finalWrd.wrd = decimal;
    finalWrd.ARE = 'A';
    return finalWrd;
}

static word *scanInstParams(char *line, int *lInd, int lineCnt)
{
    int opAmnt = 1, operationInd; /* amount of operands (including the command), index of the command in the instructions array */
    int SAM = 0, TAM = 0;         /* Source and Target Addressing Methods */
    char **operands = calloc(opAmnt, sizeof(char *));
    word *memory = malloc(MAX_WORDS, sizeof(word));

    word *unxErr, unknown;
    unxErr->ARE = UNXERROR_ARE; /* unxErr - returned if an unexpected error has acurred */
    unknown.ARE = UNKNOWN_ARE;  /* unknown - returned if can not build a word */

    if (!isAlloc(operands) || !isAlloc(memory) || (operands[0] = readWord(line, lInd)) == NULL)
    { /* reading the operation name */
        if (operands != NULL)
            freeArr(operands, opAmnt)
                free(memory);
        return unxErr;
    }
    if (strcmp(operands[0], "\0"))
    {
        printf("error [line %d]: after the end of a label must come a directive or an instruction\n", lineCnt);
        return NULL;
    }
    /* there is no possible situation in which the operands[0] is equal to "\0" -> chaking in compiler.c */
    if ((operationInd = getOperationIndex(operands[0], lineCnt)) == -1)
        return NULL;
    jumpSpaces(line, lInd);
    while ((operands[opAmnt] = readWord(line, lInd)) != NULL && !strcmp(operand[opAmnt], "\0"))
    { /* reading the operands of the command to an array */
        operands = realloc(operands, ++opAmnt * sizeof(char *));
        if (!isThereComma(line, lInd, lineCnt) || !isAlloc(operands))
        {
            free(memory);
            if (operands != NULL)
            {
                freeArr(operands, opAmnt) return unxErr;
            }
            return NULL;
        }
    }
    if (opAmnt == 0)
    {
    }
    if (operands[opAmnt] == NULL || opAmnt - 1 > 2)
    { /* could not read the last operand or there are more than 2 operands */
        freeArr(operands, opAmnt)
            free(memory);
        if (opAmnt - 1 > 2)
        {
            printf("error [line %d]: to an instruction command can be sent at most: a source and a trget operands\n", lineCnt);
            return NULL;
        }
        return unxErr;
    }

    if (!isValidOpsAmount(operationInd, opAmnt - 1, lineCnt))
    { /* the operands amount does not correspond to the needed amount of operands */
        freeArr(operands, opAmnt)
            free(memory);
        return NULL;
    }
    if (opAmnt - 1 == 1)
    {
        TAM = getAddressingMethod(operands[1]);
        if (isValidTAM(operationInd, TAM, lineCnt))
        {
            if (TAM == immidiate)
                memory[1] = getDecWord(operands[1], lineCnt);
            else
                memory[1] = unknown;
            if(!isValidOperand())
                return NULL;
        }
        else
            return NULL;
    }
    if (opAmnt - 1 < 2)
    { /* if operand amount is 0 or 1 */
        memory[0] = buildMainWord(instsList[operationInd].opcode, instsList[operationInd].funct, TAM, SAM);
        IC += opAmnt;
        freeArr(operands, opAmnt) return memory;
    }
    /* opAmnt == 2 */
    SAM = getAddressingMethod(operands[1]); /* source operand addressing method */
    TAM = getAddressingMethod(operands[2]);
    if (isValidSAM(operationInd, SAM, lineCnt && isValidTAM(operationInd, TAM, lineCnt)))
    {
        if (SAM == immidiate)
            memory[1] = getDecWord(operands[1], lineCnt);
        else
            memory[1] = unknown;
        if (TAM == immidiate)
            memory[2] = getDecWord(operands[2], lineCnt);
        else
            memory[2] = unknown;
    }
    else
        return NULL;
    memory[0] = buildMainWord(instsList[operationInd].opcode, instsList[operationInd].funct, TAM, SAM);
    IC += opAmnt;
    freeArr(operands, opAmnt) return memory;
}

static boolean isValidOpsAmount(int operationInd, int opAmount, int lineCnt)
{
    if (instsList[operationInd].ops == opAmount)
        return TRUE;
    printf("error [line %d]: wrong number of operands for the operation \"%s\"\n", lineCnt, operands[0]);
    printf("\t this operation demandes exactly %d operands\n", instsList[operationInd].ops);
    return FALSE;
}

static boolean isValidSAM(int operandInd, int AM, int lineCnt)
{
    int i;
    for (i = 0; (instsList[operandInd].SVAMs)[i] != -1 && i < MVAM; i++)
    {
        if ((instsList[operandInd].SVAMs)[i] == AM)
            return TRUE;
    }
    printf("error [line %d]: the addressing method for the source operand is not valid\n", lineCnt);
    return FALSE;
}

static boolean isValidTAM(int operandInd, int AM, int lineCnt)
{
    int i;
    for (i = 0; (instsList[operandInd].TVAMs)[i] != -1 && i < MVAM; i++)
    {
        if ((instsList[operandInd].TVAMs)[i] == AM)
            return TRUE;
    }
    printf("error [line %d]: the addressing method for the target operand is not valid\n", lineCnt);
    return FALSE;
}

static boolean isValidOperand(char *operand, int AM, int lineCnt){
    switch (AM){
    case relative:
        char *temp = operand + 1;
        return !isIlegalName(temp) && isValidLabel(operand, lineCnt, FALSE);
    case registerDirect:
    default:
        return direct;
    }
    return TRUE;
}

static int getOperationIndex(char *operation, int lineCnt)
{
    int i;
    for (i = 0; i < INSTRUCTIONS; i++)
    {
        if (strcmp(instructions[i].name, operation))
            return i;
    }
    printf("error [line %d]: invalid operation (instruction command) \"%s\" name\n", lineCnt, operation);
    return -1;
}

static int getAddressingMethod(char *operand, int lineCnt)
{
    switch (operand[0])
    {
    case '#':
        if (strlen(operand) > 1)
            return immidiate;
        printf("error [line %d]: after the addressing method sign '#' must come a decimal number (with no spaces between)\n");
        break;
    case '%':
        if (strlen(operand) > 1)
            return relative;
        printf("error [line %d]: after the addressing method sign '%' must come a label (with no spaces between)\n");
        break;
    case 'r':
        int i;
        for (i = 0; i < REGISTERS; i++)
        {
            if (strcmp(REGISTER_NAME(i), operand))
                return registerDirect;
        }
    default:
        return direct;
    }
    return -1;
}

void cleanIC(boolean terminate)
{
    int i = 0;
    while (1)
    {
        if (i < IC - OS_MEM)
            free(instImage[i]);
        i++;
    }
    IC = OS_MEM;
    if (terminate)
    {
        free(instImage);
        return;
    }
    *instImage = NULL;
    return;
}

boolean isIlegalName(char *label, int lineCnt)
{
    int i, size = 4;
    boolean error = FALSE;
    char *directives[size] = {".data", ".string", ".extern", ".entry"};

    for (i = 0; i < REGISTERS; i++)
    {
        if (strcmp(REGISTER_NAME(i), label))
        {
            printf("error [line %d]: label should not be named as a register\n", lineCnt);
            return TRUE;
        }
        if (i <= size)
        {
            if (strcmp(directives[i], label))
            {
                printf("error [line %d]: label should not be named as a directive\n", lineCnt);
                return TRUE;
            }
        }
    }
    for (i = 0; i < INSTRUCTIONS; i++)
    {
        if (strcmp(instsList[i].name, label))
        {
            printf("error [line %d]: label must not be named as an instruction\n", lineCnt);
            return TRUE;
        }
    }
    /*if(wasDefined(label)){
        printf("error [line %d]: label has been already defined\n",lineCnt);
        return TRUE;
    }*/
    return FALSE;
}

int getIC()
{
    return IC;
}

void pushInstsToFile(FILE *fd)
{
    int i;
    for (i = 0; i < IC - OS_MEM; i++)
        fprintf(fd, "%04d %x %c\n", i + OS_MEM, (*instImage)[i].wrd, (*instImage)[i].ARE);
    return;
}