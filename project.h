#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define MAX_LINE 80 /* maximum chars per line */
#define MAX_LABEL 31 /* maximum number of labels */
#define MEMORY_ADRRES 100
#define MAX_FILE_NAME 30

typedef enum { FALSE, TRUE } boolean;
enum { CODE, DATA };
enum { A=4, R=2, E=1 }; /* ARE */

typedef enum {
    LABEL, DATA_N, DATA_S, EXTERN, ENTRY, END, NOTE, OTHER
}type; /* 
LABEL = a label - symbol
DATA_N = .data with number after
DATA_S = .data with string after
EXTERN = .extern
ENTRY = .entry
END = empty line
NOTE = note
OTHER = non of the above */

typedef struct { /* stores commands such as mov, cmp, inc etc. */
    char name[5]; /* 4 chars + '0' */
    int opcode; /* code between 0 and 15 */
    int funch; /*The function value of the operation */
    int addressing[2][4]; /* table of 2*4 permittable source/destinations operands: immediate, direct, indirect, register */
}actions;

typedef struct { /* we store labels in this structure */
    char name[MAX_LABEL];
    unsigned int addres : 24; /* address of label */
    unsigned int kind : 1; /* is it CODE or DATA */
    unsigned int isextern : 1; /* if it's an .extern label put 1 else 0 */
    unsigned int entry : 1; /* if it's an .entry label put 1 else 0 */ 
}symbol_struct;

typedef struct { /* Size of memory for command prompt In machine language */
unsigned int memory : 24; 
}mem;

typedef struct { /* command word */
    unsigned int ARE : 3;
    unsigned int funch : 5;
    unsigned int target_hamster : 3;
    unsigned int target_address : 2;
    unsigned int source_hamster : 3;
    unsigned int source_address : 2;
    unsigned int opcode : 6;
}w;

typedef union { /* word to print */
    w w1;
    mem mem1;
}word;

typedef struct { /* we store external labels in this structure */
    char name[MAX_LABEL];
    int line;
}ext;

/* These are just statements will be explained where they are */
ext* extPrint;
word* code_tabel;
char* getlable(char start[], int* j);
boolean islabel(char[]);
boolean notintable(symbol_struct*, int*, char[]);
type wordType(char* start, int* h);
void printerror(int, char*);
boolean lineOfData(char*, int, int);
boolean lineOfString(char*, char*, int);
void allocation_data(type data, char ch, int num, int line);
char* legal_symbol(char* start, int length);
boolean enterInTable(char* namel, int addresl, int kindl, boolean external, int line);
int checkAddress(int action, char* start, int line);
boolean lineOfCode(char* start, int length, int line);
boolean checkAddress2(int action, char* start, int line);
boolean lineOfCode2(char* start, int line);
char *stringWord(char* cur);
void enterCode(int line);
int stage1(FILE* fd);
int stage2(FILE* fd);
void writef(char name[]);
void fullop(int operand,int numIC,int hamster,int address);


