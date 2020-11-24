#include "project.h"
#include "global.h"

/* the second passage */
int stage2(FILE* fd2)
{
    boolean notError = TRUE;
    int line = 1;
    while (feof(fd2) == 0)
    {
        char arrline[MAX_LINE];
        char* entrys;
        char* la;
        char* cur;
        int i;
        int h;
        int j = 0;
        type  wt;
        fgets(arrline, MAX_LINE, fd2);
        if (feof(fd2) == 1)
        {
            break;
        }
        cur = arrline; /* pointer to the current position in row */
        h = 0;
        wt = wordType(cur, &h);      
        if (wt == LABEL) 
        {
            la = getlable(cur, &j); /* get the name of the label */
            cur += strlen(la) + j + 1; /* updates the current position in the row - after the label */
            h = 0;
            wt = wordType(cur, &h); /* skips the label */
        }
        if (wt == NOTE || wt == END) /* chek if the line is an empty line */
            line++;
        else if ((wt == EXTERN) || (wt == DATA_S) || (wt == DATA_N)) 
            line++;
        else if (wt == ENTRY) /* add to 'entry' table */
        {
            numen++;
            cur += (6 + h);
            entrys = stringWord(cur);
            i = 0;
            if (notintable(symbol_table, &i, entrys))
            {
                printf("\neror: in line %d export a non-existent label\n", line);
                notError = FALSE;
            }
            else
            {
                symbol_table[i].entry = TRUE;/*Icon not found*/
            }
            line++;
        }
        else
        {
            if (lineOfCode2(cur, line) == FALSE)/*Handles a line of code*/
                notError = FALSE;
            line++;
        }
    } /* end of while */
    if (notError == FALSE)
    {
        printf("error exit2");
        return 1;
    }
    return 0;
}

/*return the next word*/
char* stringWord(char* start)
{
    int i = 0;
    static char str[MAX_LABEL + 2];
    while ((*start == '\t') || (*start == ' '))
        start++;
    while ((*start != ' ') && (*start != '\t') && (*start != '\n'))
    {
        str[i] = *start;
        start++;
        i++;
    }
    str[i] = '\0';
    return str;
}


/*Handles a line of code*/
boolean lineOfCode2(char* start, int line)
{
    boolean thernteerror = TRUE;
    int i;
    int j;
    int command;
    char strA[5];
    while ((*start == ' ') || (*start == '\t'))
        start++;
    for (i = 0; ((*start != ' ') && (*start != '\t') && (*start != '\n')); i++, start++)
        strA[i] = *start;
    strA[i] = '\0';
    for (j = 0; j < 16; j++)
    {
        if (strcmp(actionshash[j].name, strA) == 0)
        {
            command = j;
        }
    }
    enterCode(line);
    thernteerror = checkAddress2(command, start, line);/*Checks addressing method and whether it is legal*/
    return thernteerror;
}

/*Inserts a row in the code table*/
void enterCode(int line)
{
    IC2++;
    if (numc == 0)
        code_tabel = (word*)malloc(sizeof(word));
    else
        code_tabel = (word*)realloc(code_tabel, (numc + 1) * sizeof(word));
    if (code_tabel == NULL)
    {
        printf("\nerror: in line %d \" Allocation failed \"  \n", line);
        exit(1);
    }
    numc++;
}


