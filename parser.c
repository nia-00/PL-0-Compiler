// Estefania Sanchez
// es102053
// COP 3402, Summer 2020

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"



// global variables
void factor(int level, token *list, symbol *table);
void block(int level, token *list, symbol *table);
int currentToken, num, kind;
char id[12];


void printSymTable(symbol *table)
{
    int i;
    printf("TABLEINDEX = %d\n", tableIndex);
    for (i = 0; i < tableIndex + 1; i++)
    {
        printf("Kind = %d Name = %s Level = %d Addy = %d Mark = %d\n", table[i].kind, table[i].name, table[i].level, table[i].addy, table[i].mark);
    }
    printf("\n");
}

int getNextToken(token *tokenList)
{
    currentToken = tokenList[tokenIndex].id;
    if (currentToken == 2)
        strcpy(id, tokenList[tokenIndex].name);
    else if (currentToken == 3)
        num = tokenList[tokenIndex].number;

    tokenIndex++;
    return currentToken;
}

void tableEnter(int k, int level, int m, int mark, symbol *table)
{
    tableIndex++;

		strcpy(table[tableIndex].name, id);
    table[tableIndex].kind = k;
    fflush(stdout);
    if (k == 1) //const
		{
        table[tableIndex].val = num;
				table[tableIndex].level = 0;
    }
    else //var and proc
		{
        table[tableIndex].level = level;
				table[tableIndex].addy = m;
    }
		table[tableIndex].mark = mark;
}

int tableLookup(int level, int enterFlag, symbol *table)
{
	int i;

	for (i = tableIndex; i > 0; i--)
	{
      if (enterFlag == 1)
      {
			    if (table[i].level == level && strcmp(id, table[i].name) == 0)
			    {
					    if (table[i].mark == 1)
					    {
						      continue;
					    }
					        return i;
			    }
      }
      else
      {
        if (table[i].level <= level && strcmp(id, table[i].name) == 0)
        {
            if (table[i].mark == 1)
            {
                continue;
            }
                return i;
        }
      }
	}

	return -1;
}

void tableDelete(int numDelete, symbol *table)
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


void term(int level, token *list, symbol *table)
{
    factor(level, list, table);
    while(currentToken == multsym || currentToken == slashsym)
		{
        currentToken = getNextToken(list);
        factor(level, list, table);
    }
}

void expression(int level, token *list, symbol *table)
{
    		if (currentToken == plussym || currentToken == minussym)
        {
        		currentToken = getNextToken(list);
        }

				term(level, list, table);

    		while (currentToken == plussym || currentToken == minussym)
				{
						currentToken = getNextToken(list);
        		term(level, list, table);
    		}
}

void factor(int level, token *list, symbol *table)
{
		int i;
		if (currentToken == identsym)
		{
    		i = tableLookup(level, 0, table);
        if (i == -1)
				{
        		error(11); // undeclared identifier
        }
				if (table[i].kind == 3)
				{
						error(21); // Expression must not contain a procedure identifier
				}
        currentToken = getNextToken(list);
    }
    else if (currentToken == numbersym)
		{
        currentToken = getNextToken(list);
    }
    else if (currentToken == lparentsym)
		{
        currentToken = getNextToken(list);
        expression(level, list, table);
        if (currentToken == rparentsym)
            currentToken = getNextToken(list);
        else
            error(22); // Right parenthesis missing.
    }
		else
		{
				error(14); // call must be followed by an identifier
		}
}

void condition(int level, token *list, symbol *table)
{
 		if (currentToken == oddsym)
		{
     		currentToken = getNextToken(list);
        expression(level, list, table);
    }
    else
		{
        expression(level, list, table);
        if ((currentToken != eqsym) && (currentToken != neqsym) && (currentToken != lessym) &&
						(currentToken != leqsym) && (currentToken != gtrsym) && (currentToken != geqsym))
				{
            error(20); //Relational operator expected.
        }
        currentToken = getNextToken(list);
        expression(level, list, table);
    }
}

