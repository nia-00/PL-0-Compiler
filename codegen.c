// Estefania Sanchez
// es102053
// COP 3402, Summer 2020

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "codegen.h"

void factorCG(int level, token *list, symbol *table);
int getNextTokenCG(token *tokenList);

int tableIndex, codeIndex, tokenIndex, num, currentToken;
instruction code[MAX_CODE_LENGTH];
char id[12];

void emit(int op, int l, int m, instruction *code) {

    if (codeIndex > MAX_CODE_LENGTH)
        error(29);
    else
		{
        code[codeIndex].op = op; 	//opcode
        code[codeIndex].l = l;	// lexicographical level
        code[codeIndex].m = m;	// modifier
        codeIndex++;
    }
}

int getNextTokenCG(token *tokenList)
{
    currentToken = tokenList[tokenIndex].id;
    if (currentToken == 2)
        strcpy(id, tokenList[tokenIndex].name);
    else if (currentToken == 3)
        num = tokenList[tokenIndex].number;

    tokenIndex++;
    return currentToken;
}

void tableDeleteCG(int numDelete, symbol *table)
{
    int i;
	  for (i = tableIndex; i >= 0; i--)
	  {
		    if (table[i].mark == 0)
			  {
				    table[i].mark = 1;
				    numDelete--;
			  }
			  if (numDelete == 0)
			  {
					  return;
			  }
	  }
}

void termCG(int level, token *list, symbol *table)
{
    factorCG(level, list, table);
    while(currentToken == multsym || currentToken == slashsym)
		{
        if(currentToken == multsym)
				{
						currentToken = getNextTokenCG(list);
						factorCG(level, list, table);
						emit(2, 0, 4, code); // 2 is OPR, 4 is MUL
				}
        else
				{
						currentToken = getNextTokenCG(list);
						factorCG(level, list, table);
						emit(2, 0, 5, code); // 2 is OPR, 4 is DIV
				}
    }
}

void expressionCG(int level, token *list, symbol *table)
{
    if (currentToken == plussym )
		{
        currentToken = getNextTokenCG(list);
    }
    if (currentToken == minussym)
    {
        currentToken = getNextTokenCG(list);
        termCG(level, list, table);
        emit(2, 0, 1, code); // 2 is OPR, 1 is NEG
        while (currentToken == plussym || currentToken == minussym)
    		{
            if (currentToken == plussym)
            {
                currentToken = getNextTokenCG(list);
                termCG(level, list, table);
                emit(2, 0, 2, code); // 2 is OPR, 2 is ADD
            }
            else
            {
                currentToken = getNextTokenCG(list);
                termCG(level, list, table);
                emit(2, 0, 3, code); // 2 is OPR , 3 is SUB
            }
        }
        return;
    }

    termCG(level, list, table);

    while (currentToken == plussym || currentToken == minussym)
    {
        if (currentToken == plussym)
        {
            currentToken = getNextTokenCG(list);
            termCG(level, list, table);
            emit(2, 0, 2, code); // 2 is OPR, 2 is ADD
        }
        else
        {
            currentToken = getNextTokenCG(list);
            termCG(level, list, table);
            emit(2, 0, 3, code); // 2 is OPR , 3 is SUB
        }
    }
}

void conditionCG(int level, token *list, symbol *table)
{
    int switchToken;
 		if (currentToken == oddsym)
		{
     		currentToken = getNextTokenCG(list);
        expressionCG(level, list, table);
     		emit(2, 0, 6, code); // 2 is OPR, 6 is ODD
    }
    else //for relational operators
		{
        expressionCG(level, list, table);
       	switchToken = currentToken;
       	currentToken = getNextTokenCG(list);
       	expressionCG(level, list, table);
        switch(switchToken)
				{
        		case 9:
            		emit(2,0,8, code); // 2 is OPR, 8 is EQL
                break;
            case 10:
                emit(2,0,9, code); // 2 is OPR, 9 is NEQ
                break;
            case 11:
                emit(2,0,10, code); // 2 is OPR, 10 is LSS
                break;
            case 12:
                emit(2,0,11, code); // 2 is OPR, 11 is LEQ
                break;
            case 13:
                emit(2,0,12, code); // 2 is OPR, 12 is GTR
                break;
            case 14:
                emit(2,0,13, code); // 2 is OPR, 13 is GEQ
                break;
				}
		}
}

void factorCG(int level, token *list, symbol *table)
{
    int i, temp, temp2;
    if (currentToken == identsym)
		{
        for (i = tableIndex; i >= 0; i--)
        {
            if (strcmp(table[i].name, id) == 0 && table[i].kind != 3 && table[i].mark == 0)
            {
                temp = i;
                break;
            }
            temp = i;
        }
				if (table[temp].kind == 2)
				{
						emit(3, level - table[temp].level, table[i].addy, code);
				}
				if (table[temp].kind == 1)
				{
						emit(1, 0, table[temp].val, code);
				}
        currentToken = getNextTokenCG(list);
    }
    else if (currentToken == numbersym)
		{
        emit(1, 0, num, code); // 1 is LIT, num is M
        currentToken = getNextTokenCG(list);
    }
    else
		{
        currentToken = getNextTokenCG(list);
        expressionCG(level, list, table);
		    currentToken = getNextTokenCG(list);
    }
}