boolean checkAddress2(int action, char* start, int line)
{
    boolean notError = TRUE;
    boolean minus = FALSE;
    int jumpLines;
    int add;
    char wordw[MAX_LABEL + 2];
    char label[MAX_LABEL + 1];
    int sum = 0;
    int operand = 0;
    char ch;
    int i = 0;
    int code;
    int ICa = IC2 - 1;
    code = numc - 1;
    code_tabel[code].w1.opcode = actionshash[action].opcode;
    code_tabel[code].w1.funch = actionshash[action].funch;
    code_tabel[code].w1.ARE = A;
    while ((actionshash[action].addressing[operand][0] == 0) && (actionshash[action].addressing[operand][1] == 0) &&
        (actionshash[action].addressing[operand][2] == 0) && (actionshash[action].addressing[operand][3] == 0) && (operand < 2))/*If there is no operand for the command*/
    {
        fullop(operand, code, 0, 0);/*Enter zeros for the addressing and register method*/
        operand++;
    }
    while (operand < 2)
    {
        while ((*start == ' ') || (*start == '\t'))
            start++;
        ch = *start;
        i = 0;
        while ((*start != ' ') && (*start != '\t') && (*start != '\n') && (*start != ','))
        {
            wordw[i] = *start;
            i++;
            start++;
        }
        wordw[i] = '\0';
        if ((ch == 'r') && ((wordw[1] - '0') <= 7) && ((wordw[1] - '0') >= 0) && (wordw[2] == '\0'))/*This is a register*/
        {
            fullop(operand, code, (wordw[1] - '0'), 3);

        }
        else if (ch == '#')/*Immediate address*/
        {
            fullop(operand, code, 0, 0);
            i = 1;
            if ((wordw[i]) == '-')
            {
                minus = TRUE;
                i++;
            }
            for (; i < strlen(wordw); i++)
                sum = (sum * 10) + (wordw[i] - '0');
            if (minus == TRUE)
                sum *= -1;
            enterCode(line);
            code_tabel[numc - 1].mem1.memory = ((sum << 3) + A);
        }
        else if (ch == '&')/*Relative addressing*/
        {
            strcpy(label, wordw + 1);
            i = 0;
            if (notintable(symbol_table, &i, label))
            {
                printf("\nerror: in line %d The label %s does not exist", line, label);
                notError = FALSE;
            }
            else
            {
                fullop(operand, code, 0, 2);
                jumpLines = (symbol_table[i].addres - ICa);
                enterCode(line);
                code_tabel[numc - 1].mem1.memory = ((jumpLines << 3) + A);
            }
        }
        else
        {
            i = 0;
            if (notintable(symbol_table, &i, wordw))
            {
                printf("\nerror: in line %d unidentified label", line);
                notError = FALSE;
            }
            else
            {
                add = symbol_table[i].addres;
                if (symbol_table[i].isextern == TRUE)/*We will add to the external table*/
                {
                    if (numext == 0)
                        extPrint = (ext*)malloc(sizeof(ext));
                    else
                        extPrint = (ext*)realloc(extPrint, (numext + 1) * sizeof(ext));
                    if (extPrint == NULL)
                    {
                        printf("\nerror: in line %d \" Allocation failed \"  \n", line);
                        exit(1);
                    }
                    numext++;
                    strcpy(extPrint[numext - 1].name, symbol_table[i].name);
                    extPrint[numext - 1].line = IC2;
                }
                fullop(operand, code, 0, 1);
                enterCode(line);
                if (symbol_table[i].isextern == TRUE)
                    code_tabel[numc - 1].mem1.memory = (add << 3) + E;
                else
                    code_tabel[numc - 1].mem1.memory = (add << 3) + R;
            }
        }
        operand++;
        while ((*start == ' ') || (*start == '\t'))
            start++;
        if (operand == 1)
        {
            ch = *start;
            if (ch == ',')
                start++;
            else
            {
                printf("\nerror: in line %d missing comma\n", line);
                return FALSE;
            }
        }
    } /* end of while */
    while ((*start == ' ') || (*start == '\t'))
        start++;
    if (*start != '\n')
    {
        printf("\n error: in line %d Unnecessary operand \n", line);
        return FALSE;
    }
    return notError;
}

/*Fills the bits of the address*/
void fullop(int operand, int numIC, int hamster, int address)
{
    if (operand == 0)
    {
        code_tabel[numIC].w1.source_hamster = hamster;
        code_tabel[numIC].w1.source_address = address;
    }
    else
    {
        code_tabel[numIC].w1.target_hamster = hamster;
        code_tabel[numIC].w1.target_address = address;
    }
}

