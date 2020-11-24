#include "project.h"
#include "global.h"

/* The first passage: */
int stage1(FILE* fd)
{
    boolean notError = TRUE;
    int c;
    int line = 1;
    while (feof(fd) == 0)
    {
        char arrline[MAX_LINE]; /* array that picks up the current row */ 
        char arrlabel[MAX_LABEL]; /* array that picks up the label (if any) */  
        char* la;
        char* plabel;
        char* cur;
        int end_of_line;
        int h;
        int flag_label = FALSE; /* flag of label */
        int j = 0;
        char* lastchar; /* pointer to the last character in the row */
        type  wt;
        fgets(arrline, MAX_LINE, fd); /* picks up the current line */
        if (feof(fd) == 1)  /* if this is the end of the input, we will exit the loop */
            break;
        cur = arrline; /* pointer to the current position in row */
        end_of_line = strlen(arrline);
        lastchar = (cur + end_of_line - 2);
        h = 0;
        wt = wordType(cur, &h);
        if (wt == LABEL)
        {
            flag_label = TRUE;
            la = getlable(cur, &j); /* get the label */
            strcpy(arrlabel, la);
            end_of_line -= (strlen(arrlabel) + j + 1);
            cur += strlen(arrlabel) + j + 1; /* updates the current position in the row - after the label */
        }
        h = 0;
        wt = wordType(cur, &h);
        if (wt == NOTE || wt == END)
            line++;
        else if (wt == DATA_N || wt == DATA_S)
        {
            if (flag_label == TRUE)
                if (enterInTable(arrlabel, DC, DATA, FALSE, line) == FALSE)
                    notError = FALSE;
            if (wt == DATA_N)
                if (lineOfData(cur + h, end_of_line, line) == FALSE)
                    notError = FALSE;
            if (wt == DATA_S)
                if (lineOfString(cur + h, lastchar, line) == FALSE)
                    notError = FALSE;
            line++;
        }
        else if (wt == ENTRY)
            line++;
        else if (wt == EXTERN)
        {
            plabel = legal_symbol(cur + h, end_of_line); /* enter in 'plabel' the outer label */
            if (plabel == NULL)
            {
                printf("\nerror: in line %d invalid external label\n ", line);
                notError = FALSE;
            }
            else
                if (enterInTable(plabel, 0, -1, TRUE, line) == FALSE)
                    notError = FALSE;
            line++;
        }
        else
        {
            if (flag_label == TRUE)
                enterInTable(arrlabel, IC, CODE, FALSE, line);
            if (lineOfCode(cur, end_of_line, line) == FALSE)
                notError = FALSE;
            line++;
        }
    } /* end of line of code */
    if (notError == FALSE)
    {
        printf("errors exsit");
        return 1;
    }
    ICF = IC;
    DCF = DC;
    for (c = 0; c < nums; c++)
    {
        if (symbol_table[c].isextern == FALSE)
            if (symbol_table[c].kind == DATA)
                symbol_table[c].addres += ICF;
    }
    return 0;
}



/* goes through a data type line */
boolean lineOfData(char* cur, int length, int line)
{
    char ch;
    int sum;
    boolean plus;
    boolean minus;
    boolean digit;
    boolean legal = TRUE;
    cur += 5; /* '.data' = 5 characters */
    ch = *cur;
    while (length)
    {
        sum = 0;
        plus = FALSE;
        minus = FALSE;
        digit = FALSE;
        while ((ch == ' ' || ch == '\t'))
        {
            ch = *(++cur);
            length--;
        }
        if (ch == '+')
        {
            plus = TRUE;
            ch = *(++cur);
            length--;
        }
        if ((ch == '-') && (plus == FALSE))
        {
            minus = TRUE;
            ch = *(++cur);
            length--;
        }
        while (isdigit(ch))
        {
            digit = TRUE;
            sum = (sum * 10) + (ch - '0');
            ch = *(++cur);
            length--;
        }
        if ((ch == ' ') || (ch == '\t') || (ch == ',') || (ch == '\n'))
        {
            if (minus == TRUE)
                sum *= (-1);
            if (digit == FALSE)
                legal = FALSE;
        }
        if (legal == TRUE)
        {
            allocation_data(DATA_N, '0', sum, line);
            DC++;
        }
         else
	{
	    printf("\nerror: in line %d Missing number between the two commas\n",line);
                  return FALSE;
	}
        while ((ch == ' ' || ch == '\t') && (length > 0))
        {
            ch = *(++cur);
            length--;
        }
        if (ch == '\n')
        {
            if (legal == TRUE)
                return TRUE;
            return FALSE;
        }
        if (ch == ',')
        {
            ch = *(++cur);
            length--;
        }
        else
        { 
            printf("\nerror: in line %d Missing comma\",\"",line);
                  return FALSE;
        }
             
    }
    return TRUE;
}

