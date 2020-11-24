/*Methods for transitions*/
#include "project.h"
#include "global.h"


/* return the label */
char* getlable(char start[], int* j)
{
    static char label[MAX_LABEL];
    int i;
    while ((*start == ' ') || (*start == '\t'))
    {
        start++;
        (*j)++;
    }
    for (i = 0; (start[i] != ':'); i++)
        label[i] = start[i];
    label[i] = '\0';
    return label;
}



/* enter a symbol into symbols table */
boolean enterInTable(char* namel, int addresl, int kindl, boolean external, int line)
{
    int i = 0;
    if ((notintable(symbol_table, &i, namel)) == FALSE)
    {
        if (external == TRUE)  
        {      
		if (symbol_table[i].isextern == FALSE)
    		{
			printf("\nerror: in line %d  An internal label with this name already exists\n", line);
			return FALSE;
		}
        }
        else
	{
             printf("\nerror: in line %d  the label already exists\n", line);
             return FALSE;
	}
    }
    if ((nums++) == 0)
    {
        symbol_table = (symbol_struct*)malloc(sizeof(symbol_struct));
        if (symbol_table == NULL)
        {
            printf("\nerror in line %d Allocation failed\n", line); exit(1);
        }
    }
    else
    {
        symbol_table = (symbol_struct*)realloc(symbol_table, nums * sizeof(symbol_struct));
        if (symbol_table == NULL)
        {
            printf("\nerror in line %d Allocation failed\n", line);
            exit(1);
        }
    }
    strcpy(symbol_table[nums - 1].name, namel);
    symbol_table[nums - 1].addres = addresl;
    symbol_table[nums - 1].isextern = external;
    symbol_table[nums - 1].entry = FALSE;
    if (kindl != -1)
        symbol_table[nums - 1].kind = kindl;
    return TRUE;
}


/* check if label is legal */
boolean islabel(char word[])
{
    int i;
    if (word[strlen(word) - 1] != ':')
        return FALSE;
    if (isalpha(word[0]))
    {
        for (i = 1; i < (strlen(word) - 1); i++)
            if ((!(isalpha(word[i]))) && (!(isdigit(word[i]))))
                return FALSE;
        return TRUE;
    }
    return FALSE;
}


/* check the type of the current word */
type wordType(char* start, int* h)
{
    char str[MAX_LABEL];
    int i;
    while ((*start == ' ') || (*start == '\t'))
    {
        start++;
        (*h)++;
    }
    if (*start == '\n')
        return END;
    if (*start == ';')
        return NOTE;
    for (i = 0; (*start != ' ') && (*start != '\t') && (*start != '\n'); i++, start++)
        str[i] = *start;
    str[i] = '\0';
    if ((strcmp(str, ".data")) == 0)
        return DATA_N;
    if ((strcmp(str, ".string")) == 0)
        return DATA_S;
    if ((strcmp(str, ".extern")) == 0)
        return EXTERN;
    if ((strcmp(str, ".entry")) == 0)
        return ENTRY;
    if (islabel(str))
        return LABEL;
    return OTHER;
}


/* check if symbol is'nt in symbol-table, holds a pointer to the position of the label in the table */
boolean notintable(symbol_struct* table, int* num, char namel[])
{
    int i;
    for (i = 0; i < nums; i++)
    {
        if (strcmp(table[i].name, namel) == 0)
            return FALSE;
        (*num)++;
    }
    return TRUE;
}
  

/* check if extern label is legal */
char* legal_symbol(char* start, int length)
{
    static char str[MAX_LABEL];
    int i;
    while (((*start) == ' ') || ((*start) == '\t'))
        start++;
    start += 7;
    while ((*start == ' ') || (*start == '\t'))
        start++;
    for (i = 0; (*start != ' ') && (*start != '\t') && (*start != '\n'); i++, start++)
        str[i] = *start;
    str[i] = ':';
    str[i + 1] = '\0';
    if (islabel(str))
    {
        str[i] = '\0';
        return str;
    }
    return NULL;
}
