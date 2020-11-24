#include "project.h"
/*project made by:
Reitzy acker   315262642 
 
project goal is to translate assembly files to octal/binary files which 
are used by the linker program for assembly.
Input files are given in command arguments i.e. maman14 ps cs a rs
the program will append .as to the file names for input.
if tranlation encounters error it will send them to stderr and stop the translation at end of file.
if no errors it will produce 3 files for each input file:
1) file.ob - object code
2) file.ent - entries label codes
3) file.ext - external label codes
*/

actions actionshash[16] = { /* table of all commands which holds: string, code, number of arguments, permittable params in source destination.
	the 4 permittable params are: immediate, direct, indirect, register. 1 means permitted 0 means not permitted. */
    {"mov",0,0,{{0,1,0,1},{1,1,0,1}}},
    {"cmp",1,0,{{1,1,0,1},{1,1,0,1}}},
    {"add",2,1,{{1,1,0,1},{0,1,0,1}}},
    {"sub",2,2,{{1,1,0,1},{0,1,0,1}}},
    {"lea",4,0,{{0,1,0,0},{0,1,0,1}}},
    {"clr",5,1,{{0,0,0,0},{0,1,0,1}}},
    {"not",5,2,{{0,0,0,0},{0,1,0,1}}},
    {"inc",5,3,{{0,0,0,0},{0,1,0,1}}},
    {"dec",5,4,{{0,0,0,0},{0,1,0,1}}},
    {"jmp",9,1,{{0,0,0,0},{0,1,1,0}}},
    {"bne",9,2,{{0,0,0,0},{0,1,1,0}}},
    {"jsr",9,3,{{0,0,0,0},{0,1,1,0}}},
    {"red",12,0,{{0,0,0,0},{0,1,0,1}}},
    {"prn",13,0,{{0,0,0,0},{1,1,0,1}}},
    {"rts",14,0,{{0,0,0,0},{0,0,0,0}}},
    {"stop",15,0,{{0,0,0,0},{0,0,0,0}}}
};  
int IC;
int DC;
int ICF;
int DCF;
int nums; /* number of symbols */
int numd; /* number of lines of data */
int numc; /* number of lines of code */
int numen; /* number of symbols = entry */
int numext; /* number of symbols = extern */
int IC2; /* IC To the second passage */
symbol_struct* symbol_table; /* pointer to the symbol table */
mem* wd; /* pointer to the storage location of the external symbols - that we need to print - in memory */
ext* extPrint; /* pointer to the storage location of the data lines in memory */
word* code_tabel;

int main(int argc, char* argv[])
{
    FILE* fd;
    int error;
    char name[MAX_FILE_NAME];
    char namer[MAX_FILE_NAME];
    if (argc == 1)
        printf("\nYou did not enter a file name.\n");
    while (--argc)
    {
        IC = MEMORY_ADRRES;
        DC = 0;
        nums = 0;
        numd = 0;
        numc = 0;
        numen = 0;
        numext = 0;
        IC2 = MEMORY_ADRRES;
        strcpy(name, argv[argc]);
        strcpy(namer, name);
        strcat(namer, ".as");
        fd = fopen(namer, "r");
        if (fd == NULL)
            printf("\nThe file\"%s.as\"does not exist.\n", name);
        else
        {
            error = stage1(fd);
            if (error == 0)   /*there is not error*/
            {
                rewind(fd);
                error = stage2(fd);
                if (error == 0)   /*there is not error*/
                    writef(name);
            }
            fclose(fd);
        }
    }
    return 0;
}