void statement(int level, token *list, symbol *table)
{
    int temp;

    if (currentToken == identsym)
		{
				if ((temp = tableLookup(level, 0, table)) == -1)
				{
            // ***
						error(11); // not in symbol table
				}
				if (table[temp].kind != 2) // checking if const
				{
						error(12); // assignment to const not allowed
				}

        currentToken = getNextToken(list);

        if (currentToken != becomessym)
            error(13); //Assignment operator expected.

				currentToken = getNextToken(list);
        expression(level, list, table);
				return;
    }
		// for call
		else if (currentToken == callsym)
		{
				currentToken = getNextToken(list);
				if (currentToken != identsym)
				{
						error(14); // Call must be followed by an identifier
				}
				if ((temp = tableLookup(level, 0, table)) == -1)
				{
						error(11); // not in symbol table
				}
				if (table[temp].kind != 3) // 3 = procedure
				{
						error(15); // Call of a constant or variable is meaningless.
				}
				currentToken = getNextToken(list);
				return;
		}
    //for if... then...
    else if (currentToken == ifsym)
		{
        currentToken = getNextToken(list);
        condition(level, list, table);
        if (currentToken != thensym)
            error(16);  // then expected

				currentToken = getNextToken(list);
        statement(level, list, table);

				if (currentToken == elsesym)
				{
            currentToken = getNextToken(list);
						statement(level, list, table);
				}
				return;
    }
    // for begin... end...
    else if (currentToken == beginsym)
		{
        currentToken = getNextToken(list);
        statement(level, list, table);

        while (currentToken == semicolonsym)
			 	{
        		currentToken = getNextToken(list);
            statement(level, list, table);
        }

        if (currentToken != endsym)
        {
            error(17);  //Semicolon or end expected.
        }
				currentToken = getNextToken(list);
				return;
    }
    // for while... do...
    else if (currentToken == whilesym)
		{
        currentToken = getNextToken(list);
        condition(level, list, table);

				if(currentToken != dosym)
            error(18);  // do expected

				currentToken = getNextToken(list);

        statement(level, list, table);
        return;
    }
    // for write
    else if (currentToken == writesym)
		{
				currentToken = getNextToken(list);
				expression(level, list, table);
				return;
    }
    // for read and sto
    else if (currentToken == readsym)
		{
        currentToken = getNextToken(list);
				if (currentToken != identsym)
				{
						error(14); // Call must be followed by an identifier
				}
        if ((temp = tableLookup(level, 0, table)) == -1)
				{
            error(11); //Undeclared identifier.
				}
        if (table[temp].kind != 2) //var
				{
            error(12); //Assignment to constant or procedure is not allowed
				}
				currentToken = getNextToken(list);
				return;
    }
}

int constDeclare(int level, token *list, symbol *table)
{
		int constNum = 0;

		if (currentToken != constsym)
    {
				return constNum;
    }

		do
		{
				constNum++;
				currentToken = getNextToken(list);

    		if (currentToken != identsym)
				{
						error(4); // const, int, proc, missing identifier after
				}
				if (tableLookup(level, 1, table) != -1) // 0 meaning not found in table
				{
						error(30); // constant already declared in sym table.
				}
				currentToken = getNextToken(list);
				if (currentToken == becomessym)
				{
						error(1); // use = instead of :=
				}
        if (currentToken != eqsym)
        {
            error(3); // Identifier must be followed by =
        }
				currentToken = getNextToken(list);
				if (currentToken != numbersym)
				{
						error(2); // = not followed by number
				}
				tableEnter(1, level, 0, 0, table); //1 for const
				currentToken = getNextToken(list);
		}while (currentToken == commasym);

		if (currentToken != semicolonsym)
		{
				error(5); // semicolon or comma missing
		}
		currentToken = getNextToken(list);
		return constNum;
}

int varDeclare(int level, token *list, symbol *table)
{
		int varNum = 0;
		//currentToken = getNextToken(list);

		if (currentToken != varsym)
				return varNum;

		do
		{
				varNum++;
				currentToken = getNextToken(list);
    		if (currentToken != identsym)
				{
						error(4); // c, v, p not followed by identifier
				}
				if (tableLookup(level, 1, table) != -1)
				{
						error(30); // already in symbol table
				}
    		tableEnter(2, level, varNum + 3, 0, table); // 2 for var
    		currentToken = getNextToken(list);
		}while (currentToken == commasym);

		if (currentToken != semicolonsym)
		{
				error(5); // semicolon or comma missing
		}
		currentToken = getNextToken(list);
		return varNum;
}

int procDeclare(int level, token *list, symbol *table)
{
		int temp, numProc = 0;

		if (currentToken != procsym)
				return numProc;

		do
		{
				numProc++;
				currentToken = getNextToken(list);
				if (currentToken != identsym)
				{
						error(4); // // c, v, p not followed by identifier
				}
				if ((temp = tableLookup(level, 1, table)) != -1)
				{
						error(30); // already in symbol table
				}
				tableEnter(3, level - 1, 0, 0, table);
				currentToken = getNextToken(list);
				if (currentToken != semicolonsym)
				{
						error(5); // semicolon or comma missing
				}
				currentToken = getNextToken(list);
        if (currentToken != constsym && currentToken != varsym)
        {
            error(6); // Incorrect symbol after procedure declaration.
        }
				block(level, list, table);
				if (currentToken != semicolonsym)
				{
						error(5); // semicolon or comma missing
				}
				currentToken = getNextToken(list);
		}while (currentToken == procsym);
    return numProc;
}

void block(int level, token *list, symbol *table)
{
    int numSymbols = 0;
		numSymbols += constDeclare(level, list, table);
    numSymbols += varDeclare(level, list, table);
		numSymbols += procDeclare(level + 1, list, table);
    statement(level, list, table);
		tableDelete(numSymbols, table);
}

void program(token *list, symbol *table)
{
    currentToken = getNextToken(list);
    block(0, list, table);
    if (currentToken != periodsym)
        error(9); //Period expected.
		currentToken = getNextToken(list);
		if (currentToken != 0)
			error(31); // code following period.
}

symbol *runParse(token *tokenList)
{
    int i, j;
    tableIndex = 0;
		tokenIndex = 0;
		symbol *table = malloc(MAX_SYMBOL_TABLE_SIZE * sizeof(symbol));
    strcpy(table[0].name, "\0");
    // start program
    program(tokenList, table);
		return table;

}