/* allocates space for data */
void allocation_data(type data, char ch, int num, int line)
{
    if (numd == 0)
    {
        wd = (mem*)malloc(sizeof(mem));
        if (wd == NULL)
        {
            printf("\nerror: in line %d \" Allocation failed \"  \n", line);
            exit(1);
        }
    }
    else
    {
        wd = (mem*)realloc(wd, (numd + 1) * sizeof(mem));
        if (wd == NULL)
        {
            printf("\nerror: in line %d \" Allocation failed \"  \n", line);
            exit(1);
        }
    }
    if (data == DATA_N)
        wd[numd].memory = num;
    if (data == DATA_S)
        wd[numd].memory = ch;
    numd++;
}

/* goes through a string type line */
boolean lineOfString(char* cur, char* last, int line)
{
    char* left;
    char* right;
    cur += 7; /* '.string' = 7 characters */
    left = cur;
    right = last;
    while (((*left) == ' ') || ((*left) == '\t'))
        left++;
    while (((*right) == ' ') || ((*right) == '\t'))
        right--;

    if ((*right != '"') || (*left != '"') || (right == left))
    {
        printf("error: in line %d missing: \" ", line);
        return FALSE;
    }
    for (left += 1; left < right; left++)
    {
        allocation_data(DATA_S, *left, 0, line);
        DC++;
    }
    allocation_data(DATA_S, '\0', 0, line);
    DC++;
    return TRUE;
}



/* goes through a code type line */
boolean lineOfCode(char* start, int length, int line)
{
    int L;
    boolean legalname = FALSE;
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
            legalname = TRUE;
            command = j;
        }
    }
    if (legalname == FALSE)
    {
        printf("\nerror: in line %d invalid command\n", line);
        return FALSE;
    }
    else
    {
        L = checkAddress(command, start, line);
        if (L == -1)
            return FALSE;
        IC += L;
        return TRUE;
    }
}

/* check addres of operands and return the number of command lines */
int checkAddress(int action, char* start, int line)
{
    int add = 5;
    char wordw[MAX_LABEL + 2];
    int L = 0;
    int operand = 0;
    char ch;
    int i = 0;
    while ((actionshash[action].addressing[operand][0] == 0) && (actionshash[action].addressing[operand][1] == 0) &&
        (actionshash[action].addressing[operand][2] == 0) && (actionshash[action].addressing[operand][3] == 0) && (operand < 2))
        operand++;
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
        if ((ch == 'r')  && (wordw[2] == '\0'))
        {
			if(((wordw[1] - '0') <= 7) && ((wordw[1] - '0') >= 0))
			{
				add = 3;
			}
			else
			{
				printf("\n error: in line %d Invalid register \n", line);
                return -1;
			}
		}
        else if (ch == '#')
        {
            if (!((isdigit(wordw[1])) || (wordw[1] == '-')))
            {
                printf("\n error: in line %d invalid character in immediate address \n", line);
                return -1;
            }
            for (i = 2; i < strlen(wordw); i++)
            {
                if (!(isdigit(wordw[i])))
                {
                    printf("\n error: in line %d invalid character in immediate address \n", line);
                    return -1;
                }
            }
            add = 0;
        }
        else if (ch == '&')
            add = 2;
        else
            add = 1;
        if (actionshash[action].addressing[operand][add] == 0)
        {
            if (operand == 0)
                printf("\nerror: in line %d The address method of the source operand is invalid\n", line);
            else
                printf("\nerror: in line %d The address method of the target operand is invalid\n", line);
            return -1;
        }
        switch (add)
        {
        case 0:
            L += 1;
            break;
        case 1:
            L += 1;
            break;
        case 2:
            L += 1;
            break;
        case 3:
            L += 0;
            break;
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
                return -1;
            }
        }
    } /* end of while */
    while ((*start == ' ') || (*start == '\t'))
        start++;
    if (*start != '\n')
    {
        printf("\n error: in line %d Unnecessary operand \n", line);
        return -1;
    }
    return L + 1;
}

