#include "project.h"
#include "global.h"

/* the method writes the three output files */
void writef(char name[])
{
    FILE * fdob, * fdex, * fden;
    int line = MEMORY_ADRRES;
    int i;
    int j;
    char nameob[MAX_FILE_NAME];
    char nameen[MAX_FILE_NAME];
    char nameex[MAX_FILE_NAME];
    strcpy(nameob, name);
    strcat(nameob, ".ob");
    strcpy(nameex, name);
    strcat(nameex, ".ext");
    strcpy(nameen, name);
    strcat(nameen, ".ent");
    fdob = fopen(nameob, "w");
    if (numext != 0)
        fdex = fopen(nameex, "w");
    if (numen != 0)
        fden = fopen(nameen, "w");
    for (i = 0; i < numext; i++) /* print the file .ext */
    {
        fprintf(fdex, "\n%s ", extPrint[i].name);
        for (j = 1; j < 10000000; j *= 10)
            if ((extPrint[i].line / j) == 0)
                fprintf(fdex, "%d", 0);
        fprintf(fdex, "%d", extPrint[i].line);
    }
    for (i = 0; i < nums; i++) /* print the file .ent */
    {
        if (symbol_table[i].entry == TRUE)
        {
            fprintf(fden, "\n%s ", symbol_table[i].name);
            for (j = 1; j < 10000000; j *= 10)
                if ((symbol_table[i].addres / j) == 0)
                    fprintf(fden, "%d", 0);
            fprintf(fden, "%d", symbol_table[i].addres);
        }
    }
    fprintf(fdob, "%7d %d", ICF - MEMORY_ADRRES, DCF); /* print the finals: IC, DC */
    for (i = 0; i < numc; i++)
    {
        char c;
        int j;
        int t;
        int bit = 20;
        mem musk;
        fprintf(fdob, "\n");
        musk.memory = 15;
        for (j = 1; j < 10000000; j *= 10) /* print the addreses of lines */
            if ((line / j) == 0)
                fprintf(fdob, "%d", 0);
        fprintf(fdob, "%d ", line);
        musk.memory <<= bit;
        while (musk.memory) /* convert to hexa */
        {
            t = (code_tabel[i].mem1.memory & musk.memory);
            t >>= bit;
            if (t > 9)
            {
                switch (t)
                {
                case(10):c = 'a';
                    break;
                case(11):c = 'b';
                    break;
                case(12):c = 'c';
                    break;
                case(13):c = 'd';
                    break;
                case(14):c = 'e';
                    break;
                case(15):c = 'f';
                    break;
                }
                fprintf(fdob, "%c", c);
            }
            else
                fprintf(fdob, "%d", t);

            musk.memory >>= 4;
            bit -= 4;
        }
        line++;
    }
    for (i = 0; i < numd; i++) /* print the data */
    {
        char c;
        int j;
        int t;
        int bit = 20;
        mem musk;
        fprintf(fdob, "\n");
        musk.memory = 15;
        for (j = 1; j < 10000000; j *= 10) /* print the addreses of lines */
            if ((line / j) == 0)
                fprintf(fdob, "%d", 0);
        fprintf(fdob, "%d ", line); /* convert to hexa */
        musk.memory <<= bit;
        while (musk.memory)
        {
            t = (wd[i].memory & musk.memory);
            t >>= bit;
            if (t > 9)
            {
                switch (t)
                {
                	case(10):c = 'a';
                    	break;
                	case(11):c = 'b';
                    	break;
                	case(12):c = 'c';
                	    break;
                	case(13):c = 'd';
                	    break;
                	case(14):c = 'e';
                	    break;
                	case(15):c = 'f';
                	    break;
                }
                fprintf(fdob, "%c", c);
            }
            else
                fprintf(fdob, "%d", t);
            musk.memory >>= 4;
            bit -= 4;
        }
        line++;
    }
    fclose(fdob);
    if (numext != 0)
        fclose(fdex);
    if (numen != 0)
        fclose(fden);
}

