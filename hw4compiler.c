// Estefania Sanchez
// es102053
// COP 3402, Summer 2020

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lex.h"
#include "parser.h"
#include "codegen.h"
#include "vm.h"

int lexFlag = 0;
int codeGenFlag = 0;
int vmFlag = 0;
FILE *ifp;
//token list[5000];

const char *lexSymbol[] = {"", "nulsym", "identsym", "numbersym", "plussym",
													 "minussym", "multsym", "slashsym", "oddsym", "eqsym",
													 "neqsym", "lessym", "leqsym", "gtrsym", "geqsym",
													 "lparentsym", "rparentsym", "commasym", "semicolonsym",
													 "periodsym", "becomessym", "lbracesym", "rbracesym",
													 "ifsym", "thensym", "whilesym", "dosym", "callsym",
													 "constsym", "varsym", "procsym", "writesym", "readsym",
													 "elsesym"};

void compile(char *filename)
{
	FILE *ifp;
	char c;
	int i;

	printf("\n\nInput File:\n");
	ifp = fopen(filename, "r");
	while (fscanf(ifp, "%c", &c) != EOF)
  {
    printf( "%c", c);
  }
  printf("\n\n");
	fclose(ifp);

	token *list = runLex(filename);
	symbol *table = runParse(list);
	instruction *code = runCodeGen(table, list);
	runVirtualMachine();

	if (lexFlag == 1)
	{
		ifp = fopen("lexOutput.txt", "r");
		while (fscanf(ifp, "%c", &c) != EOF)
		{
			printf("%c", c);
		}
		printf("\n");
		fclose(ifp);
	}

	if (errorType == 0)
	{
			printf("No errors, program is syntactically correct.\n\n");
	}

	if (codeGenFlag == 1)
	{
		ifp = fopen("codegenOutput.txt", "r");
		while (fscanf(ifp, "%c", &c) != EOF)
		{
			printf("%c", c);
		}
		printf("\n");
		fclose(ifp);
	}
	if (vmFlag == 1)
	{
		ifp = fopen("vmExecution.txt", "r");
		while (fscanf(ifp, "%c", &c) != EOF)
		{
			printf("%c", c);
		}
		printf("\n");
		fclose(ifp);
	}
}

int main(int argc, char *argv[])
{
	int i;

	for (i = 2; i < argc; i++)
	{
		if (strcmp(argv[i], "-l") == 0)
			lexFlag = 1;
		if (strcmp(argv[i], "-a") == 0)
			codeGenFlag = 1;
		if (strcmp(argv[i], "-v") == 0)
			vmFlag = 1;
	}

	compile(argv[1]);
}