void statementCG(int level, token *list, symbol *table)
{
    int i, temp, temp2;

    if (currentToken == identsym)
		{
        for (i = tableIndex; i > 0; i--)
        {
            if (strcmp(table[i].name, id) == 0 && table[i].mark == 0)
            {
                temp = i;
                break;
            }
            temp = i;
        }

        currentToken = getNextTokenCG(list);
        currentToken = getNextTokenCG(list);
        expressionCG(level, list, table);
        emit(4, level - table[temp].level, table[temp].addy, code);
    }
    //for if... then... else...
    if (currentToken == ifsym)
		{
        currentToken = getNextTokenCG(list);
        conditionCG(level, list, table);
        temp = codeIndex;
        emit(8, 0, 0, code); // 8 is JPC, 0 is  L and 0 is M
        currentToken = getNextTokenCG(list);
        statementCG(level, list, table);
        if (currentToken == elsesym)
        {
            currentToken = getNextTokenCG(list);
            temp2 = codeIndex;
            emit(7, 0, 0, code); // JMP
            code[temp].m = codeIndex;
            statementCG(level, list, table);
            code[temp2].m = codeIndex;
        }
        else
            code[temp].m = codeIndex;
    }
    // for begin... end...
    if (currentToken == beginsym)
		{
        currentToken = getNextTokenCG(list);
        statementCG(level, list, table);

         while (currentToken == semicolonsym)
				 {
            currentToken = getNextTokenCG(list);
            statementCG(level, list, table);
         }
         currentToken = getNextTokenCG(list);
         currentToken = getNextTokenCG(list);
    }
    // for while... do...
    if (currentToken == whilesym)
		{
        currentToken = getNextTokenCG(list);
        temp = codeIndex;
        conditionCG(level, list, table);
        currentToken = getNextTokenCG(list);
        temp2 = codeIndex;
        emit(8, 0, 0, code); // 8 is JPC, 0 is L and 0 is M
        statementCG(level, list, table);
        emit(7, 0, temp, code); // 7 is JMP, 0 is L and temp is M, jump to instruction temp
        code[temp2].m = codeIndex;
    }
    // for write
    if (currentToken == writesym)
		{
        currentToken = getNextTokenCG(list);
        expressionCG(level, list, table);
        emit(9,0,1, code); // 9 is SIO1, 0 is L and 1 is M, write top stack element to screen
    }
    // for read and sto
    if (currentToken == readsym)
		{
        currentToken = getNextTokenCG(list);

        for (i = tableIndex; i >= 0; i--)
        {
            if (strcmp(table[i].name, id) == 0 && table[i].mark == 0)
            {
                temp = i;
                break;
            }
            temp = i;
        }

        currentToken = getNextTokenCG(list);
        fflush(stdout);
        emit(10,0,2, code); // 10 is SIO2, 0 is L and 1 is M, write top stack element to screen
        emit(4, level - table[temp].level, table[temp].addy, code); // 4 is STO
    }
		if (currentToken == callsym)
		{
        currentToken = getNextTokenCG(list);
        for (i = tableIndex; i >= 0; i--)
        {
            if (table[i].kind == 3 && strcmp(table[i].name, id) == 0 && table[i].mark == 0)
            {
                temp = i;
                break;
            }
            temp = i;
        }
				emit(5, level - table[i].level, table[i].addy, code);
        currentToken = getNextTokenCG(list);
		}

}

void blockCG(int level, token *list, symbol *table)
{
    int i, n = 0, v = 0;
		if (currentToken == constsym)
		{
				while (table[tableIndex].kind == 1)
				{
						n++;
						table[tableIndex].mark = 0;
						tableIndex++;
						for (i = 0; i < 4; i++)
						{
								currentToken = getNextTokenCG(list);
						}
				}
				currentToken = getNextTokenCG(list);
		}
		if (currentToken == varsym)
		{
				while (table[tableIndex].kind == 2)
				{
						n++;
						v++;
						table[tableIndex].mark = 0;
						tableIndex++;
						for (i = 0; i < 2; i++)
						{
								currentToken = getNextTokenCG(list);
						}
				}
				currentToken = getNextTokenCG(list);
		}
		while (currentToken == procsym)
		{
				currentToken = getNextTokenCG(list);
        currentToken = getNextTokenCG(list);
        currentToken = getNextTokenCG(list);
				n++;
				table[tableIndex].mark = 0;
				table[tableIndex].addy = codeIndex;
				tableIndex++;
				blockCG(level + 1, list, table);
				emit(2, 0, 0, code); // return
		}

		emit(6, 0, 4 + v, code); // inc
    statementCG(level, list, table);
		tableDeleteCG(n, table);
}

void programCG(token *list, symbol *table)
{
		int i, j, k, procCount = 0;

		emit(7, 0, 0, code); // JMP to main

		for (i = 0; i <= tableIndex; i++)
		{
				if (table[i].kind == 3)
				{
						procCount++;
						emit(7, 0, 0, code); // JMP to main
				}
		}

    currentToken = getNextTokenCG(list);
    tableIndex = 1;
    blockCG(0, list, table);

		j = codeIndex;
		i = 0;

		while (j >= 0)
		{
				if (code[j].op == 6)
				{
						code[i].m = j;
            i++;
            j--;
						break;
				}
				j--;
		}
    k = 0;
    while (k < j)
    {
        if (code[k].op == 6)
        {
            code[i].m = k;
            i++;
        }
        k++;
    }


    emit(11, 0, 3, code); // End of programCG
}


instruction *runCodeGen(symbol *table, token* list)
{
    FILE* ofp = fopen("parserOutput.txt", "w");
    int i;
		tokenIndex = 0;
		codeIndex = 0;

    // start programCG
    programCG(list, table);

    //prints instructions to file
    for (i=0; i<codeIndex;i++)
		{
        fprintf(ofp, "%d %d %d\n", code[i].op, code[i].l, code[i].m);
    }

    fclose(ofp);
}
